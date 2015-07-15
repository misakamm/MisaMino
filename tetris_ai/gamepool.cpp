#include "gamepool.h"
#include "random.h"

namespace AI {
    uint64 hashmap[AI_POOL_MAX_H][32];
    uint64 qhashmap[AI_POOL_MAX_H][2][32];

    struct initobj {
        initobj() {
            void InitHashTable();
            InitHashTable();
        }
    } _init_obj;
    void InitHashTable() {
        Random rnd;
        for ( int y = 0; y < AI_POOL_MAX_H; ++y) {
            for ( int x = 0; x < 32; ++x) {
                hashmap[y][x] = ((AI::uint64)rnd.rand() << 32) | rnd.rand();
            }
            for ( int x = 0; x < 2; ++x) {
                for ( int i = 0; i < 32; ++i) {
                    uint64 val = 0;
                    for ( int b = 0; b < 5; ++b ) {
                        if ( i & ( 1 << b ) ) val ^= hashmap[y][x*5+b];
                    }
                    qhashmap[y][x][i] = val;
                }
            }
        }
    }

    uint64 hash(const GameField & pool) {
        uint64 ret = 0;
        for ( int y = AI_POOL_MAX_H - 1; y >= 0; --y ) {
            if ( pool.m_row[y] == 0 ) break;
            ret ^= qhashmap[y][0][pool.m_row[y] & 31];
            ret ^= qhashmap[y][1][(pool.m_row[y] >> 5) & 31];
        }
        return ret;
    }

}