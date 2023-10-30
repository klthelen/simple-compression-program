#include "bitsy.h"
#include <unistd.h>

/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]) {
	unsigned short l;      // stores readByte
	unsigned char buf[8];  // Hold previous 8 bytes
	int match = -1;        // Flag to see if we have recently seen this byte
	int readSF = 0;        // How many bytes we've read so far (up to 8, used for the first 8 bytes)
	int cou = 0;           // How many times this byte repeats
	unsigned char eB;      // Truncate unsigned short to unsigned char
  
	while ((l = readByte()) < 256) { 	// While we can read a byte
    eB = l;  							// Store byte in an unsigned char (8 bits)
    
	START:					// Used for recent, repeating
		// Check for any matches in our recently read bytes
	    match = -1;                      // No match so far
    	for (int i = 0; i < readSF; i++) {
      		if (buf[i] == eB) {         	    // We've seen this byte in the last 8 bytes read
				match = readSF - i - 1;     // Match is the offset
     		}
    	}
    
	    // At this point we have
   		// k : our currently read byte
   		// match: -1 if not seen in the last 8 bytes or its the offset to the previously seen byte
		// We need to see if this byte repeats further
    
    	if (match != -1) { 					// If a match exists in the previous 8 bytes
      		cou = -1;
      		// Read a byte, if it's not the end of the file, do our standard checking
      		// (Recent, repeating, recent, non-repeating, or non-recent)
      		// We use do while to ensure that the last byte is always read (usually the last byte is a newline, but not always)
      		if ((l = readByte()) < 256) {
				do {
					unsigned char j = l;
					if (eB == j) {
						cou++;				// Found another of the same byte
						if (cou == 8) {
							goto NXT;		// Maximum of 8 repeats
						}
	  				}
	  				else {
						NXT: 				// Used in case there are more than 8 repeats
	    				// Encode as recent, either repeating or non-repeating
							// Recent, repeating
	   						if (cou > -1) {                 
								// Encode: <0><1><psn><count>
								writeBit(0);       			// <0>
								writeBit(1);       			// <1>
								// <psn>
								if (match < 4) { 	   		// 0 1 2 3 
									writeBit(0);
									if (match < 2) {  		// 0 1
										writeBit(0);
										if (match == 0) {
											writeBit(0); 	// 0
										}
										else {
											writeBit(1); 	// 1
										}
									}
									else {           		// 2 3
										writeBit(1);
										if (match == 2) {
											writeBit(0); 	// 2
										}
										else {
											writeBit(1); 	// 3
										}
		      						}
								}
	      						else {             			// 4 5 6 7
									writeBit(1);
									if (match < 6) {  		// 4 5
										writeBit(0);
										if (match == 4) {
		    								writeBit(0); 	// 4
										}
		  								else {
		    								writeBit(1); 	// 5
										}
									}
									else {           		// 6 7
		  								writeBit(1);
		  								if (match == 6) {
		   									writeBit(0);	// 6
										}
		  								else {
		    								writeBit(1); 	// 7
										}
									}
	      						}
								// <count>
								// Recall: If cou == 0, it repeats one time->2 Total If it's non-repeating, it won't be here
	    	 					if (cou < 4) {      			// 0 1 2 3
									writeBit(0);
									if (cou < 2) {    			// 0 1
										writeBit(0);
										if(cou == 0) {
											writeBit(0); 		// 0
										}
										else {
											writeBit(1); 		// 1
										}
									}
									else {           			// 2 3
		  								writeBit(1);
		  								if (cou == 2) {
		    								writeBit(0); 		// 2
										}
										else {
											writeBit(1); 		// 3
										}
									}
	      						}
	     						else {             				// 4 5 6 7
									writeBit(1);
									if (cou < 6) {    			// 4 5
										writeBit(0);
										if (cou == 4) {
											writeBit(0); 		// 4
										}
										else {
											writeBit(1); 		// 5
										}
									}
									else {          			// 6 7
										writeBit(1);
										if (cou == 6) {
											writeBit(0); 		// 6
										}
										else {
											writeBit(1); 		// 7
										}
									}
								}
							}
	    					// Recent, non-repeating
	    					else {                  
	      						// Encode: <0><0><psn>
								writeBit(0);
								writeBit(0);
	      						// <psn>
			      				if (match < 4) {     				// 0 1 2 3 
									writeBit(0);
									if (match < 2) {   				// 0 1
										writeBit(0);
										if (match == 0) {
											writeBit(0);  			// 0
										}
										else {
											writeBit(1);  			// 1
										}
									}
									else {            				// 2 3
				  						writeBit(1);
				  						if (match == 2) {
				    						writeBit(0); 			// 2
										}
				  						else {
				    						writeBit(1);  			// 3
										}
									}
								}
								else {              				// 4 5 6 7
									writeBit(1);
									if (match < 6) {   				// 4 5
										writeBit(0);
		  								if (match == 4) {
											writeBit(0);  			// 4
										}
										else {
											writeBit(1);  			// 5
										}
									}
									else {            				// 6 7
										writeBit(1);
										if (match == 6) {
											writeBit(0);  			// 6
										}
										else {
											writeBit(1);  			// 7
										}
									}
								}
			    			}
	    
			    			// Move our previous 8 bytes
							// If we haven't read 8 yet, put eB directly into storage
			    			if (readSF < 8) {
			      				buf[readSF] = eB; 	
			      				readSF++;
			    			}
							// Else we've read at least 8 bytes
			    			else {
			      				// Move bytes over to make space for eB
								for (int i = 0; i < 7; i++) {
									buf[i] = buf[i+1];
								}
	      						// Insert eB
	      						buf[7] = eB;
	    					}
	    
			    			eB = j;     			// j isn't a duplicate or its > the 8th duplicate, so start the process over with j
	    					goto START; 			// jump back without reading another byte 
					}
				} while((l = readByte()) < 256); 	// Read bytes while not at end of file
	
				// EOF
				if (l > 255) {
					goto EOF;
				}
			}
			// End of file, eB is the last byte
      		else {
      			EOF:
				if (cou > -1) {                  // Recent, repeating
					// Encode: <0><1><psn><count>
					writeBit(0);      	 	// <0>
					writeBit(1);     	  	// <1>
	  				// <psn>
					if (match < 4) {    	// 0 1 2 3 
	   					writeBit(0);
	    				if (match < 2) {  	// 0 1
							writeBit(0);
	      					if (match == 0) {
								writeBit(0); // 0
							}
	      					else {
								writeBit(1); // 1
							}
	    				}
	    				else {           			// 2 3
	     					writeBit(1);
								if (match == 2) {
									writeBit(0); 	// 2
								}
								else {
									writeBit(1); 	// 3
								}
						}
					}
					else {             				// 4 5 6 7
						writeBit(1);
						if (match < 6) {  			// 4 5
							writeBit(0);
							if (match == 4) {
								writeBit(0); 		// 4
							}
							else {
								writeBit(1); 		// 5
							}
						}
	    				else {           			// 6 7
							writeBit(1);
							if (match == 6) {
								writeBit(0); 		// 6
							}
							else {
								writeBit(1); 		// 7
							}
						}
	  				}

	  				// <count>
	  				// Recall: If cou == 0, it repeats one time->2 Total If it's non-repeating, it won't be here
	 				if (cou < 4) {      		// 0 1 2 3
	   					writeBit(0);
	    				if (cou < 2) {    		// 0 1
							writeBit(0);
							if (cou == 0) {
								writeBit(0); 	// 0
							}
							else {
								writeBit(1); 	// 1
							}
						}
						else {		  	        // 2 3
							writeBit(1);
							if (cou == 2) {
								writeBit(0); 	// 2
							}
							else {
								writeBit(1); 	// 3
							}
						}
	  				}
	  				else {             			// 4 5 6 7
	    				writeBit(1);
	   					if (cou < 6) {    		// 4 5
							writeBit(0);
							if (cou == 4) {
								writeBit(0); 	// 4
							}
	      					else {
								writeBit(1); 	// 5
							}
	    				}
					    else {           		// 6 7
	      					writeBit(1);
							if (cou == 6) {
								writeBit(0); 	// 6
							}
							else {
								writeBit(1); 	// 7
							}
						}
					}
				}
				else {
	  				// Encode: <0><0><psn>
					writeBit(0);
					writeBit(0);
					// <psn>
					if (match < 4) { 			// 0 1 2 3 
						writeBit(0);
						if (match < 2) { 		// 0 1
							writeBit(0);
							if (match == 0) {
								writeBit(0); 	// 0
							}
							else {
								writeBit(1); 	// 1
							}
	    				}
	    				else { 					// 2 3
							writeBit(1);
	      					if (match == 2) {
								writeBit(0); 	// 2
							}
	      					else {
								writeBit(1); 	// 3
							}
	    				}
					}
					else { 						// 4 5 6 7
						writeBit(1);
	    				if (match < 6) {  		// 4 5
							writeBit(0);
							if (match == 4) {
								writeBit(0); 	// 4
							}
							else
								writeBit(1); 	// 5
						}
						else { 					// 6 7
							writeBit(1);
							if (match == 6) {
								writeBit(0); 	// 6
							}
							else {
								writeBit(1); 	// 7
							}
						}
					}
				}
			}
		}
		// No match in the previous 8 bytes
		else { 
			// Encode as infrequent: <1><symbol>
			writeBit(1);   						// <1>
			writeByte(eB); 						// <symbol>
      
			// Move our previous 8 bytes
			if(readSF < 8) {
				buf[readSF] = eB; 				// If we haven't read 8 yet, put eB directly into storage
				readSF++;        				// We've now read +1 byte
			}
			else {              				// Else we've read at least 8 bytes
				// Move bytes over to make space for eB
				for(int i = 0; i < 7; i++) {
	  				buf[i] = buf[i+1];
				}
				// Insert eB
				buf[7] = eB;
      		}
    	}
  	}
	flushWriteBuffer();
	return 0; 	// exit status. success=0, error=-1   
}

