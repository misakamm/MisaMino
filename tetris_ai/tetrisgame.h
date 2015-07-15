#pragma once

#include <time.h>
#include <vector>
#include <string>
#include <algorithm>

#include "tetris.h"
#include "tetris_ai.h"
#include "random.h"

#include "sound.h"

#include "tetris_setting.h"

class TetrisGame : public AI::Tetris {
public:
    TetrisGame() {
        m_base = AI::point(0, 30);
        m_size = AI::point(20, 20);
        hold = true;
        ai_movs_flag = -1;
        reset((unsigned)time(0));
        ai_delay = 0;
        mov_llrr = 0;
        env_change = 0;
        n_win = 0;
        last_max_combo = 0;
        mSFXon = false;
        m_lr = 0;
        pAIName = NULL;
        pTetrisAI = NULL;
        AI::AI_Param param = {

//  18,  37,  11,  12,   9,   2, -30, -36, -14,   0,   8,  -2,   9,
//  21,  30,  -4,   9,   0,   5, -22,  -6, -12,   9,   4,  -3,  14
//  16,  29,   7,   7,   50,   6, -23, -24,  -4,   9,   6,  60,  13
//  16,  29,   7,   7,  50,   6, -23, -24,  -4,   9,   6,  60,   7
//  21,  21,  19,  12,  10,   7, -17,  -2,  -3,   6,   9,  15,  15
//  19,  32,  14,  13,   3,  22, -18, -15, -14,  11,  11,   5,  33
//  21,  30,  15,  13,   5,  -2, -15, -20, -14,   9,   7,   4,  25
//  21,  30,  15,  13,   5,  -2, -15, -20, -14,   9,   7,   30,  25
//  17,  34,  28,  20,  57,   3, -10,  29, -72,  17,  10,   50,  27

//   9,  14,   7,   9,   9,  11,  11,   5,  13,   2,   8,  10,  13
//   8,  13,  11,   7,   7,  12,   6,   2,  12,   3,  10,  12,  18
//   8,  13,  11,   7,   7,  12,   6,   2,  12,   3,  10,  12,  18,  18
//   7,  29,  16,  13,  11,  19,   7,   7,  20,   6,  14,  13,  13,  13
//  12,  14,   8,  15,   8,  14,   8,   4,  19,   2,  14,  10,  11,  11
//  21,  27,  15,   6,  40,   8,  20,   6,   7,  -6,   3,  16,   8,   8
//  19,  27,  14,   5,  17,  12,  19,   5,  11,  -2,   3,  13,  10,  14
//  13,  53,  45,  17,  15,  11,  12,   5,   7,   4,   8,  17,  18,   3
//  17,  26,  18,   1,   9,  12,  16,   4,  16,  -4,  11,   7,  11,  14
//  19,  25,  20,   6,  23,  13,  12,   3,  14,   5,  13,  10,  13,  26
//  11,  27,  21,   3,  28,  10,  17,   5,  20,  -2,  11,  15,  31,  11
//   8,  26,  16,   5,  23,  12,   7,   5,   8,  -8,  14,  15,   9,  18 // APL 高
//   8,  26,  21,   7,  21,  12,   5,   5,   1,  -3,  13,  11,  11,   4
//  13,  28,  21,  14,  25,  11,  21,   6,   8,  -3,  13,  24,  31,  1
//   8,  26,  21,   7,  21,  12,   5,   5,   1,  -3,  13,  11,  11,   4
//  10,  10,  10,  10,  10,  10,  10,  10,  10,   0,  10,  10,  10,  10,
//   8,  22,  12,   9,  11,   6,   7,   5,  15,   2,   8,   7,  10,  16
//  12,  29,  22,  13,  28,  11,  17,   7,  20,  -1,  15,  13,  26,  17
//   9,  28,  24,  10,  13,  12,   3,   3,  14,  -3,  11,  28,  26,   9
//  8,  26,  21,   7,  21,  12,   5,   5,  20,  -3,  13,  21,  11,   4
//   8,  38,  18,  13, -14,  19,   7,   4,  14, -15,   9,  19,  16,  -3
//   5,  38,  20,   5,   8,  39,   7,   8, -13,  -5,   4,  24,   7, -20
//  36,  34,  50,   8,  12,  12,   7,  10,   2,   6,  11,   10,   9,   6 // 挖挖机
//  22,  36,  56,  17,  17,  29,   6,   6,   6,   7,  18,  13,   8,  21
//   3,  34,  38,  12,  20,  16,  10,   7,  10,  10,  14,  35,  12,   0
//  26,  35,  45,  14,  -2,  10,   6,   7,   4,   8,  13,  14,   8,  1
//  27,  43,  52,   9,  53,   8,  13,  11,   3,   0,  18,  11,  17,  20
//  32,  43,  60,   9,   8,  24,  13,   8,   2,   7,  20,  10,  13,  21
//  28,  41,  63,   8,  30,   9,  12,  10,  -5,   6,  15,   9,  20,  21
//  43,  43,  64,  21,  43,  12,  10,  16,  20,  22,   0,  17,  57,  12
//  28,  41,  69,  28,  0,  16,  34,  12,   4,  21,   3,  0,  20,  14, 50
//  28,  41,  69,  28,  0,  16,  34,  12,   4,  21,   3,  12,  20,  14, 50, 1000
//  40,  32,  76,  16,  41,  17,  13,  21,  10,  21,   3,  21,  70,  17,  54
//  28,  41,  69,  28,  51,  16,  34,  12,   4,  21,   3,  12,  20,  14, 50, 0
//  47,  26,  70,   4,  46,  16,  26,  21,   7,  31,  14,  17,  69,   1,  43, 300
//  34,  18,  61,   9,  39,  13,  22,  13,  -9,  38,   9,  15,  64,   9,  36, 300

//  47,  26,  70,   4,  46,  16,  26,  21,   7,  31,  14,  17,  69,  11,  53, 300

//  33,  25,  57,  19,  37,  11,  33,  10,   9,  38,  12,  13,  63,  11,  51, 300
//  37,  24,  50,  29,  53,  15,  26,  12,  13,  36,  11,   7,  69,  12,  53, 300
//  36,  25,  50,  20,  55,  15,  28,  12,  15,  36,  12,  10,  70,  12,  53, 300
//  40,  15,  50,  20,  56,  16,  17,  12,   7,  55,  99,  23,  78,  16,  67, 300 // 1.4.1
//  21,  20,  66,  40,  27,  22,  48,  26,   8,  71,  13,  24,  92,   7,  69,  300
//  49,  18,  76,  33,  39,  27,  32,  25,  22,  99,  41,  29,  96,  14,  60,  300 //1.4.3

//  40,  27,  65,  22,  37,  18,  66,  28,   9,  32,   4,  16,  93,  10,  27,  300,

//  45,  28,  84,  21,  35,  27,  56,  30,   9,  64,  13,  18,  97,  11,  29,  300 //1.4.4
//  40,  20,  98,  13,  35,  22,  63,  29,   5,  68,  11,  15,  98,  14,  32,  300
//  44,  32,  96,  28,  42,  24,  49,  25,  -6,  58,  17,  20,  51,  10,  32, 300
//  36,  20,  99,  27,  41,  32,  28,  24,  11,  56,  26,  24,  43,  14,  27, 300 //1.4.4
//  44,  13,  98,  31,  51,  30,  53,  27,  16,  56,  29,  27,  34,  16,  24,  300

//  36,  30,  71,  67,  72,  48,  22,  16,  34,  60,   0,  34,  46,  35,  16,  33 // test
//  61,  33,  87,  52,  85,  30,  30,  28,   2,  71,  55,  51,  98,  35,  57,  60 // test
//70,  16,  99,  50,  95,  33,  21,  29,  38,  98,  10,  54,  91,  26,  42,  65
//45,  28,  84,  11,  35,  27,  26,  30,  39,  64,  13,  18,  97,  11,  29,  300 //1.4.4
//13,   9,  17,  10,  29,  25,  39,   2,  12,  19,   7,  24,  21,  16,  14,  19,  20
  26,   0,  20,  17,  45,  41,  -8,   9,  10,  27,  15,  20,  19,  11,  44,   4,   300//,1183

        };
        if ( GAMEMODE_4W ) {
            //param.h_variance = 0;
            param.tspin = 0;
            param.tspin3 = 0;
        }
        m_ai_param = param;
    }
    void reset ( unsigned seed, unsigned pass = 0 ) {
        while ( ai_movs_flag != -1 ) ::Sleep(1);
        last_max_combo = m_max_combo;
        AI::Tetris::reset(seed, 10, 22);
        m_randatt.seed( seed );
        for ( unsigned i = 0; i < pass; ++i ) m_randatt.rand();
        ai_delay = 0;
        mov_llrr = 0;
        env_change = 0;
        ai_movs.movs.clear();
        n_pieces = 0;
        ai_last_deep = 0;
        ai_movs_flag = -1;
        accept_atts.clear();
        m_last_hole_x = -1;
        total_clears = 0;
        total_atts = 0;
        total_sent = 0;
        total_accept_atts = 0;
        m_ko = 0;
        m_att_info = "";
    }
    bool tryXMove(int dx) {
        bool ret = Tetris::tryXMove( dx );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_move.play( m_lr );
        }
        return ret;
    }
    bool tryXXMove(int dx) {
        bool ret = Tetris::tryXMove( dx );
        while ( Tetris::tryXMove(dx) ) ;
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_move.play( m_lr );
        }
        return ret;
    }
    bool tryYMove(int dy) {
        bool ret = Tetris::tryYMove( dy );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_softdrop.play( m_lr );
        }
        return ret;
    }
    bool tryYYMove(int dy) {
        bool ret = Tetris::tryYMove( dy );
        while ( Tetris::tryYMove(dy) ) ;
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_softdrop.play( m_lr );
        }
        return ret;
    }
    bool trySpin(int dSpin) {
        bool ret = Tetris::trySpin( dSpin );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_rotate.play( m_lr );
        }
        return ret;
    }
    bool trySpin180() {
        bool ret = Tetris::trySpin180( );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_rotate.play( m_lr );
        }
        return ret;
    }
    bool tryHold() {
        bool ret = Tetris::tryHold( );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_hold.play( m_lr );
        }
        return ret;
    }
    bool drop() {
        bool ret = Tetris::drop( );
        if ( mSFXon && ret ) {
            GameSound::ins().mSFX_lockdown.play( m_lr );
        }
        return ret;
    }
    void clearSFX( ) {
        if ( m_clear_info.clears <= 0 || ! mSFXon ) return ;
        int att = m_clear_info.attack;
        int b2b = m_clear_info.b2b > 1;
        int combo = m_clear_info.combo;
        att -= b2b;
        att -= AI::getComboAttack( combo );
        if ( m_clear_info.pc ) {
            att -= 6;
            if ( GameSound::ins().mSFX_pc.isOpen() )
                GameSound::ins().mSFX_pc.play( m_lr );
        }
        if ( combo > 1 ) {
            int i = 0;
            for ( ; i < combo - 1 && i < 20; ++i) {
                if ( ! GameSound::ins().mSFX_combo[i].isOpen() ) break;
            }
            if ( --i >= 0 ) {
                if ( GameSound::ins().mSFX_combo[i].isOpen() ) GameSound::ins().mSFX_combo[i].play( m_lr );
            }
        }
        if ( m_clear_info.wallkick_spin && att > 0 ) {
            if ( m_clear_info.wallkick_spin == 2 && (AI::isEnableAllSpin() || m_clear_info.clears == 1) ) {
                if ( b2b ) {
                    GameSound::sound& m = GameSound::ins().mSFX_b2b_tspin[0];
                    if (m.isOpen()) m.play( m_lr );
                } else {
                    GameSound::sound& m = GameSound::ins().mSFX_tspin[0];
                    if (m.isOpen()) m.play( m_lr );
                }
            } else {
                if ( b2b ) {
                    GameSound::sound& m = GameSound::ins().mSFX_b2b_tspin[m_clear_info.clears];
                    if (m.isOpen()) m.play( m_lr );
                } else {
                    GameSound::sound& m = GameSound::ins().mSFX_tspin[m_clear_info.clears];
                    if (m.isOpen()) m.play( m_lr );
                }
           }
        } else if ( m_clear_info.clears > 0 ) {
            if ( b2b ) {
                GameSound::sound& m = GameSound::ins().mSFX_b2b_tetris;
                if (m.isOpen()) m.play( m_lr );
            } else {
                GameSound::sound& m = GameSound::ins().mSFX_clears[m_clear_info.clears - 1];
                if (m.isOpen()) m.play( m_lr );
            }
        }
    }
    bool ko () {
        if ( mSFXon ) {
            if ( m_ko ) {
                if ( GameSound::ins().mSFX_ko.isOpen() ) {
                    GameSound::ins().mSFX_ko.play();
                    return true;
                }
            } else {
                if ( GameSound::ins().mSFX_gameover.isOpen() ) {
                    GameSound::ins().mSFX_gameover.play();
                    return true;
                }
            }
        }
        return false;
    }
    void acceptAttackSFX() {
        if ( mSFXon && GameSound::ins().mSFX_lineattack.isOpen() ) {
            GameSound::ins().mSFX_lineattack.play( m_lr );
        }
    }
    void acceptAttack(int n) {
        int att[2] = {0};
        for ( int i = 0; i < 32; i += 2) {
            att[0] |= 1 << i;
        }
        att[0] &= m_pool.m_w_mask;
        att[1] = ~att[0] & m_pool.m_w_mask;

        int rowdata = m_randatt.randint( m_pool.m_w );
        while ( m_last_hole_x == rowdata ) {
            rowdata = m_randatt.randint( m_pool.m_w );
        }
        m_last_hole_x = rowdata;
        rowdata = ~( 1 << rowdata ) & m_pool.m_w_mask;
        for ( ; n > 0; --n ) {
            if ( ATTACK_MODE == 0 ) addRow( 0 ); // 空气行
            if ( ATTACK_MODE == 1 ) addRow( rowdata ); // TOP行
            if ( ATTACK_MODE == 2 ) addRow( att[n&1] ); // 火拼行
            ++total_accept_atts;
        }
        if ( alive() ) {
            if ( m_pool.isCollide(m_cur_x, m_cur_y, m_cur)  ) {
                m_state = STATE_OVER;
                m_ko = 1;
            }
        }
    }
    bool game() {
        bool ret = AI::Tetris::game();
        if ( ret ) m_piecedelay = 0;
        else ++m_piecedelay;
        if ( GAMEMODE_4W && AI_SHOW ) {
            if ( m_clearLines > 0 ) {
                for ( int i = 1; i <= m_clearLines; ++i) {
                    setRow( poolh() - 40 + i,
                        ~(15 << 3) & m_pool.m_w_mask );
                }
            }
        }
        return ret;
    }
    void soundon( bool on ) {
        mSFXon = on;
    }
public:
    bool hold;
    AI::Moving ai_movs;
    int ai_movs_flag;
    int ai_last_deep;
    int ai_delay;
    AI::AIName_t pAIName;
    AI::TetrisAI_t pTetrisAI;
    int mov_llrr;
    int env_change;
    int n_pieces;
    std::vector<int> accept_atts;
    int m_last_hole_x;
    int n_win;
    int total_clears;
    int total_atts;
    int total_sent;
    int total_accept_atts;
    int last_max_combo;
    int m_ko;
    int m_lr; // 3d音效开启
    int m_piecedelay;
    AI::Random m_randatt;
    AI::AI_Param m_ai_param;
    std::string m_name;
    bool mSFXon;
    mutable std::string m_att_info;
};
