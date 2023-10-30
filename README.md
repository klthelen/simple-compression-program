
# simple-compression-program

An implementation of a simple compression algorithm. 

This program produces two binaries, `czy` and `dzy`. czy is the compression program and dzy is the decompression program. 

# The Algorithm

The algorithm reads a file one byte at a time and compares it with the previous 8 bytes read. It checks how many of the following characters (up to 8) are the same as the current byte. If the byte has been previously seen, it outputs the position of the previous byte relative to the current position as well as the number of times it repeats. Otherwise, the byte is output prefxied with a binary 1. <br/>

Although this algorithm does not save us as much space as other algorithms, it is very simple to understand and implement.

### Simple example 1

For example, if a file named `ab.txt` contains only the text `ab`, its binary (which can be viewed with `xxd -b ab.txt`) is `01100001 01100010`. The compression algorithm would read the first byte ('a') and, after noticing that it has not been read before and will not be read again in the next 8 bytes, will output `101100001` (9 bits). It will do the same with the next byte ('b') resulting in a compressed file with the contents `101100001 101100010`.<br/>

Note that in order to adhere to proper byte format, the above binary will be padded with trailing 1s, meaning that the compressed file's entire contents will be: `10110000 11011000 10111111`.<br/>

### Simple example 2

If a file named `abaaa.txt` contains only the text `abaaa`, its binary is `01100001 01100010 01100001 01100001 01100001`. The compression algorithm does the same thing to the first two bytes ('a' and 'b') as it did in example 1, but it will have a different behavior for the following bytes.

Upon reading the 3rd character ('a'), the algorithm checks and notices that this character has been read in the previous 8 bytes. It also looks ahead and notices that it will repeat for the next 2 bytes. Instead of outputting the actual binary value of the character, it will output bits in the format `<0><1><psn><count>` where `<0>` means that this byte has recently been read, `<1>` means this byte is repeating, `<psn>` is the offset to where this byte was actually read previously and `<count>` is the number of times it repeats (where 0 (000) means it repeats 1 time and 7 (111) means it repeats 8 times).<br/>

Altogether, the contents of this file will be compressed into the following: <br/>
`101100001 101100010 01001001`<br/>

Note that we use only 7 bits in the compressed file to represent 3 bytes in the uncompressed file.

The above bits do not neatly make bytes (9 bits, 9 bits, 8 bits). These same bits redistributed look like the following: <br/>

`10110000 11011000 10010010 01`<br/>

Then, we pad the end of the last bit with 1s, giving us the final compressed file of: </br>

`10110000 11011000 10010010 01111111`<br/>

While this simple algorithm does produce a larger file for a small example like the above, we will notice our compression actually save us space with larger files that use more repeated characters. 


# Testing Compression

You can test the program as follows: <br/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make all` <br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./czy < file_to_compress.extension > compressed_file_name.extension` <br/>

You can then run the compressed file through dzy to test the decompression. <br/>

You can remove all compiled files with: <br/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make clean`</br>

# Testing Decompression

You can test the program as follows, assuming you already have a file compressed by czy: <br/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./dzy < compressed_file_name.extension > new_file_name.extension`</br>

You can then compare your decompressed file with the original file used in czy, either manually or by using cmp: <br/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`cmp file_to_compress.extension new_file_name.extension`</br>

Note that cmp should produce no output if the original (uncompressed) file and the decompressed file are identical (which they should be). 

You can remove all compiled files with: <br/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make clean`</br>

# Example

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make all`</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./czy < czy.c > compressed_czy.c`</br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`./dzy < compressed_czy.c > decompressed_czy.c`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`cmp czy.c decompressed_czy.c`<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`make clean`<br/>

# Compression ratio comparison

**Uncompressed Size / Our Compression Compressed Size**<br/>
bitsy.c&nbsp;: 8152/7383     = 1.104<br/>
czy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 22056/13511   = 1.632<br/>
czy.c&nbsp;&nbsp;&nbsp;&nbsp;: 9083/7679     = 1.183<br/>
dzy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 22056/12648   = 1.744<br/>
dzy.c&nbsp;&nbsp;&nbsp;: 5763/4736     = 1.217<br/>

**Uncompressed Size / GZip Compression**<br/>
bitsy.c&nbsp;: 8152/2558     = 3.187<br/>
czy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 22056/5108    = 4.318<br/>
czy.c&nbsp;&nbsp;&nbsp;&nbsp;: 9083/2409     = 3.770<br/>
dzy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 22056/4740    = 4.653<br/>
dzy.c&nbsp;&nbsp;&nbsp;: 5763/1969     = 2.927<br/>

**Our Compression Compressed Size / GZip Compression**
bitsy.c&nbsp;: 7383/2558     = 2.886<br/>
czy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 13511/5108    = 2.645<br/>
czy.c&nbsp;&nbsp;&nbsp;&nbsp;: 7679/2409     = 3.188<br/>
dzy&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: 12648/4740    = 2.668<br/>
dzy.c&nbsp;&nbsp;&nbsp;: 4736/1969     = 2.405<br/>
