#ifndef __UTILS_H__
#define __UTILS_H__

typedef unsigned int uint;
typedef unsigned char uchar, uint8;
typedef unsigned short ushort, uint16;
typedef uint uint32;

const uint BITS = sizeof(uint);

class BitArray {
    public:
        BitArray(uint size);
        ~BitArray();
        bool get(uint index) const;
        void set(uint index, bool value);
        void set(uint start, uint end, bool value);
        uint size() const;

    private:
        uint *data;
        uint length;
};

class FrequencyTable {
    public:
        FrequencyTable(uint size, uint16 limit);
        ~FrequencyTable();
        uint16 get(uint index);
        void set(uint index, uint16 value);
        void add(uint index, uint16 value = 1);
        uint16 getTotal();
        uint16 getL(uint index);
        uint16 getR(uint index);
        uint size();

    private:
        uint16 *data;
        uint length;
        uint16 limit;
        uint16 total;
};

#endif

