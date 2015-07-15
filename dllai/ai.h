#pragma once

#include "gamefield.h"
#include <vector>

#define AI_MAX_DEPTH    2

namespace AI {
    struct Moving
    {
        enum {
            MOV_NULL,
            MOV_L,
            MOV_R,
            MOV_LL,
            MOV_RR,
            MOV_D,
            MOV_DD,
            MOV_LSPIN,
            MOV_RSPIN,
            MOV_DROP,
            MOV_HOLD,
            MOV_SPIN2,
        };
        std::vector<int> movs;
        int x, y;
        int score;
        signed char spin;
        signed char wallkick_spin;
        Moving () { wallkick_spin = 0; movs.reserve (16); }
        Moving ( const Moving & m ) {
            movs = m.movs;
            x = m.x;
            y = m.y;
            spin = m.spin;
            score = m.score;
            wallkick_spin = m.wallkick_spin;
        }
        Moving ( const Moving & m, int _spin ) {
            movs.reserve (16);
            movs = m.movs;
            spin = (signed char)_spin;
        }
        bool operator < (const Moving& _m) const {
            return score > _m.score;
        }
        bool operator == (const Moving& _m) const {
            if ( x != _m.x || y != _m.y ) return false;
            if ( spin != _m.spin ) return false;
            if ( wallkick_spin != _m.wallkick_spin ) return false;
            return true;
        }
    };
    struct MovingSimple
    {
        enum {
            MOV_NULL,
            MOV_L,
            MOV_R,
            MOV_LL,
            MOV_RR,
            MOV_D,
            MOV_DD,
            MOV_LSPIN,
            MOV_RSPIN,
            MOV_DROP,
            MOV_HOLD,
            MOV_SPIN2,
        };
        enum {
            INVALID_POS = -64,
        };
        int x, y;
        int lastmove;
        signed char spin;
        signed char wallkick_spin;
        bool hold;
        MovingSimple () { x = INVALID_POS; wallkick_spin = 0; lastmove = MovingSimple::MOV_NULL; }
        MovingSimple ( const Moving & m ) {
            x = m.x;
            y = m.y;
            spin = m.spin;
            wallkick_spin = m.wallkick_spin;
            if (m.movs.empty()) hold = false;
            else hold = (m.movs[0] == MovingSimple::MOV_HOLD);
            if (m.movs.empty()) lastmove = MovingSimple::MOV_NULL;
            else lastmove = m.movs.back();
        }
        MovingSimple ( const MovingSimple & m ) {
            x = m.x;
            y = m.y;
            spin = m.spin;
            wallkick_spin = m.wallkick_spin;
            hold = m.hold;
            lastmove = m.lastmove;
        }
        MovingSimple ( const MovingSimple & m, int _spin ) {
            lastmove = m.lastmove;
            hold = m.hold;
            spin = (signed char)_spin;
        }
        bool operator == (const MovingSimple& _m) const {
            if ( x != _m.x || y != _m.y ) return false;
            if ( spin != _m.spin ) return false;
            if ( lastmove != _m.lastmove ) return false;
            if ( hold != _m.hold ) return false;
            if ( wallkick_spin != _m.wallkick_spin ) return false;
            return true;
        }
    };
    template <class T>
    struct MovList
    {
        std::vector<T> queue;
        //T queue[1024];
        int beg, end;
        MovList() {
            beg = end = 0;
        }
        MovList( size_t size ) {
            beg = end = 0;
            //queue.resize( size );
            queue.reserve( size );
        }
        void clear() {
            beg = end = 0;
            queue.clear();
        }
        size_t size() const {
            return end - beg;
        }
        T& back() {
            return queue[end-1];
        }
        void push(const T& mov) {
            queue.push_back(mov);
            //queue[end] = mov;
            ++end;
        }
        bool empty () const {
            return beg == end;
        }
        void pop(T& mov) {
            mov = queue[beg++];
        }
    };
    enum AIScore {
        SCORE_LOSE = -100000000,
    };
    void setComboList( std::vector<int> combolist );
    int getComboAttack( int combo );
    void GenMoving(const GameField& field, std::vector<MovingSimple> & movs, Gem cur, int x, int y, bool hold);
    void FindPathMoving(const GameField& field, std::vector<Moving> & movs, Gem cur, int x, int y, bool hold);
    MovingSimple AISearch(const GameField& field, Gem cur, bool curCanHold, int x, int y, const std::vector<Gem>& next
        , bool canHold, int upcomeAtt, int level, int player
        , int maxdepth = AI_MAX_DEPTH, int* score = NULL, int lastatt = 0, int lastclear = 0, int depth = 0);
    void setSpin180( bool enable );
}
