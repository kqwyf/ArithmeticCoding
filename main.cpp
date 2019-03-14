#include <cstdio>
#include <cstring>

#include "utils.h"
#include "arithmetic.h"

using namespace std;

long fsize(FILE *f) {
    long cur = ftell(f);
    fseek(f, 0, SEEK_END);
    long result = ftell(f);
    fseek(f, cur, SEEK_SET);
    return result;
}

int main(int argc, char **argv) {
    bool adaptive = false;
    char source[128];
    char target[128];
    memset(source, 0, sizeof source);
    memset(target, 0, sizeof target);
    for(int i = 1; i < argc; i++) {
        if(strcmp("-a", argv[i]) == 0)
            adaptive = true;
        else if(argv[i][0] != '-') {
            if(!source[0])
                strncpy(source, argv[i], 127);
            else
                strncpy(target, argv[i], 127);
        }
    }

    // read the source file
    FILE *fs = fopen(source, "r");
    if(fs == NULL) {
        printf("Cannot open the source file.\n");
        return -1;
    }
    long size = fsize(fs);
    char *sourceBuf = new char[size];
    fread(sourceBuf, sizeof(char), size, fs);
    fclose(fs);
    printf("original length: %ld bytes\n", size);

    // compression
    FrequencyTable *table = new FrequencyTable(1 << 8, 1 << 16);
    if(adaptive) {
        for(int i = 0; i < 1 << 8; i++)
            table->add((uint)i);
    } else {
        for(long i = 0; i < size; i++)
            table->add((uint)sourceBuf[i]);
    }
    BitArray *code = new BitArray(2 * size * 8); // assuming size of compressed data < 2 * original size
    uint codeLen = encode(*code, (uint8*)sourceBuf, size, *table, adaptive);
    delete[] sourceBuf;
    delete table;
    printf("compressed length: %d bytes\n", (codeLen + 7) / 8);

    // decompression
    table = new FrequencyTable(1 << 8, 1 << 16);
    if(adaptive) {
        for(int i = 0; i < 1 << 8; i++)
            table->add(i);
    } else {
        for(long i = 0; i < size; i++)
            table->add((uint)sourceBuf[i]);
    }
    char *targetBuf = new char[size];
    uint targetSize = decode((uint8*)targetBuf, size, *code, *table, adaptive);
    delete code;
    delete table;
    if(targetSize == 0) {
        printf("Cannot decompress any data.\n");
    }

    // write decompression result into target file
    FILE *ft = fopen(target, "w");
    if(ft == NULL) {
        printf("Cannot open the target file.\n");
        return -1;
    }
    fwrite(targetBuf, sizeof(char), targetSize, ft);
    fclose(ft);
    delete[] targetBuf;
    return 0;
}
