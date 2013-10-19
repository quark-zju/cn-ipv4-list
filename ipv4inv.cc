#include <cstdio>
#include <cinttypes>
#include <stdexcept>
#include <set>

union IPv4Addr {
    struct {
        uint32_t x;
        uint8_t prefix;
    };

    struct {
        // FIXME Little Endian only
        uint8_t d;
        uint8_t c;
        uint8_t b;
        uint8_t a;

        uint8_t p;
    };

    bool read() {
        prefix = 32;
        return scanf("%hhu.%hhu.%hhu.%hhu/%hhu", &a, &b, &c, &d, &prefix) >= 4 && valid();
    }

    bool read(const char * s) {
        prefix = 32;
        return sscanf(s, "%hhu.%hhu.%hhu.%hhu/%hhu", &a, &b, &c, &d, &prefix) >= 4 && valid();
    }

    bool valid() const {
        return ((x >> (32 - prefix)) << (32 - prefix)) == x;
    }

    const char * c_str() const {
        // FIXME not thread safe
        static char buf[128];
        snprintf(buf, sizeof(buf), "%hhu.%hhu.%hhu.%hhu/%hhu", a, b, c, d, prefix);
        return buf;
    }

    IPv4Addr() : x(0), prefix(0) { };
    IPv4Addr(uint32_t x, uint8_t prefix = 32) : x(x), prefix(prefix) { };
    IPv4Addr(const char * s) { read(s); }

    uint32_t last() const {
        return (uint32_t)((uint64_t)x + (((uint64_t)1) << (32 - prefix)) - 1);
    }

    uint32_t first() const {
        return x;
    }

    int overlap_with(const IPv4Addr& addr) const {
        return first() <= addr.last() && last() >= addr.first();
    }

    int contain(const IPv4Addr& addr) const {
        return first() <= addr.first() && addr.last() <= last();
    }

    bool operator < (const IPv4Addr& rhs) const {
        return (x < rhs.x) || (x == rhs.x && prefix < rhs.prefix);
    }
};

struct IPv4AddrSet : public std::set<IPv4Addr> {
    bool contain(IPv4Addr addr) const {
        auto it = lower_bound(addr);
        return it->contain(addr) || (it != begin() && (--it)->contain(addr));
    }

    bool overlap(IPv4Addr addr) const {
        auto it_begin = lower_bound(addr.first());
        auto it_end   = upper_bound(addr.last());

        if (it_begin != begin()) --it_begin;
        if (it_end   != end())   ++it_end;

        for (auto it = it_begin; it != it_end; ++it) if (it->overlap_with(addr)) return true;
        return false;
    }

    void insert(IPv4Addr addr) {
        if (!addr.valid()) throw std::runtime_error("Invalid IPv4Addr");
        if (overlap(addr)) throw std::runtime_error("IPv4AddrSet does not allow overlapping IPv4Addr");
        std::set<IPv4Addr>::insert(addr);
    }

    void print() const {
        for (const auto& i : *this) printf("%s\n", i.c_str());
    }

    IPv4AddrSet invert() const {
        IPv4AddrSet ret;

        uint8_t prefix = 0;
        uint64_t     x = 0;

        for(;;) {
            // choose a prefix that does not overlap
            ++prefix;

            IPv4Addr addr(x, prefix);
            if (!addr.valid()) continue;

            if (!overlap(addr)) {
                // that is ok, add to return set
                ret.insert(addr);
            } else if (!count(addr)) {
                // overlapped but not encounter exact entry, try next prefix
                continue;
            }

            // next addr
            x = ((x >> (32 - prefix)) + 1) << (32 - prefix);
            if (x >> 32) break;
            prefix = 0;
        }
        return ret;
    }
};

IPv4AddrSet used;

int main(int argc, char const *argv[]) {

    // Add non-Internet addresses
    used.insert(IPv4Addr("10.0.0.0/8"));
    used.insert(IPv4Addr("192.168.0.0/16"));
    used.insert(IPv4Addr("172.16.0.0/12"));
    used.insert(IPv4Addr("127.0.0.0/8"));

    // used.contain(IPv4Addr("10.3.0.0/16")) // true
    // used.contain(IPv4Addr("192.0.0.0/8")) // false
    // used.overlap(IPv4Addr("10.3.0.0/16")) // true
    // used.overlap(IPv4Addr("192.0.0.0/8")) // true
    // used.invert().print() will output:
    // 0.0.0.0/5
    // 8.0.0.0/7
    // 11.0.0.0/8
    // 12.0.0.0/6
    // 16.0.0.0/4
    // 32.0.0.0/3
    // 64.0.0.0/3
    // 96.0.0.0/4
    // 112.0.0.0/5
    // 120.0.0.0/6
    // 124.0.0.0/7
    // 126.0.0.0/8
    // 128.0.0.0/3
    // 160.0.0.0/5
    // 168.0.0.0/6
    // 172.0.0.0/12
    // 172.32.0.0/11
    // 172.64.0.0/10
    // 172.128.0.0/9
    // 173.0.0.0/8
    // 174.0.0.0/7
    // 176.0.0.0/4
    // 192.0.0.0/9
    // 192.128.0.0/11
    // 192.160.0.0/13
    // 192.169.0.0/16
    // 192.170.0.0/15
    // 192.172.0.0/14
    // 192.176.0.0/12
    // 192.192.0.0/10
    // 193.0.0.0/8
    // 194.0.0.0/7
    // 196.0.0.0/6
    // 200.0.0.0/5
    // 208.0.0.0/4
    // 224.0.0.0/3
    
    for (IPv4Addr addr; addr.read();) used.insert(addr);

    used.invert().print();

    return 0;
}

