/*FILECOMPARE
 * Generates SHA256 hashes for supplied files, then compares all generated hashes
 to check if the files are identical. The maximum number of arguments is set by
 the MAX_OPERANDS variable.
Author: Cody Register
Date: 20211212*/


#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/sha.h>

#define BUFFER_SIZE 256
#define MAX_OPERANDS 10//Maximum arguments
/*I'm sure that a dynamic way of handling this exists, but I didn't feel like
 trying to figure it out. I'm very bad at C. */

int getHash(char * filename, int i);
int compareHash(int i, int j);
unsigned char hashes[MAX_OPERANDS][SHA256_DIGEST_LENGTH];

int main(int argc, char** argv) {
    int debug = 0;

    if (argc < 3){
        fprintf(stderr, "Error, this program requires at least two arguments\n");
        return EXIT_FAILURE;
    }
    else if (argc > MAX_OPERANDS){
        fprintf(stderr, "Error, you've entered too many arguments. The current maximum is set to %i.\n",MAX_OPERANDS);
        return EXIT_FAILURE;
    }
    for (int i = 1; i < argc; i++) {
        if (getHash(argv[i], i - 1) < 0) {
            fprintf(stderr, "Error generating hash for %s: %s\n", argv[i], strerror(errno));
            return EXIT_FAILURE;
        }
    }
    if (debug > 0) {
        for (int j = 0; j < 2; j++) {
            printf("Hash for %s: ", argv[j + 1]);
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                printf("%02x", hashes[j][i]);
            }
            printf("\n");
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        for (int j = i + 1; j < argc; j++) {
            if (compareHash(i, j) != 0) {
                printf("Files %s and %s differ\n", argv[i], argv[j]);
            } else {
                printf("Files %s and %s are identical\n", argv[i], argv[j]);
            }
        }
    }
}

int compareHash(int i, int j) {
    return memcmp(hashes[i - 1], hashes[j - 1], SHA256_DIGEST_LENGTH);
}

int getHash(char * filename, int i) {
    char buffer[BUFFER_SIZE];
    int fd;
    SHA256_CTX ctx;
    int num_bytes;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Error opening file %s: %s\n", filename, strerror(errno));
        return -1;
    }
    SHA256_Init(&ctx);
    while ((num_bytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
        SHA256_Update(&ctx, buffer, BUFFER_SIZE);
    }
    SHA256_Final(hashes[i], &ctx);
    close(fd);
    return 0;
}
