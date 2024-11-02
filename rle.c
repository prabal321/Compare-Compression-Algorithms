#include <stdio.h>
#include <stdlib.h>

// Function to perform RLE compression for binary data
void rle_compress_file(const char* input_file, const char* output_file) {
    FILE *input = fopen(input_file, "rb");
    FILE *output = fopen(output_file, "wb");
    if (!input || !output) {
        perror("File error");
        return;
    }

    int current_char = fgetc(input);
    while (current_char != EOF) {
        int count = 1;
        while (count < 255) {  // Max count for a single byte is 255
            int next_char = fgetc(input);
            if (next_char == current_char) {
                count++;
            } else {
                ungetc(next_char, input);  // Push the character back to the stream
                break;
            }
        }
        fputc(count, output);       // Write the count
        fputc(current_char, output); // Write the character
        current_char = fgetc(input);
    }

    fclose(input);
    fclose(output);
}

// Function to perform RLE decompression for binary data
void rle_decompress_file(const char* input_file, const char* output_file) {
    FILE *input = fopen(input_file, "rb");
    FILE *output = fopen(output_file, "wb");
    if (!input || !output) {
        perror("File error");
        return;
    }

    int count;
    while ((count = fgetc(input)) != EOF) {
        int ch = fgetc(input);
        if (ch == EOF) {
            fprintf(stderr, "File corrupted.\n");
            break;
        }
        for (int i = 0; i < count; i++) {
            fputc(ch, output);
        }
    }

    fclose(input);
    fclose(output);
}

int main() {
    int choice;
    char input_file[256], output_file[256];

    printf("Choose an option:\n");
    printf("1. Compress\n");
    printf("2. Decompress\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character left in the buffer

    printf("Enter the input file path: \nExample: ./input.txt if the file is in the same directory\n");
    scanf("%s", input_file);

    printf("Enter the output file path: \nExample: ./output.txt if the file is in the same directory\n");
    scanf("%s", output_file);

    if (choice == 1) {
        rle_compress_file(input_file, output_file);
        printf("File compressed successfully.\n");
    } else if (choice == 2) {
        rle_decompress_file(input_file, output_file);
        printf("File decompressed successfully.\n");
    } else {
        fprintf(stderr, "Invalid choice! Please select either 1 or 2.\n");
        return 1;
    }

    return 0;
}
