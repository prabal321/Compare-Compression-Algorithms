#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 4096
#define BYTE_SIZE 256

// Node structure for the dictionary
typedef struct {
    int prefix;
    char character;
} DictionaryEntry;

void lzw_compress(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "r");
    FILE *output = fopen(output_file, "wb");

    if (!input || !output) {
        perror("File error");
        return;
    }

    // Initialize dictionary with single character entries
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    for (int i = 0; i < BYTE_SIZE; i++) {
        dictionary[i].prefix = -1;
        dictionary[i].character = (char)i;
    }

    int dict_size = BYTE_SIZE;
    int prefix = fgetc(input);

    while (1) {
        int c = fgetc(input);
        if (c == EOF) break;

        int found = -1;
        for (int i = 0; i < dict_size; i++) {
            if (dictionary[i].prefix == prefix && dictionary[i].character == (char)c) {
                found = i;
                break;
            }
        }

        if (found != -1) {
            prefix = found;
        } else {
            fwrite(&prefix, sizeof(int), 1, output);

            if (dict_size < MAX_DICT_SIZE) {
                dictionary[dict_size].prefix = prefix;
                dictionary[dict_size].character = (char)c;
                dict_size++;
            }

            prefix = c;
        }
    }

    fwrite(&prefix, sizeof(int), 1, output);
    fclose(input);
    fclose(output);
}

void lzw_decompress(const char *input_file, const char *output_file) {
    FILE *input = fopen(input_file, "rb");
    FILE *output = fopen(output_file, "w");

    if (!input || !output) {
        perror("File error");
        return;
    }

    // Initialize dictionary with single character entries
    char *dictionary[MAX_DICT_SIZE];
    for (int i = 0; i < BYTE_SIZE; i++) {
        dictionary[i] = malloc(2);
        dictionary[i][0] = (char)i;
        dictionary[i][1] = '\0';
    }

    int dict_size = BYTE_SIZE;
    int old_code;
    fread(&old_code, sizeof(int), 1, input);
    fputs(dictionary[old_code], output);

    while (1) {
        int new_code;
        if (fread(&new_code, sizeof(int), 1, input) == 0) break;

        char *entry;
        if (new_code < dict_size) {
            entry = dictionary[new_code];
        } else {
            entry = malloc(strlen(dictionary[old_code]) + 2);
            strcpy(entry, dictionary[old_code]);
            entry[strlen(dictionary[old_code])] = dictionary[old_code][0];
            entry[strlen(dictionary[old_code]) + 1] = '\0';
        }

        fputs(entry, output);

        if (dict_size < MAX_DICT_SIZE) {
            dictionary[dict_size] = malloc(strlen(dictionary[old_code]) + 2);
            strcpy(dictionary[dict_size], dictionary[old_code]);
            dictionary[dict_size][strlen(dictionary[old_code])] = entry[0];
            dictionary[dict_size][strlen(dictionary[old_code]) + 1] = '\0';
            dict_size++;
        }

        old_code = new_code;
    }

    fclose(input);
    fclose(output);
}

int main() {
    int choice;
    char input_filename[100];
    char compressed_filename[100];
    char decompressed_filename[100];

    // Ask the user for their choice
    printf("Select operation:\n");
    printf("1. Compress a file\n");
    printf("2. Decompress a file\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    // Perform the appropriate action based on user input
    if (choice == 1) {
        // Ask for input and output file paths
        printf("Enter input file path (e.g., input.dat): ");
        scanf("%s", input_filename);
        printf("Enter output compressed file path (e.g., compressed.bin): ");
        scanf("%s", compressed_filename);
        
        lzw_compress(input_filename, compressed_filename);
        printf("File compressed successfully. Output saved in '%s'.\n", compressed_filename);
    } else if (choice == 2) {
        // Ask for input and output file paths
        printf("Enter input compressed file path (e.g., compressed.bin): ");
        scanf("%s", compressed_filename);
        printf("Enter output decompressed file path (e.g., decompressed.dat): ");
        scanf("%s", decompressed_filename);
        
        lzw_decompress(compressed_filename, decompressed_filename);
        printf("File decompressed successfully. Output saved in '%s'.\n", decompressed_filename);
    } else {
        printf("Invalid choice! Please select either 1 or 2.\n");
    }

    return 0;
}