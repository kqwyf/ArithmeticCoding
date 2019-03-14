#include "utils.h"


BitArray::BitArray(uint size):length(size) {
    data = new uint[(size + BITS - 1) / BITS];
}

BitArray::~BitArray() {
    delete data;
}

bool BitArray::get(uint index) const {
    return (data[index / BITS] >> (index % BITS)) & 1;
}

void BitArray::set(uint index, bool value) {
    uint v = value ? 1 : 0;
    v <<= index % BITS;
    data[index / BITS] &= ~(1 << (index % BITS));
    data[index / BITS] |= v;
}

void BitArray::set(uint start, uint end, bool value) {
    // TODO: optimize the algorithm
    for(uint i = start; i <= end; i++)
        this->set(i, value);
}

uint BitArray::size() const {
    return length;
}


FrequencyTable::FrequencyTable(uint size, uint limit):length(size),limit(limit) {
    data = new uint[size];
    for(uint i = 0; i < size; i++)
        data[i] = 0;
    total = 0;
}

FrequencyTable::~FrequencyTable() {
    delete data;
}

uint FrequencyTable::get(uint index) {
    return data[index];
}

// TODO: divide all frequencies by 2 when any frequency is too large
void FrequencyTable::set(uint index, uint value) {
    total -= data[index];
    data[index] = value;
    total += data[index];
}

// TODO: ditto
void FrequencyTable::add(uint index, uint value = 1) {
    data[index] += value;
    total += value;
}

uint FrequencyTable::getTotal() {
    return total;
}

uint FrequencyTable::getL(uint index) {
    // TODO: optimize the algorithm
    uint result = 0;
    for(uint i = 0; i < index; i++)
        result += data[i];
    return result;
}

uint FrequencyTable::getR(uint index) {
    return getL(index) + get(index);
}

uint FrequencyTable::size() {
    return length;
}

