#pragma once
#include "gamepool.h"
#include "random.h"
#include "tetris_ai.h"

namespace AI {

    struct point {
        float x;
        float y;
        point() : x(0), y(0) {}
        point(float _x, float _y) : x(_x), y(_y) {}
        point(int _x, int _y)
            : x(static_cast<float>(_x))
            , y(static_cast<float>(_y))
        {}
    };

    class Tetris {
    public:
        enum {
            STATE_INIT,
            STATE_READY,
            STATE_MOVING,
            STATE_PASTED,
            STATE_OVER,
        };
        struct clear_info {
            int gem_num;
            int clears;
            int attack;
            int b2b;
            int combo;
            int pc;
            signed char wallkick_spin;
            int total_pc;
            int total_b2b;
            int total_cb_att;
            int t[4];
            int normal[5];
            void reset( int _gem_num ) {
                memset(this, 0, sizeof(*this));
                gem_num = _gem_num;
            }
            void newgem( int _gem_num ) {
                gem_num = _gem_num;
                clears = 0;
                attack = 0;
                b2b = 0;
                combo = 0;
                pc = 0;
                wallkick_spin = 0;
            }
        };
        Tetris() : m_pool( 10, 20 ) {
            m_state = STATE_INIT;
            reset ( 0, 10, 20 );
        }
        void genNext() {
            int m[] = {1,2,3,4,5,6,7};
            int s[7];
            int v = m_rand.randint(5040);
            int mod = 5040 / 7;
            for ( int i = 6; i > 0; --i ) {
                s[6-i] = m[v / mod];
                for ( int j = v / mod; j < 6; ++j) {
                    m[j] = m[j+1];
                }
                v %= mod;
                mod /= i;
            }
            s[6] = m[0];
            for ( int i = 0; i < 7; ++i ) {
                m_next[ m_next_num + i] = AI::getGem( s[i], 0);
            }
            m_next_num += 7;
        }
        void reset (unsigned seed, signed char w, signed char h) {
            m_pool.reset( w, h );
            m_rand.seed( seed );
            m_next_num = 0;
            while ( m_next_num < 100 ) genNext();
            //for ( int i = 0; i < 32; ++i ) {
            //    m_next[i] = AI::getGem( m_rand.randint(7) + 1, 0);
            //}
            m_state = STATE_READY;
            m_cur = AI::getGem(0, 0);
            m_cur_x = AI::gem_beg_x;
            m_cur_y = AI::gem_beg_y;
            m_curnum = 0;
            m_clearLines = 0;
            m_attack = 0;
            m_max_combo = 0;
            m_frames = 0;
            m_drop_frame = 0;
            m_clear_info.reset( 0 );
            memset( m_color_pool, 0, sizeof( m_color_pool ) );
        }
        bool tryXMove(int dx) {
            if ( m_state != STATE_MOVING ) return false;
            if (m_pool.isCollide(m_cur_x + dx, m_cur_y, m_cur))
                return false;
            m_cur_x += dx;
            wallkick_spin = 0;
            return true;
        }
        bool tryYMove(int dy) {
            if ( m_state != STATE_MOVING ) return false;
            if (m_pool.isCollide(m_cur_x, m_cur_y + dy, m_cur))
                return false;
            m_cur_y += dy;
            wallkick_spin = 0;
            return true;
        }
        bool trySpin(int dSpin) {
            if ( m_state != STATE_MOVING ) return false;
            AI::Gem gem = AI::getGem(m_cur.num, (m_cur.spin + dSpin + 4) % 4);
            if (m_pool.isCollide(m_cur_x, m_cur_y, gem)) {
                int spin = 1;
                if ( dSpin == 1 ) spin = 0;
                if ( m_pool.wallkickTest(m_cur_x, m_cur_y, gem, spin) ) {
                    m_cur = gem;
                    wallkick_spin = 2;
                    return true;
                } else {
                    return false;
                }
            }
            m_cur = gem;
            wallkick_spin = 1;
            return true;
        }
        bool trySpin180() {
            if ( m_state != STATE_MOVING ) return false;
            AI::Gem gem = AI::getGem(m_cur.num, (m_cur.spin + 2) % 4);
            if (m_pool.isCollide(m_cur_x, m_cur_y, gem)) {
                return false;
            }
            m_cur = gem;
            wallkick_spin = 1;
            return true;
        }
        bool tryHold() {
            if ( m_state != STATE_MOVING ) return false;
            if ( m_hold ) return false;
            m_hold = true;
            int hold = m_pool.m_hold;
            m_pool.m_hold = m_cur.num;
            if ( hold == 0 ) {
                m_cur_x = AI::gem_beg_x;
                m_cur_y = AI::gem_beg_y;
                m_cur = AI::getGem(m_next[0].num, 0);
                removeNext();
            } else {
                m_cur_x = AI::gem_beg_x;
                m_cur_y = AI::gem_beg_y;
                m_cur = AI::getGem(hold, 0);
            }
            if ( m_pool.isCollide(m_cur_x, m_cur_y, m_cur)) {
                m_state = STATE_OVER;
                return true;
            }
            return true;
        }
        void paste() {
            for ( int y = 0; y < 4; ++y ) {
                for ( int x = 0; x < 4; ++x ) {
                    if ( m_cur.bitmap[y] & ( 1 << x ) ) {
                        m_color_pool[m_cur_y + y + 32][m_cur_x + x] = m_cur.num;
                    }
                }
            }
        }
        bool drop () {
            if ( m_state != STATE_MOVING ) return false;
            m_clear_info.newgem( m_cur.num );
            while ( tryYMove( 1 ) );
            wallkick_spin = m_pool.WallKickValue(m_cur.num, m_cur_x, m_cur_y, m_cur.spin, wallkick_spin);
            m_pool.paste( m_cur_x, m_cur_y, m_cur );
            paste();
            m_drop_frame = m_frames;
            m_cur = AI::getGem( 0, 0);
            m_state = STATE_PASTED;
            return true;
        }
        void color_pool_clearLines() {
            int dy = 63;
            for ( int y = dy; y >= 0; --y ) {
                int x = 0;
                for ( ; x < poolw(); ++x ) {
                    if ( m_color_pool[y][x] == 0 ) break;
                }
                if ( x < poolw() ) {
                    if ( dy != y ) {
                        for (x = 0 ; x < poolw(); ++x ) {
                            m_color_pool[dy][x] = m_color_pool[y][x];
                        }
                    }
                    --dy;
                }
            }
            for ( ; dy >= 0; --dy ) {
                for (int x = 0 ; x < poolw(); ++x ) {
                    m_color_pool[dy][x] = 0;
                }
            }
        }
        void clearLines () {
            if ( m_state != STATE_PASTED ) return;
            m_clearLines = m_pool.clearLines( wallkick_spin );
            color_pool_clearLines();
            m_attack = m_pool.getAttack( m_clearLines, wallkick_spin );
            m_max_combo = std::max(m_max_combo, m_pool.combo - 1);

            m_clear_info.clears = m_clearLines;
            m_clear_info.attack = m_attack;
            m_clear_info.b2b = m_pool.b2b;
            m_clear_info.combo = m_pool.combo;
            m_clear_info.wallkick_spin = wallkick_spin;
            m_clear_info.total_cb_att += getComboAttack( m_pool.combo );
            if ( m_clear_info.b2b > 1 && m_attack > 0 )
            {
                ++m_clear_info.total_b2b;
            }
            {
                int i = gem_add_y + m_pool.height();
                for ( ; i >= 0; --i ) {
                    if ( m_pool.m_row[i] ) break;
                }
                if ( i < 0 ) {
                    m_clear_info.pc = 1;
                    ++m_clear_info.total_pc;
                }
            }
            int special = 0;
            //if ( m_clear_info.gem_num == 1 && m_clearLines >= 4 )
            //{
            //    special = 1;
            //    m_clear_info.normal[4] += 1;
            //}
            if ( m_attack > 0 )
            {
                if ( m_clear_info.wallkick_spin ) {
                    special = 1;
                    //if ( m_clear_info.gem_num == 2 )
                    {
                        if ( wallkick_spin == 2 && (AI::isEnableAllSpin() || m_clear_info.clears == 1) ) {
                        //if ( m_clear_info.wallkick_spin == 2 ) {
                            ++m_clear_info.t[0];
                        } else {
                            ++m_clear_info.t[m_clear_info.clears];
                        }
                    }
                }
                //if ( m_clear_info.gem_num == 2 )
                //{
                //    int att = m_attack;
                //    att -= m_pool.b2b > 1;
                //    att -= getComboAttack( m_pool.combo );
                //    if ( m_clear_info.pc ) {
                //        att -= 6;
                //    }
                //    if ( att / m_clear_info.clears >= 2 ) { // T1T2T3
                //        special = 1;
                //        ++m_clear_info.t[m_clear_info.clears];
                //    } else if ( m_clear_info.clears == 2 ) { // double
                //        //++m_clear_info.normal[2];
                //    } else if ( m_clear_info.clears == 1 && att == 1 ) { // T0
                //        special = 1;
                //        ++m_clear_info.t[0];
                //    }
                //}
            }
            if ( m_clearLines > 0 && special == 0 )
            {
                ++m_clear_info.normal[m_clearLines];
            }

            m_state = STATE_READY;
        }
        void addRow( int att ) {
            {
                for ( int y = 1; y < 64; ++y ) {
                    for (int x = 0 ; x < poolw(); ++x ) {
                        m_color_pool[y-1][x] = m_color_pool[y][x];
                    }
                }
                for (int x = 0 ; x < poolw(); ++x ) {
                    if ( att & ( 1 << x ) ) {
                        m_color_pool[poolh() + 32][x] = 8;
                    } else {
                        m_color_pool[poolh() + 32][x] = 0;
                    }
                }
            }
            m_pool.addRow( att );
            if ( m_cur_y > 1 ) {
                m_cur_y -= 1;
            }
            if ( m_pool.m_row[0] ) {
                m_state = STATE_OVER;
            }
        }
        void setRow( int y, int att ) {
            {
                for (int x = 0 ; x < poolw(); ++x ) {
                    if ( att & ( 1 << x ) ) {
                        m_color_pool[y + 32][x] = 8;
                    } else {
                        m_color_pool[y + 32][x] = 0;
                    }
                }
            }
            m_pool.row[y] = att;
        }
        void removeNext() {
            for (int i = 1; i < m_next_num; ++i) {
                m_next[i - 1] = m_next[i];
            }
            --m_next_num;
            while ( m_next_num < 100 ) genNext();
            //m_next[15] = AI::getGem( m_rand.randint(7) + 1, 0);
        }
        bool newpiece() {
            if ( m_state != STATE_READY ) return false;
            m_cur_x = AI::gem_beg_x;
            m_cur_y = AI::gem_beg_y;
            ++m_curnum;
            m_cur = m_next[0];
            m_hold = false;
            wallkick_spin = 0;
            removeNext();
            //if ( m_pool.row[0] || m_pool.row[1] || m_pool.isCollide(m_cur_x, m_cur_y, m_cur)) {
            if ( m_pool.isCollide(m_cur_x, m_cur_y, m_cur) ) {
                m_state = STATE_OVER;
                return true;
            }
            m_state = STATE_MOVING;
            return true;
        }
        bool game() {
            ++m_frames;
            switch (m_state) {
            case STATE_MOVING:
                {
                }
                break;
            case STATE_PASTED:
                {
                    clearLines();
                }
            case STATE_READY:
                {
                    return newpiece();
                }
                break;
            default:
                break;
            }
            return false;
        }
        int poolw() const {
            return m_pool.width();
        }
        int poolh() const {
            return m_pool.height();
        }
        int curx() const {
            return m_cur_x;
        }
        int cury() const {
            return m_cur_y;
        }
        int getCurGemCell(int x, int y) const {
            if ( m_cur.bitmap[y] & ( 1 << x ) ) return 1;
            return 0;
        }
        int getNextGemCell(int next, int x, int y) const {
            if ( m_next[next].bitmap[y] & ( 1 << x ) ) return 1;
            return 0;
        }
        int getPoolCell(int x, int y) const {
            return m_color_pool[y+32][x];
            //if ( m_pool.row[y + 1] & ( 1 << x) ) return 1;
            return 0;
        }
        bool alive () const {
            return m_state != STATE_OVER;
        }
    public:
        int m_state;
    public:
        Random m_rand;
        AI::GameField m_pool;
        AI::Gem m_cur;
        int m_color_pool[64][32];
        int m_hold;
        int m_cur_x, m_cur_y;
        int m_curnum;
        signed char wallkick_spin;
        AI::Gem m_next[128];
        int m_next_num;
        point m_base, m_size;
        int m_clearLines;
        int m_attack;
        int m_max_combo;
        int m_frames;
        int m_drop_frame;
        clear_info m_clear_info;
    };

}
