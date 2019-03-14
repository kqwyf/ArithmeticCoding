#ifndef __UTILS_H__
#define __UTILS_H__

typedef unsigned int uint;

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
        FrequencyTable(uint size, uint limit);
        ~FrequencyTable();
        uint get(uint index);
        void set(uint index, uint value);
        void add(uint index, uint value = 1);
        uint getTotal();
        uint getL(uint index);
        uint getR(uint index);
        uint size();

    private:
        uint *data;
        uint length;
        uint limit;
        uint total;
};

#endif

