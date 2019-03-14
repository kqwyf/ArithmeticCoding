#ifndef __ARITHMETIC_H__
#define __ARITHMETIC_H__

#include "utils.h"

uint encode(BitArray &target, uint8 *source, uint n, FrequencyTable &f, bool adaptive);
uint decode(uint8 *target, uint n, const BitArray &source, FrequencyTable &f, bool adaptive);

#endif

