#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "utils.h"

typedef unsigned char uchar, uint8;
typedef unsigned short ushort, uint16;
typedef uint uint32;

uint encode(const BitArray &target, uint8 *source, uint n, const FrequencyTable &f, bool adaptive);
uint decode(uint8 *target, uint n, const BitArray &source, const FrequencyTable &f, bool adaptive);

#endif
