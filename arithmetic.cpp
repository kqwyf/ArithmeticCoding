#include "utils.h"
#include "arithmetic.h"

#define MSB(x) ((x>>31)&1)
#define MS2B(x) ((x>>30)&3)
#define MSB_MASK 0x80000000

typedef unsigned long long uint64;

uint encode(BitArray &target, uint8 *source, uint n, FrequencyTable &f, bool adaptive) {
    uint index = 0;
    uint l = (uint)0, r = ~(uint)0;
    uint cnt = 0;
    for(uint i = 0; i < n; i++) {
        uint8 c = source[i];
        if(adaptive) f.add(c);
        uint newl = (uint)(l + (uint64)(r - l) * f.getL(c) / f.getTotal());
        uint newr = (uint)(l + (uint64)(r - l) * f.getR(c) / f.getTotal());
        l = newl;
        r = newr;
        while(true) {
            if(MSB(l) == MSB(r)) {
                target.set(index - cnt, MSB(l));
                if(cnt > 0)
                    target.set(index - cnt + 1, index, !MSB(l));
                index++;
                l = (l << 1) & ~(uint)1;
                r = (r << 1) | (uint)1;
                continue;
            } else if(MS2B(l) == 0x1 && MS2B(r) == 0x2) {
                cnt++;
                index++;
                l = (l & MSB_MASK) | (((l << 1) & ~(uint)1) & ~MSB_MASK);
                r = (r & MSB_MASK) | (((r << 1) | (uint)1) & ~MSB_MASK);
                continue;
            }
            break;
        }
    }
    if(cnt > 0) {
        index -= cnt;
        target.set(index, 1);
    }
    return index;
}

uint decode(uint8 *target, uint n, const BitArray &source, FrequencyTable &f, bool adaptive) {
}

