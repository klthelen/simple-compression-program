#include "bitsy.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){
  unsigned short l;                   // Unsigned short to read bytes into
  unsigned char buf[8];               // Hold 8 read characters
  int readSF = 0;                     // How many bytes we've read so far
  int index = 0;                      // Index in our recently read bytes array
  unsigned char eB, rB;               // Unsigned chars to hold truncated unsigned shorts
  unsigned short hold, b1, b2, b3;    // Temporary unsigned shorts used to hold values 
  int position, repetition;           // Used to save position and repetition number of bytes

  // Read a bit
  while((l = readBit()) < 256) {  
    eB = l;                 // Store bit in unsigned char
    position = -1;          // For recently seen bytes
    repetition = -1;        // For repetition amongst bytes
    
    if (eB == 1) {          // <1>
      hold = readByte();    // Read a byte
      if (hold > 255) {     // EOF()
        exit(1);
      }
      rB = hold;
      buf[index] = rB;      // Store the byte
      index++;
      if (index > 7) {      // At the end of the buffer, circle back [X ~ ~ ~ ~ ~ ~ ~]
        index = 0;
      }
      readSF++;             // We've read 1 more byte
      if (readSF > 8) {
        readSF = 8;
      }
    }
    // Read the next bit
    else {                  // <0>
      l = readBit();
      if (l > 255) {        // EOF(), this shouldn't happen
	      exit(1);
      }
      eB = l;               // 0 or 1
      // Recent, non-repeating OR Recent, repeating
      // Read next 3 bits for position
      b1 = readBit(); // 0 or 1
      b2 = readBit(); // 0 or 1
      b3 = readBit(); // 0 or 1

      // Find position
      if (b1 == 0) {        // 0XX
	      if (b2 == 0) {      // 00X
	        if (b3 == 0) {    // 000
	          position = 0;   // => 0
          }
	        else {            // 001
	          position = 1;   // => 1
          }
      	}
        else {              // 01X
          if (b3 == 0) {    // 010
            position = 2;   // => 2
          }
          else {            // 011
            position = 3;   // => 3
          }
        }
      }
      else {                // 1XX
        if (b2 == 0) {      // 10X
          if (b3 == 0) {    // 100
            position = 4;   // => 4
          }
          else {            // 101
            position = 5;   // => 5
          }
        }
        else {              // 11X
          if (b3 == 0) {    // 110
            position = 6;   // => 6
          }
          else {            // 111
            position = 7;   // => 7
          }
        }
      }
      
      // Now we have <0><0 or 1><postion>
      // If our 2nd bit was 1, there is repetition. Otherwise, move on
      if (eB == 1) { // Repetition. How many times?
        // Read next 3 bits for repetition
        b1 = readBit(); // 0 or 1
        b2 = readBit(); // 0 or 1
        b3 = readBit(); // 0 or 1
	
	
	      // Find repetition
	      if (b1 == 0) {          // 0XX
	        if (b2 == 0) {        // 00X
	          if (b3 == 0) {      // 000
	            repetition = 0;   // => 0
            }
            else {              // 001
              repetition = 1;   // => 1
            }
          }
	        else {                // 01X
	          if (b3 == 0) {      // 010
	            repetition = 2;   // => 2
            }
            else {              // 011
              repetition = 3;   // => 3
            }
          }
    	  }
        else {                  // 1XX
          if (b2 == 0) {        // 10X
            if (b3 == 0) {      // 100
              repetition = 4;   // => 4
            }
            else {              // 101
              repetition = 5;   // => 5
            }
          }
          else {                // 11X
            if (b3 == 0) {      // 110
              repetition = 6;   // => 6
            }
            else {              // 111
              repetition = 7;   // => 7
            }
          }
        }
      }
    }
    
    // Write our byte
    if (position == -1) {                 // Infrequent
      writeByte(rB);
    }
    // Find byte in our most recent bytes (up to 8)
    else {                                // Some form of frequent
      int found = index - position - 1;   // index - position - 1 = where the byte is located
      if (found < 0) {                    // Adjust if we go off of our circular array
	      found = found + readSF;           // a negative value + readSF will take us to the correct spot in the array
      }
      // Storage
      buf[index] = buf[found];
      index++;
      if (index > 7) {                    // At the end of the buffer, circle back [X ~ ~ ~ ~ ~ ~ ~]
        index = 0;
      }
      readSF++;                           // We've read 1 more byte
      if (readSF > 8) {
        readSF = 8;
      }
      
      // Recent, regardless if its repeating or not, we can write the byte at least once
      writeByte(buf[found]);
      // Recent, repeating will write up to an additional 8 times (repetition == 0 means it will write another 1 time)
      if (repetition > -1) {      
        // Write our byte as many times as it repeats
        // Recall: a reptition of 0 means the byte will be printed a total of 2 times 
        for(int i = 0; i <= repetition; i++) {
          writeByte(buf[found]);
        }
      }
    }
  }
    
  return 0; 
}
