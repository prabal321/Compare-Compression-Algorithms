#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_TREE_HT 100

struct MinHeapNode {
    unsigned char data; // Use unsigned char for binary data
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* createNode(unsigned char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    if (!temp) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    temp->data = data;
    temp->freq = freq;
    temp->left = temp->right = NULL;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    if (!minHeap) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode*)malloc(minHeap->capacity * sizeof(struct MinHeapNode));
    if (!minHeap->array) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return minHeap;
}

void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    minHeap->size++;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

void generateHuffmanCodes(struct MinHeapNode* root, int* arr, int top, char* codes[256]) {
    if (root->left) {
        arr[top] = 0;
        generateHuffmanCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        generateHuffmanCodes(root->right, arr, top + 1, codes);
    }
    if (!(root->left) && !(root->right)) {
        for (int i = 0; i < top; ++i) {
            codes[(unsigned char)root->data][i] = arr[i] + '0';
        }
        codes[(unsigned char)root->data][top] = '\0';
    }
}

struct MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size, char* codes[256]) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    top = extractMin(minHeap);
    int arr[MAX_TREE_HT], topIndex = 0;
    generateHuffmanCodes(top, arr, topIndex, codes);
    return top;
}

void calculateFrequency(FILE* input, unsigned char* data, int* freq, int* size) {
    int freqTable[256] = {0};
    unsigned char ch;

    while (fread(&ch, sizeof(unsigned char), 1, input) == 1) {
        freqTable[ch]++;
    }

    for (int i = 0; i < 256; i++) {
        if (freqTable[i] > 0) {
            data[*size] = (unsigned char)i;
            freq[*size] = freqTable[i];
            (*size)++;
        }
    }
}

void writeBitToBuffer(char bit, FILE *output, unsigned char *buffer, int *bufferIndex) {
    if (*bufferIndex == 8) {
        fwrite(buffer, sizeof(unsigned char), 1, output);
        *bufferIndex = 0;
        *buffer = 0;
    }
    if (bit == '1') {
        *buffer |= (1 << (7 - *bufferIndex)); // set the bit
    }
    (*bufferIndex)++;
}

void encodeFile(FILE* input, FILE* output, char* codes[256], unsigned char* data, int* freq, int size) {
    // Write the size of the frequency table
    fwrite(&size, sizeof(int), 1, output);

    // Write the character frequency table to the output file
    for (int i = 0; i < size; i++) {
        fputc(data[i], output); // Write character
        fwrite(&freq[i], sizeof(int), 1, output); // Write its frequency
    }

    unsigned char ch;
    unsigned char buffer = 0;
    int bufferIndex = 0;
    fseek(input, 0, SEEK_SET);

    while (fread(&ch, sizeof(unsigned char), 1, input) == 1) {
        char *code = codes[ch];
        for (int i = 0; code[i] != '\0'; i++) {
            writeBitToBuffer(code[i], output, &buffer, &bufferIndex);
        }
    }
    if (bufferIndex > 0) {
        fwrite(&buffer, sizeof(unsigned char), 1, output); // Write remaining bits
    }
}

void decodeFile(FILE* input, FILE* output, struct MinHeapNode* root) {
    struct MinHeapNode* current = root;
    unsigned char bit;
    
    while (fread(&bit, sizeof(unsigned char), 1, input) == 1) {
        for (int i = 0; i < 8; i++) {
            if (bit & (1 << (7 - i))) {
                current = current->right;
            } else {
                current = current->left;
            }

            if (!current->left && !current->right) {
                fputc(current->data, output);
                current = root;
            }
        }
    }
}

void freeHuffmanTree(struct MinHeapNode* root) {
    if (root) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        free(root);
    }
}


// RLE Functions

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


// LZW Functions
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



const char* select_best_method(const char* input_file) {
    const char* methods[3] = {"Huffman", "RLE", "LZW"};
    const char* output_files[3] = {"temp_huffman.bin", "temp_rle.bin", "temp_lzw.bin"};

    // Perform compression with each method
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            // Huffman Compression
            char data[BYTE_SIZE];
            int freq[BYTE_SIZE] = {0};
            int size = 0;
            char* codes[BYTE_SIZE];
            for (int j = 0; j < BYTE_SIZE; j++) {
                codes[j] = (char*)malloc(MAX_TREE_HT * sizeof(char));
                codes[j][0] = '\0';
            }

            FILE* input = fopen(input_file, "rb");
            if (input == NULL) {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            calculateFrequency(input, data, freq, &size);
            struct MinHeapNode* root = buildHuffmanTree(data, freq, size, codes);
            FILE* output = fopen(output_files[i], "wb");
            if (output == NULL) {
                perror("Failed to open output file for Huffman");
                fclose(input);
                exit(EXIT_FAILURE);
            }
            encodeFile(input, output, codes, data, freq, size);
            fclose(input);
            fclose(output);
            freeHuffmanTree(root);
            for (int j = 0; j < BYTE_SIZE; j++) {
                free(codes[j]);
            }
        } else if (i == 1) {
            // RLE Compression
            rle_compress_file(input_file, output_files[i]);
        } else if (i == 2) {
            // LZW Compression
            lzw_compress(input_file, output_files[i]);
        }
    }

    // Compare file sizes
    long original_size = ftell(fopen(input_file, "rb")); // Make sure this is correct.
    long min_size = LONG_MAX;
    int best_index = -1;

    for (int i = 0; i < 3; i++) {
        FILE* file = fopen(output_files[i], "rb");
        if (file != NULL) {
            fseek(file, 0, SEEK_END); // Move to the end of the file
            long compressed_size = ftell(file);
            fclose(file);

            if (compressed_size < min_size) {
                min_size = compressed_size;
                best_index = i;
            }
        }
    }

    // Clean up temporary files
    for (int i = 0; i < 3; i++) {
        remove(output_files[i]);
    }

    return methods[best_index];
}
// Main Function
// Main Function
int main() {
    int method, choice;
    char input_file[500], output_file[500];

    printf("Choose an operation:\n");
    printf("1. Compress\n");
    printf("2. Decompress\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character

    // Prompt for input and output file paths
    printf("Enter the input file path: ");
    fgets(input_file, sizeof(input_file), stdin);
    input_file[strcspn(input_file, "\n")] = 0;  // Remove the newline character

    printf("Enter the output file path: ");
    fgets(output_file, sizeof(output_file), stdin);
    output_file[strcspn(output_file, "\n")] = 0;  // Remove the newline character

    struct MinHeapNode* root = NULL;
    unsigned char data[256];
    int freq[256];
    int size = 0;
    char* codes[256];

    for (int i = 0; i < 256; i++) {
        codes[i] = (char*)malloc((MAX_TREE_HT + 1) * sizeof(char));
        codes[i][0] = '\0';
    }

    if (choice == 1) { // Compress
        printf("Choose a compression method:\n");
        printf("1. Huffman Coding\n");
        printf("2. RLE (Run-Length Encoding)\n");
        printf("3. LZW (Lempel-Ziv-Welch)\n");
        printf("4. Automatic (choose best method)\n");
        printf("Enter your method choice (1-4): ");
        scanf("%d", &method);
        getchar(); // Consume the newline character

        // Exclude LZW for .jpg and .png files
        if (strstr(input_file, ".jpg") || strstr(input_file, ".png")) {
            if (method == 3) {
                printf("LZW compression is not supported for JPEG or PNG files.\n");
                return EXIT_FAILURE;
            }
        }

        if (method == 1) {
            FILE* input = fopen(input_file, "rb");
            FILE* output = fopen(output_file, "wb");
            if (!input || !output) {
                fprintf(stderr, "Error opening file.\n");
                return EXIT_FAILURE;
            }

            calculateFrequency(input, data, freq, &size);
            root = buildHuffmanTree(data, freq, size, codes);
            encodeFile(input, output, codes, data, freq, size);
            fclose(input);
            fclose(output);

            printf("Compression completed. Output written to %s\n", output_file);
            freeHuffmanTree(root);
            for (int i = 0; i < 256; i++) {
                free(codes[i]);
            }
        } else if (method == 2) {
            rle_compress_file(input_file, output_file);
            printf("RLE compression completed. Output written to %s\n", output_file);
        } else if (method == 3) {
            lzw_compress(input_file, output_file);
            printf("LZW compression completed. Output written to %s\n", output_file);
        } else if (method == 4) {
            const char* best_method = select_best_method(input_file);
            printf("Automatically selected method: %s\n", best_method);
            // Re-run the selected method here
            if (strcmp(best_method, "Huffman") == 0) {
                FILE* input = fopen(input_file, "rb");
                FILE* output = fopen(output_file, "wb");
                if (!input || !output) {
                    fprintf(stderr, "Error opening file.\n");
                    return EXIT_FAILURE;
                }

                calculateFrequency(input, data, freq, &size);
                root = buildHuffmanTree(data, freq, size, codes);
                encodeFile(input, output, codes, data, freq, size);
                fclose(input);
                fclose(output);

                printf("Compression completed using Huffman. Output written to %s\n", output_file);
                freeHuffmanTree(root);
                for (int i = 0; i < 256; i++) {
                    free(codes[i]);
                }
            } else if (strcmp(best_method, "RLE") == 0) {
                rle_compress_file(input_file, output_file);
                printf("RLE compression completed. Output written to %s\n", output_file);
            } else if (strcmp(best_method, "LZW") == 0) {
                lzw_compress(input_file, output_file);
                printf("LZW compression completed. Output written to %s\n", output_file);
            }
        }
    } else if (choice == 2) { // Decompress
        printf("Choose a decompression method:\n");
        printf("1. Huffman Coding\n");
        printf("2. RLE (Run-Length Encoding)\n");
        printf("3. LZW (Lempel-Ziv-Welch)\n");
        printf("Enter your method choice (1-3): ");
        scanf("%d", &method);
        getchar(); // Consume the newline character

        if (method == 1) {
            FILE* input = fopen(input_file, "rb");
            FILE* output = fopen(output_file, "wb");
            if (!input || !output) {
                fprintf(stderr, "Error opening file.\n");
                return EXIT_FAILURE;
            }

            // Read the size of the frequency table
            fread(&size, sizeof(int), 1, input);

            // Read the character frequency table from the input file
            for (int i = 0; i < size; i++) {
                data[i] = fgetc(input); // Read character
                fread(&freq[i], sizeof(int), 1, input); // Read its frequency
            }

            root = buildHuffmanTree(data, freq, size, codes);
            decodeFile(input, output, root);
            fclose(input);
            fclose(output);

            printf("Decompression completed. Output written to %s\n", output_file);
            freeHuffmanTree(root);
            for (int i = 0; i < 256; i++) {
                free(codes[i]);
            }
        } else if (method == 2) {
            rle_decompress_file(input_file, output_file);
            printf("RLE decompression completed. Output written to %s\n", output_file);
        } else if (method == 3) {
            lzw_decompress(input_file, output_file);
            printf("LZW decompression completed. Output written to %s\n", output_file);
        }
    } else {
        fprintf(stderr, "Invalid choice.\n");
    }

    return 0;
}