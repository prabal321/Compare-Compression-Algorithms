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
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
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

int main() {
    char choice;
    char inputFilePath[500], outputFilePath[500];

    printf("Choose operation:\n");
    printf("1. Compress (Huffman Encoding)\n");
    printf("2. Decompress (Huffman Decoding)\n");
    printf("Enter your choice: ");
    scanf(" %c", &choice);

    printf("Enter input file path:\n");
    scanf("%s", inputFilePath);

    printf("Enter output file path:\n");
    scanf("%s", outputFilePath);

    struct MinHeapNode* root = NULL;
    unsigned char data[256];
    int freq[256];
    int size = 0;
    char* codes[256];

    for (int i = 0; i < 256; i++) {
        codes[i] = (char*)malloc((MAX_TREE_HT + 1) * sizeof(char));
        codes[i][0] = '\0';
    }

    if (choice == '1') {
        FILE* input = fopen(inputFilePath, "rb");
        FILE* output = fopen(outputFilePath, "wb");
        if (!input || !output) {
            fprintf(stderr, "Error opening file.\n");
            return EXIT_FAILURE;
        }

        calculateFrequency(input, data, freq, &size);
        root = buildHuffmanTree(data, freq, size, codes);
        encodeFile(input, output, codes, data, freq, size);
        fclose(input);
        fclose(output);

        printf("Compression completed. Output written to %s\n", outputFilePath);
    } else if (choice == '2') {
        FILE* input = fopen(inputFilePath, "rb");
        FILE* output = fopen(outputFilePath, "wb");
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

        printf("Decompression completed. Output written to %s\n", outputFilePath);
    }

    freeHuffmanTree(root);
    for (int i = 0; i < 256; i++) {
        free(codes[i]);
    }

    return 0;
}