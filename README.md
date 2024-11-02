# Comparing-Various-Compression-Algorithms-on-Diverse-FIle-Types


File Compression and Decompression Program
This repository contains C programs for compressing and decompressing files using three algorithms: Huffman Coding, Lempel-Ziv-Welch (LZW), and Run-Length Encoding (RLE). Additionally, it includes a fourth program, fullcode.c, which combines all features and can automatically select the optimal algorithm based on the file's characteristics.

Files
Individual Algorithms:
huffman.c: Compresses and decompresses files using Huffman Coding.
lzw.c: Compresses and decompresses files using Lempel-Ziv-Welch (LZW).
rle.c: Compresses and decompresses files using Run-Length Encoding (RLE).

Combined Program:
fullcode.c: Allows the user to choose a specific algorithm or automatically selects the most efficient one for compression.

Usage Instructions
1. Prepare Your Input File
Create or select a text file to compress, such as input.txt, and ensure it’s in the same directory as the program files.

2. Choose a Compression Method
Run an individual algorithm or use fullcode.c for automatic algorithm selection. Below are instructions for each.

3. Compression and Decompression
Using Individual Algorithms
Run the Program: Compile and run the algorithm of your choice (e.g., huffman.c).


Enter Compression Details:
Input File: Specify the file you want to compress (e.g., input.txt).
Compressed Output File: Name the compressed file (e.g., compressed_huffman.bin).
Compression Algorithm: Each .c file corresponds to a specific algorithm (Huffman, LZW, or RLE).
Decompression:


Compressed File: Specify the previously generated compressed file (e.g., compressed_huffman.bin).
Decompressed Output File: Name the file for decompressed data (e.g., decompressed_huffman.txt).
Decompression Algorithm: Run the matching algorithm (e.g., if huffman.c was used to compress, use the same file for decompression).




Using the Combined Program (fullcode.c)
Run the Program: Compile and run fullcode.c.
Enter Compression Details:
Input File: Specify the file you want to compress (e.g., input.txt).
Compressed Output File: Provide a name for the compressed file (e.g., compressed_auto.bin).
Algorithm Selection:
Choose an algorithm (Huffman, LZW, or RLE), or select AUTO to let the program determine the best option.
Decompression:
Compressed File: Enter the compressed file generated during compression.
Decompressed Output File: Name the file for decompressed data (e.g., output_auto.txt).
Decompression Algorithm: Use the same algorithm chosen or auto-selected in compression.

