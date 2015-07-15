#pragma once


#ifndef XP_RELEASE

std::vector<int> genNext(std::vector<int> val, AI::Random& rnd) {
    for ( size_t i = 0; i < val.size(); ++i) {
    //int i = rnd.randint(val.size()); {
        int a = max(2, abs(val[i]) / 2);
        a = rnd.randint(a + 1);
        if ( rnd.randint(2) ) {
            val[i] += a;
        } else {
            val[i] -= a;
        }
    }
    return val;
}

std::vector<int> genNext2(std::vector<int> val1, std::vector<int> val2, int out, AI::Random& rnd) {
    for ( size_t i = 0; i < val1.size(); ++i) {
        double center =  (val2[i] + val1[i]) / 2.0;
        double diff = abs(val2[i] - val1[i]) / 2.0;
        if ( diff < 4 ) diff = rnd.randint(5);
        double a = 0, b = 0;
        if ( out ) a = (rnd.randfloat() * diff * 4);
        else a = (rnd.randfloat() * diff * 2);
        if ( out ) b = fabs(rnd.randfloat() * val1[i] / 10);
        if ( rnd.randint(2) ) a = -a, b = -b;
        val1[i] = (int)floor( center + a + b + 0.5 );
    }
    return val1;
}

class EvE {
    struct ai_info {
        AI::AI_Param ai;
        int score;
        ai_info () {
            memset(&ai, 0, sizeof(ai));
            score = -1;
        }
        ai_info( const AI::AI_Param& _ai ) : ai(_ai) {
            score = -1;
        }
        bool operator < (const ai_info& info) const {
            return score < info.score;
        }
    };
public:
    EvE() {
        round = TRAINING_ROUND;
        nround = 0;
        state_find_best = false;
        find_best = false;
        //find_best = true;
        m_init = false;
    }
    void init(std::deque<ai_info> _ai) {
        ai = _ai;
        state_find_best = false;
        find_best = false;
        //find_best = true;
        m_init = false;
    }
    ai_info gen(ai_info param, AI::Random& rnd) {
        std::vector<int> v;
        int n = sizeof(param) / sizeof(int);
        for ( int i = 0; i < n; ++i ) {
            v.push_back(((int*)&param.ai)[i]);
        }
        v = genNext(v, rnd);
        for ( int i = 0; i < n; ++i ) {

            ((int*)&param.ai)[i] = v[i];
        }
        param.score = -1;
        return param;
    }
    ai_info gen2(ai_info param1, ai_info param2, int out, AI::Random& rnd) {
        std::vector<int> v1;
        std::vector<int> v2;
        int n = sizeof(AI::AI_Param) / sizeof(int);
        for ( int i = 0; i < n; ++i ) {
            v1.push_back(((int*)&param1.ai)[i]);
            v2.push_back(((int*)&param2.ai)[i]);
        }
        v1 = genNext2(v1, v2, out, rnd);
        for ( int i = 0; i < n; ++i ) {
            if ( v1[i] > 99 ) v1[i] = 99; // TODO
            if ( v1[i] < -99 ) v1[i] = -99;

            ((int*)&param1.ai)[i] = v1[i];
        }
        param1.score = -1;
        return param1;
    }
    bool loadResult(char filename[] = "param.txt") {
        FILE * fp = fopen(filename, "r");
        if ( fp ) {
            int n = sizeof(AI::AI_Param) / sizeof(int);
            ai.clear();
            for ( size_t i = 0; ; ++i ) {
                while ( i >= ai.size() ) {
                    ai_info p;
                    ai.push_back(p);
                }
                for ( int j = 0; j < n; ++j ) {
                    fscanf(fp, "%d,", ((int*)&ai[i].ai) + j);
                }
                fscanf(fp, "%d", &ai[i].score);
                if ( feof( fp ) ) {
                    break;
                }
            }
            ai.pop_back();
            fclose(fp);
            nround = ai.size() / 2;
            if ( ai.size() > 0 )
                return true;
            else
                return false;
        } else {
            return false;
        }
    }
    void saveResult(char filename[] = "param.txt", bool append = false) {
        FILE * fp = fopen(filename, (append ? "a": "w") );
        if ( fp ) {
            int n = sizeof(AI::AI_Param) / sizeof(int);
            for ( auto v: ai) {
                for ( int i = 0; i < n; ++i ) {
                    fprintf(fp, "%4d,", ((int*)&v.ai)[i]);
                }
                fprintf(fp, "%4d", v.score);
                fprintf(fp, "\n");
            }
            fprintf(fp, "\n");
            fclose(fp);
        }
    }
    void newgame(AI::AI_Param& p1, AI::AI_Param& p2, AI::Random& rnd) {
        if ( --nround < 0 ) {
            nround = ai.size() / 2;
            std::random_shuffle( ai.begin(), ai.end() );
        }
        /*
        if ( ai.size() <= 4 ) {
            saveResult("good.txt");
            while ( ai.size() < 20 ) {
                int g = rnd.randint(3);
                if ( g < 2 ) {
                    int i = rnd.randint(ai.size()-1);
                    ai.push_front( gen2(ai[i], ai[ai.size()-1], g, rnd) );
                } else {
                    ai.push_front( gen(ai[ai.size()-1], rnd) );
                }
                auto it = ai.begin();
                ++it;
                std::random_shuffle( it, ai.end() );
            }
        } // */
        /*
        for ( size_t i = 0; i < ai.size(); ++i ) {
            if ( ai[0].score == -1 ) break;
            ai.push_back(ai.front());
            ai.pop_front();
        }
        if ( ai[0].score != -1 ) {
            std::sort(ai.begin(), ai.end());
            while ( ai.size() > 5 ) ai.pop_front();
            saveResult("good.txt");
            std::vector<ai_info> next;
            for ( size_t i = 0; i < ai.size(); ++i) {
                ai[i].score = -1;
            }
            for ( size_t i = 0; i < ai.size(); ++i) {
                for ( size_t j = i + 1; j < ai.size(); ++j) {
                    next.push_back( gen2(ai[i], ai[j], 0, rnd) );
                    next.push_back( gen2(ai[i], ai[j], 1, rnd) );
                }
            }
            for ( auto v: ai) {
                for ( int i = 0; i < 1; ++ i) {
                    next.push_back( gen(v, rnd) );
                }
            }
            for ( auto v: next) {
                ai.push_back(v);
            }
            for ( size_t i = 0; i < ai.size(); ++i ) {
                if ( ai[0].score == -1 ) break;
                ai.push_back(ai.front());
                ai.pop_front();
            }
        }
        // */
        //*
        volatile int ai_size = ai.size();
        while ( ai_size <= (AI_TRAINING_DEEP == 0 ? AI_TRAINING_0 : AI_TRAINING_2) ) {
            if ( !find_best ) {
                best_ai = ai;
                ai_size = ai.size();
            }
            if ( find_best && best_ai.empty() ) {
                saveResult("good.txt", true);
                state_find_best = true;
                best_ai = ai;
            }
            if ( !find_best || ai.size() == 1 )
            {
                saveResult("best.txt", true);
                state_find_best = false;
                ai = best_ai;
                ai_size = best_ai.size();
                best_ai.clear();

                std::vector<ai_info> next;
                int ai_cur_size = ai_size;
                for ( int i = 0; i < ai_cur_size; ++i) {
                    ai[i].score = -1;
                    for ( int j = i + 1; j < ai_cur_size; ++j) {
                        next.push_back( gen2(ai[i], ai[j], 0, rnd) );
                        next.push_back( gen2(ai[i], ai[j], 1, rnd) );
                        ai_size += 2;
                    }
                }
                for ( auto v: ai) {
                    for ( int i = 0; i < 0; ++ i) {
                        next.push_back( gen(v, rnd) );
                        ai_size += 1;
                    }
                }

                if ( ai.size() == 1 ) {
                    for ( auto v: ai) {
                        for ( int i = 0; i < AI_TRAINING_0; ++ i) {
                            next.push_back( gen(v, rnd) );
                            ai_size += 1;
                        }
                    }
                }

                
                for ( auto v: next) {
                    ai.push_back(v);
                }
                std::random_shuffle( ai.begin(), ai.end() );
                nround = ai_size / 2;
            }
            if ( find_best ) break;
        }
        // */
        p1 = ai[0].ai;
        p2 = ai[1].ai;
        //p2 = ai[0].ai;
        m_p1 = m_p2 = 0;
        m_p1_score = 0;
        m_p2_score = 0;
        m_p2_sqr_score = 0;
        m_p2_last_pieces = 0;
        m_p2_att_val = 0;
        m_p2_seed.clear();
        for ( int i = 0; i < round; ++i) {
            m_p2_seed.push_back(rnd.rand());
        }
        saveResult();
    }
    void result(int p1_win, int p1_score, int p2_win, int p2_score) {
        ai[0].score = p1_score;
        ai[1].score = p2_score;
        if ( !state_find_best && abs(p1_win - p2_win) < (p1_win + p2_win) / 3 ) {
            ai.push_back(ai.front());
            ai.pop_front();
            ai.push_back(ai.front());
            ai.pop_front();
        } else if ( p1_win > p2_win ) {
            ai.push_back(ai.front());
            ai.pop_front();
            ai.pop_front();
        } else if ( p1_win < p2_win ) {
            ai.pop_front();
            ai.push_back(ai.front());
            ai.pop_front();
        }
    }
    void result(int score) {
        ai[0].score = score;
        ai.push_back(ai.front());
        ai.pop_front();
    }
    void game(TetrisGame& p1, TetrisGame& p2, AI::Random& rnd) {
        /*
        if ( !m_init || !p2.alive() ) {
            if ( m_init ) {
                m_p2++;
                m_p2_score += p2.total_atts;
                m_p2_sqr_score += p2.total_atts * p2.total_atts;
                if ( m_p2 >= round ) {
                    double d = 0;
                    d = m_p2_score / (double)m_p2;
                    d *= d;
                    d = m_p2_sqr_score / (double)m_p2 - d;
                    result( int(m_p2_score - sqrt(d) * m_p2) );
                    newgame( p1.m_ai_param, p2.m_ai_param, rnd );
                }
            } else {
                newgame( p1.m_ai_param, p2.m_ai_param, rnd );
            }
            p2.reset( m_p2_seed[m_p2] );
            m_init = true;
        } else {
            if ( p2.n_pieces != m_p2_last_pieces ) {
                m_p2_last_pieces = p2.n_pieces;
                if ( p2.total_atts < 7 ) {
                    m_p2_att_val += 20;
                } else if ( p2.total_atts < 11 ) {
                    m_p2_att_val += 20;
                } else if ( p2.total_atts < 14 ) {
                    m_p2_att_val += 15;
                } else {
                    m_p2_att_val += 10;
                }
                //if ( std::binary_search(attmap, &attmap[23], m_p2_last_pieces) ) {
                //if ( m_p2_last_pieces % 12 == 0 ) {
                {
                    if ( p2.total_atts < 7 ) {
                        if ( m_p2_att_val >= 120 * 4 ) {
                            m_p2_att_val -= 120 * 4;
                            p2.acceptAttack ( 3 );
                        }
                    } else {
                        if ( m_p2_att_val >= 120 * 3 ) {
                            m_p2_att_val -= 120 * 3;
                            p2.acceptAttack ( 2 );
                        }
                    }
                }
            }
        }
        // */
        //*
        if ( !m_init ) {
            newgame( p1.m_ai_param, p2.m_ai_param, rnd );
            m_init = true;
        }
        if ( !p1.alive() || !p2.alive() ) {
            if ( p1.total_clears > 0 ) {
                m_p1_score += p1.total_atts * 1000 / p1.total_clears;
            }
            if ( p2.total_clears > 0 ) {
                m_p2_score += p2.total_atts * 1000 / p2.total_clears;
            }
            if ( false && p1.total_atts != p2.total_atts ) {
                if ( p1.total_atts > p2.total_atts ) m_p1++;
                else m_p2++;
            } else {
                if ( p1.alive() ) m_p1++;
                else m_p2++;
            }

            if ( (m_p1 + m_p2) % 2 == 0) {
                if ( m_p1 >= round || m_p2 >= round
                    || m_p1 >= 10 && (m_p1 + 1) / (m_p2 + 1) > 1
                    || m_p2 >= 10 && (m_p2 + 1) / (m_p1 + 1) > 1
                    || abs(m_p1 - m_p2) > round / 3
                    || min(m_p1, m_p2) > round - round / 3
                    ) {
                        result(m_p1, m_p1_score / (m_p1 + m_p2), m_p2, m_p2_score / (m_p1 + m_p2));
                        newgame(p1.m_ai_param, p2.m_ai_param, rnd);
                }
            }
            {
                unsigned seed = rnd.rand();
                p1.reset(seed);
                p2.reset(seed);
                int pass = rnd.randint( 1024 );
                for ( int i = 0; i < pass; ++i ) {
                    p1.m_rand.rand();
                    p2.m_rand.rand();
                }
                void onGameStart(TetrisGame& tetris, AI::Random& rnd, int index);
                onGameStart(p1, rnd, 0);
                onGameStart(p2, rnd, 1);
            }
            std::swap(m_p1, m_p2);
            std::swap(m_p1_score, m_p2_score);
            std::swap(p1.m_ai_param, p2.m_ai_param);
            std::swap(p1.n_win, p2.n_win);
        }
        // */
    }
    volatile int state_find_best;
    volatile bool find_best;
    bool m_init;
    std::deque<ai_info> ai;
    std::deque<ai_info> best_ai;
    int round;
    int nround;
    int m_p1, m_p2;
    std::vector<unsigned> m_p2_seed;
    int m_p1_score;
    int m_p2_score;
    int m_p2_sqr_score;
    int m_p2_last_pieces;
    int m_p2_att_val;
};
#endif
