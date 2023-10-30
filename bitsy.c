#include "bitsy.h"
#include <unistd.h> // Read, write
#include <stdlib.h> // exit()

struct BByte {
  unsigned b1;
  unsigned b2;
  unsigned b3;
  unsigned b4;
  unsigned b5;
  unsigned b6;
  unsigned b7;
  unsigned b8;
  int pos;
};

struct BByte hByte = {
  .b1 = 0,
  .b2 = 0,
  .b3 = 0,
  .b4 = 0,
  .b5 = 0,
  .b6 = 0,
  .b7 = 0,
  .b8 = 0,
  .pos = 9  // > 8 means read next byte
};

struct BByte bufByte = {
  .b1 = 0,
  .b2 = 0,
  .b3 = 0,
  .b4 = 0,
  .b5 = 0,
  .b6 = 0,
  .b7 = 0,
  .b8 = 0,
  .pos = 0 // 0: buffer is empty, 8: buffer is full
};

/* readByte
 * Abstraction to read a byte.
 * Relys on readBit.
 */
unsigned short readByte(){ 
  /* This function should not call read() directly.
   * If we are buffering data in readBit, we dont want to skip over
   * that data by calling read again. Instead, call readBit and 
   * construct a byte one bit at a type. This also allows us to read
   * 8 bits that are not necessarily byte alligned.
   */
  unsigned rb1, rb2, rb3, rb4, rb5, rb6, rb7, rb8;

  // Reading 8 bits
  rb1 = readBit();
  rb2 = readBit();
  rb3 = readBit();
  rb4 = readBit();
  rb5 = readBit();
  rb6 = readBit();
  rb7 = readBit();
  rb8 = readBit();

  // End of file
  if ((rb1 > 255) || (rb2 > 255) || (rb3 > 255) || (rb4 > 255) || (rb5 > 255) || (rb6 > 255) || (rb7 > 255) || (rb8 > 255)) {
    return 256;
  }
  unsigned char xc;
  xc = ((rb1 << 7) | (rb2 << 6) | (rb3 << 5) | (rb4 << 4) | (rb5 << 3) | (rb6 << 2) | (rb7 << 1) | (rb8));
  return (unsigned short)xc; 
}

/* readBit
 * Abstraction to read a bit.
 */
unsigned short readBit(){
  /* This function is responsible for reading the next bit on the
   * input stream from stdin (fd = 0). To accomplish this, keep a 
   * byte sized buffer. Each time read bit is called, use bitwise
   * operations to extract the next bit and return it to the caller.
   * Once the entire buffered byte has been read the next byte from 
   * the file. Once eof is reached, return a unique value > 255
   */

  // If pos > 7 then we are at the end of current byte so we read the next one
  if (hByte.pos > 7) {
    unsigned char *buf = malloc(sizeof(unsigned char));
    int success = read(0, buf, 1); // Read 1 byte

    // Pack our BByte
    hByte.b1 = (*buf >> 7) &0b1;
    hByte.b2 = (*buf >> 6) &0b1;
    hByte.b3 = (*buf >> 5) &0b1;
    hByte.b4 = (*buf >> 4) &0b1;
    hByte.b5 = (*buf >> 3) &0b1;
    hByte.b6 = (*buf >> 2) &0b1;
    hByte.b7 = (*buf >> 1) &0b1;
    hByte.b8 = *buf &0b1;
    hByte.pos = 1; // 1st position
    free(buf);     // Return memory
    
    if(success == 0) {                  // If at the end of the file -> Return a unique value > 255
      return 256;
    }
    
    return (unsigned short) hByte.b1;   // Return first bit of byte
  }
  else {
    hByte.pos = hByte.pos + 1;          // Increment position
    unsigned short holder;
    
    // Assign bit to correct spot in our structure
    switch(hByte.pos) {
    case(2):
      holder = (unsigned short) hByte.b2;
      hByte.b2 = 0;
      break;
    case(3):
      holder = (unsigned short) hByte.b3;
      hByte.b3 = 0;
      break;
    case(4):
      holder = (unsigned short) hByte.b4;
      hByte.b4 = 0;
      break;
    case(5):
      holder = (unsigned short) hByte.b5;
      hByte.b5 = 0;
      break;
    case(6):
      holder = (unsigned short) hByte.b6;
      hByte.b6 = 0;
      break;
    case(7):
      holder = (unsigned short) hByte.b7;
      hByte.b7 = 0;
      break;
    case(8):
      holder = (unsigned short) hByte.b8;
      hByte.b8 = 0;
      break;
    case(9):       // error, should never be here
      exit(1);
    }
    return holder; // Return assigned bit
  }
  
  // Return a unique value greater than the max byte value to communicate EOF
  return 256; 
}

/* writeByte
 * Abstraction to write a byte.
 */
void writeByte(unsigned char byt){
  /* Use writeBit to write each bit of byte one at a time. Using writeBit
   * abstracts away byte boundaries in the output.
   */
  writeBit((byt >> 7) &0b1); // .b1
  writeBit((byt >> 6) &0b1); // .b2
  writeBit((byt >> 5) &0b1); // .b3
  writeBit((byt >> 4) &0b1); // .b4
  writeBit((byt >> 3) &0b1); // .b5
  writeBit((byt >> 2) &0b1); // .b6
  writeBit((byt >> 1) &0b1); // .b7
  writeBit((byt) &0b1);      // .b8
}

/* writeBit
 * Abstraction to write a single bit.
 */
void writeBit(unsigned char bit){
  /* Keep a byte sized buffer. Each time this function is called, insert the 
   * new bit into the buffer. Once 8 bits are buffered, write the full byte
   * to stdout (fd=1).
   */
  unsigned char xc;
  int success;
  
  // Check if our buffer is full (if bufByte.pos is 8 then all 8 bits are taken)
  if(bufByte.pos == 8) {
    // Buffer is full, so something went wrong (this should never happen)
    exit(100);
  }

  // Find correct bit
  switch(bufByte.pos) {
  case(0):
    bufByte.b1 = bit;
    break;
  case(1):
    bufByte.b2 = bit;
    break;
  case(2):
    bufByte.b3 = bit;
    break;
  case(3):
    bufByte.b4 = bit;
    break;
  case(4):
    bufByte.b5 = bit;
    break;
  case(5):
    bufByte.b6 = bit;
    break;
  case(6):
    bufByte.b7 = bit;
    break;
  case(7):
    bufByte.b8 = bit;
    break;
  }
  bufByte.pos = bufByte.pos + 1; // 1 more bit read
  
  if(bufByte.pos == 8) { // If we've read 8 bits
    // Unpack our BByte
    xc = ((bufByte.b1 << 7) |
	  (bufByte.b2 << 6) |
	  (bufByte.b3 << 5) |
	  (bufByte.b4 << 4) |
	  (bufByte.b5 << 3) |
	  (bufByte.b6 << 2) |
	  (bufByte.b7 << 1) |
	  (bufByte.b8));
    
    success =  write(1, &xc, 1);
    // success is number of bytes written to file (should be 1)
    if(success < 1) {
      // Should never happen but if something goes wrong the program terminates
      exit(100);
    }
    // Reset bufByte position
    bufByte.pos = 0;

    // Reset bits
    bufByte.b1 = 0;
    bufByte.b2 = 0;
    bufByte.b3 = 0;
    bufByte.b4 = 0;
    bufByte.b5 = 1;
    bufByte.b6 = 0;
    bufByte.b7 = 1;
    bufByte.b8 = 0;
  }  
}

/* flushWriteBuffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flushWriteBuffer(){
  /* This will be utilized when finishing encoding. It may be that some bits
   * are still buffered and have not been written to stdout. Call this function 
   * which does the following: 
   * (1) Determine if any buffered bits have yet to be written
   * (2) Pad remaining bits in the byte with 1s
   * (3) Write byte to stdout
   */
  
  // Pad bufByte with trailing 1s
  switch(bufByte.pos) {
  case(0):          // Buffer is empty, nothing to write
    break;
  case(1):          // 1 thing in buffer, pad last 7 spots
    bufByte.b2 = 1; // Pad 2nd spot
  case(2):          // 2 things in buffer, pad last 6 spots
    bufByte.b3 = 1; // Pad 3rd spot
  case(3):          // 3 things in buffer, pad last 5 spots
    bufByte.b4 = 1; // Pad 4th spot
  case(4):          // 4 things in buffer, pad last 4 spots
    bufByte.b5 = 1; // Pad 5th spot
  case(5):          // 5 things in buffer, pad last 3 spots
    bufByte.b6 = 1; // Pad 6th spot
  case(6):          // 6 things in buffer, pad last 2 spots
    bufByte.b7 = 1; // Pad 7th spot
  case(7):          // 7 things in buffer, pad last 1 spot
    bufByte.b8 = 1; // Pad 8th spot
  case(8):          // There is a whole byte in the buffer to write (should not happen)
  default:
    break;
  }

  if((bufByte.pos < 8) && (bufByte.pos > 0)) {
    // bufByte is padded with trailing 1s
    // Assemble byte and write it
    unsigned char xc = ((bufByte.b1 << 7) |
			(bufByte.b2 << 6) |
			(bufByte.b3 << 5) |
			(bufByte.b4 << 4) |
			(bufByte.b5 << 3) |
			(bufByte.b6 << 2) |
			(bufByte.b7 << 1) |
			(bufByte.b8));
    
    int success = write(1, &xc, 1);
    bufByte.pos = 9;
    if(success < 1) {
      // Should never happen unless something goes wrong
      exit(100);
    }
  }
}
