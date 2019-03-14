#include <cstdio>
#include <cstring>

#include "utils.h"
#include "arithmetic.h"

using namespace std;

/*
 * Compressed File Structure:
 *
 * original file size (in bytes) - 4 or 8 bytes (long)
 * number of entries in frequency table (if not adaptive) - 4 bytes
 * value of entries (if not adaptive) - 2 * n bytes
 * compressed data - ? bytes
 */

const uint TABLE_SIZE = 1 << 8;
const uint TABLE_LIMIT = 1 << 15;

long fsize(FILE *f) {
    long cur = ftell(f);
    fseek(f, 0, SEEK_END);
    long result = ftell(f);
    fseek(f, cur, SEEK_SET);
    return result;
}

int compress(FILE *source, FILE *target, bool adaptive) {
    // read the original data
    long sourceSize = fsize(source);
    char *sourceBuf = new char[sourceSize];
    fread(sourceBuf, sizeof(char), sourceSize, source);

    // initialize the frequency table
    FrequencyTable *table = new FrequencyTable(TABLE_SIZE, TABLE_LIMIT);
    if(adaptive) {
        for(uint i = 0; i < TABLE_SIZE; i++)
            table->set(i, 1);
    } else {
        table->add(0);
        for(long i = 0; i < sourceSize; i++)
            table->add((uint)sourceBuf[i]);
    }

    // compress
    BitArray *code = new BitArray(2 * sourceSize * 8); // assuming size of compressed data < 2 * original size
    uint codeLen = encode(*code, (uint8*)sourceBuf, sourceSize, *table, adaptive);
    long targetSize = (codeLen + 7) / 8;
    delete[] sourceBuf;

    // write to the target file
    fwrite(&sourceSize, sizeof(long), 1, target);
    if(!adaptive) {
        fwrite(&TABLE_SIZE, sizeof(uint), 1, target);
        for(uint i = 0; i < TABLE_SIZE; i++) {
            uint16 v = table->get(i);
            fwrite(&v, sizeof(uint16), 1, target);
        }
    }
    delete table;
    fwrite(code->toUintArray(), sizeof(uint), (targetSize + sizeof(uint) - 1) / sizeof(uint), target);
    delete code;

    if(adaptive)
        return targetSize;
    else
        return targetSize + sizeof(uint) + sizeof(uint16) * TABLE_SIZE;
}

int decompress(FILE *source, FILE *target, bool adaptive) {
    // read the meta info
    long sourceSize = fsize(source);
    long sourceDataSize = sourceSize;
    long originalSize;
    fread(&originalSize, sizeof(long), 1, source);
    FrequencyTable *table;
    if(adaptive) {
        table = new FrequencyTable(TABLE_SIZE, TABLE_LIMIT);
        for(uint i = 0; i < TABLE_SIZE; i++)
            table->set(i, 1);
    } else {
        uint tableCount;
        fread(&tableCount, sizeof(uint), 1, source);
        table = new FrequencyTable(tableCount, TABLE_LIMIT);
        for(uint i = 0; i < tableCount; i++) {
            uint16 v;
            fread(&v, sizeof(uint16), 1, source);
            table->set(i, v);
        }
        sourceDataSize -= sizeof(uint) + sizeof(uint16) * tableCount;
    }

    // read the compressed data
    sourceDataSize = (sourceDataSize + sizeof(uint) - 1) / sizeof(uint);
    uint *sourceBuf = new uint[sourceDataSize];
    fread(sourceBuf, sizeof(uint), sourceDataSize, source);
    BitArray *code = new BitArray(sourceBuf, sourceDataSize);
    delete[] sourceBuf;

    // decompress
    char *targetBuf = new char[originalSize];
    uint targetSize = decode((uint8*)targetBuf, originalSize, *code, *table, adaptive);
    delete code;
    delete table;
    if(targetSize != originalSize) { // assertion failed
        delete[] targetBuf;
        return 0;
    }

    // write to the target file
    fwrite(targetBuf, sizeof(char), targetSize, target);
    delete[] targetBuf;

    return targetSize;
}

int main(int argc, char **argv) {
    bool adaptive = false;
    bool decompression = false;
    char *source = NULL;
    char *target = NULL;
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            if(strcmp("-a", argv[i]) == 0)
                adaptive = true;
            else if(strcmp("-d", argv[i]) == 0)
                decompression = true;
        } else if(argv[i][0] != '-') {
            if(!source)
                source = argv[i];
            else
                target = argv[i];
        }
    }

    FILE *fs = fopen(source, "rb");
    if(fs == NULL) {
        printf("Cannot open the source file.\n");
        return -1;
    }
    FILE *ft = fopen(target, "wb");
    if(ft == NULL) {
        printf("Cannot open the target file.\n");
        fclose(fs);
        return -1;
    }

    long size;
    if(decompression) {
        size = decompress(fs, ft, adaptive);
        if(size <= 0)
            printf("Decompression failed.\n");
        else
            printf("Decompressed size: %ld bytes.\n", size);
    } else {
        size = compress(fs, ft, adaptive);
        if(size <= 0)
            printf("Compression failed.\n");
        else
            printf("Compressed size: %ld bytes.\n", size);
    }

    fclose(fs);
    fclose(ft);
    return 0;
}
