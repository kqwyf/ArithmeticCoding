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
    for(uint i = 0; i <= n; i++) {
        uint8 c = i == n ? 0 : source[i]; // append a terminate symbol to the signal
        if(adaptive) f.add(c);
        uint newl = (uint)(l + (uint64)(r - l) * f.getL(c) / f.getTotal());
        uint newr = (uint)(l + (uint64)(r - l) * f.getR(c) / f.getTotal());
        l = newl;
        r = r == newr ? newr : newr - 1; // avoid intervals overlapping
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
        if(c == 0) break; // 0 is regarded as the terminate symbol
    }
    if(cnt > 0) {
        index -= cnt;
        target.set(index, 1);
    }
    return index;
}

uint decode(uint8 *target, uint n, const BitArray &source, FrequencyTable &f, bool adaptive) {
    uint index = 0, i = 0;
    uint l = (uint)0, r = ~(uint)0;
    uint v = 0;
    for(i = 0; i < sizeof(uint); i++) {
        uint t = i < source.size() ? source.get(i) : 0;
        v = (v << 1) | t;
    }
    for(index = 0; index < n; index++) {
        if((uint)(l + (uint64)(r - l) * f.getR(0) / f.getTotal()) > v)
            break; // found the terminate symbol
        uint8 c;
        // TODO: optimize the algorithm
        for(c = 1; c != 0; c++) {
            uint tmpr = (uint)(l + (uint64)(r - l) * f.getR(c) / f.getTotal());
            if(tmpr != r) tmpr--; // avoid intervals overlapping
            if(tmpr >= v) break;
        }
        if(c == 0) return 0; // ERROR. It is not expected to return from here.
        if(adaptive) f.add(c);
        target[index] = c;
        uint newl = (uint)(l + (uint64)(r - l) * f.getL(c) / f.getTotal());
        uint newr = (uint)(l + (uint64)(r - l) * f.getR(c) / f.getTotal());
        l = newl;
        r = r == newr ? newr : newr - 1; // avoid intervals overlapping
        while(true) {
            if(MSB(l) == MSB(r)) {
                l = (l << 1) & ~(uint)1;
                r = (r << 1) | (uint)1;
                uint t = i < source.size() ? source.get(i++) : 0;
                v = (v << 1) | t;
                continue;
            } else if(MS2B(l) == 0x1 && MS2B(r) == 0x2) {
                l = (l & MSB_MASK) | (((l << 1) & ~(uint)1) & ~MSB_MASK);
                r = (r & MSB_MASK) | (((r << 1) | (uint)1) & ~MSB_MASK);
                uint t = i < source.size() ? source.get(i++) : 0;
                v = (v & MSB_MASK) | (((v << 1) | t) & ~MSB_MASK);
                continue;
            }
            break;
        }
        if(c == 0) break; // 0 is regarded as the terminate symbol
    }
    return index;
}

