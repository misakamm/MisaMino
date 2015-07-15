#include "ai.h"

namespace AI {
    int Evaluate(const GameField& field, int att, int clear, int depth, int player) {
        int score = 0;
        int field_w = field.width(), field_h = field.height();
        int min_y[32] = {0};
        struct factor {
            int hole, h_change, y_factor, h_variance, att, noattclear;
        } ai_factor_l[3] = {
                {-50, -5, -10, -10,  40, -30},
                {-50, -5, -10, -10,  30, -35},
                {-50, -5, -10, -10,  20, -35},
        };
        factor ai_factor;
        if ( depth > 2 ) ai_factor = ai_factor_l[2];
        else ai_factor = ai_factor_l[depth];

        // get all columns height
        {
            int beg_y = 0;
            while ( field.row[beg_y] == 0 ) ++beg_y;
            for ( int x = 0; x < field_w; ++x) {
                for ( int y = beg_y, ey = field_h + 1; y <= ey; ++y) {
                    if ( field.row[y] & ( 1 << x ) ) {
                        min_y[x] = y;
                        break;
                    }
                }
            }
        }
        min_y[field_w] = min_y[field_w-2];
        // find holes
        {
            int hole_score = 0;
            for ( int x = 0; x < field_w; ++x) {
                for ( int y = min_y[x] + 1; y <= field_h; ++y) {
                    if ( ( field.row[y] & ( 1 << x ) ) == 0) {
                        hole_score += ai_factor.hole;
                    }
                }
            }
            score += hole_score;
        }
        // height change
        {
            int last = min_y[1];
            for ( int x = 0; x <= field_w; last = min_y[x], ++x) {
                int v = min_y[x] - last;
                int absv = abs(v);
                score += absv * ai_factor.h_change;
            }
        }
        // variance
        {
            int h_variance_score = 0;
            int avg = 0;
            {
                int sum = 0;
                int sample_cnt = 0;
                for ( int x = 0; x < field_w; ++x) {
                    avg += min_y[x];
                }
                {
                    double h = field_h - (double)avg / field_w;
                    score += int(ai_factor.y_factor * h * h / field_h);
                }
                for ( int x = 0; x < field_w; ++x) {
                    int t = avg - min_y[x] * field_w;
                    if ( abs(t) > field_h * field_w / 4 ) {
                        if ( t > 0 ) t = field_h * field_w / 4;
                        else t = -(field_h * field_w / 4);
                    }
                    sum += t * t;
                    ++sample_cnt;
                }
                if ( sample_cnt > 0 ) {
                    h_variance_score = sum * ai_factor.h_variance / (sample_cnt * 100);
                }
                score += h_variance_score;
            }
        }
        // clear and attack
        if ( clear > 0 )
        {
            if ( depth > 0 ) {
                score += int( att * ai_factor.att * ((double)att / clear - 0.7) );
            } else {
                score += att * ai_factor.att;
            }
            if ( att == 0 ) {
                score += clear * ai_factor.noattclear;
            }
        }
        return score;
    }
    int pasteClearAttack(GameField& field, int gemnum, int x, int y, int spin, signed char wallkick_spin, int &att) {
        wallkick_spin = field.WallKickValue(gemnum, x, y, spin, wallkick_spin);
        field.paste(x, y, getGem(gemnum, spin));
        int clear = field.clearLines( wallkick_spin );
        att = field.getAttack( clear, wallkick_spin );
        return clear;
    }
    MovingSimple AISearch(const GameField& field, Gem cur, bool curCanHold, int x, int y, const std::vector<Gem>& next
        , bool canhold, int upcomeAtt, int level, int player
        , int maxdepth, int* pscore, int lastatt, int lastclear, int depth) {

        std::vector<MovingSimple> movs;
        if ( pscore ) *pscore = SCORE_LOSE;
        GenMoving(field, movs, cur, x, y, false);
        if ( movs.empty() ) {
            return MovingSimple();
        }
        if ( maxdepth > level ) maxdepth = level;

        MovingSimple best;
        int maxScore = SCORE_LOSE;
        for (size_t i = 0; i < movs.size() ; ++i) {
            GameField newfield = field;
            int clear, att;
            clear = pasteClearAttack(newfield, cur.num, movs[i].x, movs[i].y, movs[i].spin, movs[i].wallkick_spin, att);
            int score = 0;
            if ( maxdepth > 0 && next.size() > 0) {
                std::vector<Gem> newnext( next.begin() + 1, next.end() );
                AISearch(newfield, next[0], canhold, gem_beg_x, gem_beg_y, newnext, canhold, upcomeAtt, level, player, maxdepth - 1, &score, lastatt + att, lastclear + clear, depth + 1);
            } else {
                score += Evaluate(newfield, lastatt + att, lastclear + clear, depth, player);
            }
            if ( score > maxScore ) {
                maxScore = score;
                best = movs[i];
            }
        }
        int hold_num = field.m_hold;
        if ( field.m_hold == 0 && ! next.empty()) {
            hold_num = next[0].num;
        }
        if ( canhold && curCanHold && hold_num ) {
            GenMoving(field, movs, AI::getGem(hold_num, 0), gem_beg_x, gem_beg_y, true);
            if ( ! movs.empty() ) {
                for (size_t i = 0; i < movs.size() ; ++i) {
                    GameField newfield = field;
                    int clear, att;
                    clear = pasteClearAttack(newfield, hold_num, movs[i].x, movs[i].y, movs[i].spin, movs[i].wallkick_spin, att);
                    int score = 0;
                    newfield.m_hold = cur.num;
                    if ( maxdepth > 0 && (field.m_hold != 0 && next.size() > 0 || next.size() > 1) ) {
                        if ( field.m_hold == 0 ) {
                            std::vector<Gem> newnext( next.begin() + 2, next.end() );
                            AISearch(newfield, next[1], canhold, gem_beg_x, gem_beg_y, newnext, canhold, upcomeAtt, level, player, maxdepth - 1, &score, lastatt + att, lastclear + clear, depth + 1);
                        } else {
                            std::vector<Gem> newnext( next.begin() + 1, next.end() );
                            AISearch(newfield, next[0], canhold, gem_beg_x, gem_beg_y, newnext, canhold, upcomeAtt, level, player, maxdepth - 1, &score, lastatt + att, lastclear + clear, depth + 1);
                        }
                    } else {
                        score += Evaluate(newfield, lastatt + att, lastclear + clear, depth, player);
                    }
                    if ( score > maxScore ) {
                        maxScore = score;
                        best = movs[i];
                    }
                }
            }
        }
        if ( pscore ) *pscore = maxScore;
        return best;
    }
}
