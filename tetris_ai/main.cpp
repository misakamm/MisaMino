#define _CRT_SECURE_NO_WARNINGS
#define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH

#include "tetrisgame.h"
#include "gamemode.h"

#include "graphics.h"

#include "profile.h"
#include <map>


PIMAGE colorCell( int w, int h, color_t normal, color_t lt, color_t rb ) {
    PIMAGE img;
    img = newimage(w, h);
    setbkcolor(normal, img);
    setcolor(lt, img);
    line(0, 0, w, 0, img);
    line(0, h, 0, 0, img);
    line(1, 1, w, 1, img);
    line(1, 1, 1, h, img);
    setcolor(rb, img);
    line(w - 1, 0, w - 1, h - 1, img);
    line(0, h - 1, w, h - 1, img);
    line(w - 2, h - 2, w - 2, 0, img);
    line(w - 2, h - 2, 0, h - 2, img);
    return img;
}

/*
float gemColor[8] = { // QQTetris
    0.0f,
    0.0f,
    300.0f,
    90.0f,
    180.0f,
    60.0f,
    210.0f,
    140.0f,
};
/*/
float gemColor[8] = { // TOP
    0.0f,
    180.0f,
    300.0f,
    30.0f,
    210.0f,
    0.0f,
    140.0f,
    60.0f,
};
//*/

float gemColorS = 0.7f;

void tetris_draw_next(const TetrisGame& tetris, PIMAGE* gem) {
    setcolor(EGERGB(0x40, 0xa0, 0x40));
    for ( int i = 1; i <= 1; ++i ) {
        int bx = int(tetris.m_base.x + 5);
        int by = int(tetris.m_base.y + tetris.m_size.y * 1);
        rectangle(bx - i,
            by - i,
            int(bx + tetris.m_size.x * 4) + i,
            int(by + tetris.m_size.y * 3) + i
            );
    }
    outtextxy(int(tetris.m_base.x + 5), int(tetris.m_base.y), "HOLD");
    if ( tetris.m_pool.m_hold > 0 ) {
        AI::Gem hold = AI::getGem( tetris.m_pool.m_hold, 0 );
        for ( int y = 0; y < 4; ++y) {
            for ( int x = 0; x < 4; ++x) {
                if ( hold.bitmap[y] & ( 1 << x ) ) {
                    int bx = int(tetris.m_base.x + 5 + tetris.m_size.x * x);
                    int by = int(tetris.m_base.y + tetris.m_size.y * (y + 1));
                    if ( tetris.m_pool.m_hold > 1 && tetris.m_pool.m_hold < 7 ) {
                        bx += int(tetris.m_size.x / 2);
                    }
                    if ( tetris.m_pool.m_hold > 1 ) {
                        by += int(tetris.m_size.y / 2);
                    }
                    putimage(bx, by, gem[tetris.m_pool.m_hold]);
                }
            }
        }
    }
    setcolor(EGERGB(0x40, 0x40, 0xff));
    xyprintf(int(tetris.m_base.x + tetris.m_size.x * (tetris.poolw() + 5) + tetris.m_size.x / 2), int(tetris.m_base.y),
        "NEXT %3d", (tetris.n_pieces - (tetris.m_pool.m_hold == AI::GEMTYPE_NULL)) - 1);
    for ( int i = 1; i <= 1; ++i ) {
        int bx = int(tetris.m_base.x + tetris.m_size.x * (tetris.poolw() + 5) + tetris.m_size.x / 2);
        int by = int(tetris.m_base.y + tetris.m_size.y * 1 );
        rectangle(bx - i,
            by - i,
            int(bx + tetris.m_size.x * 4) + i,
            int(by + tetris.m_size.y * 3 * DISPLAY_NEXT_NUM) + i
            );
    }
    for ( int n = 0; n < DISPLAY_NEXT_NUM; ++n) {
        for ( int y = 0; y < 4; ++y) {
            for ( int x = 0; x < 4; ++x) {
                if (tetris.getNextGemCell(n, x, y)) {
                    int bx = int(tetris.m_base.x + tetris.m_size.x * (tetris.poolw() + 5) + tetris.m_size.x * x + tetris.m_size.x / 2);
                    int by = int(tetris.m_base.y + tetris.m_size.y * (y + 1 + n * 3));
                    if ( tetris.m_next[n].num > 1 && tetris.m_next[n].num < 7 ) {
                        bx += int(tetris.m_size.x / 2);
                    }
                    if ( tetris.m_next[n].num > 1 ) {
                        by += int(tetris.m_size.y / 2);
                    }
                    putimage(bx, by, gem[tetris.m_next[n].num]);
                }
            }
        }
    }
}

void tetris_draw(const TetrisGame& tetris, bool showAttackLine, bool showGrid) {
    static PIMAGE pool_gem[9] = {0}, pool_gem_dark[9] = {0};
    PIMAGE* gem = pool_gem_dark;
    if ( pool_gem[1] == NULL ) {
        for ( int i = 1; i < 8; ++i ) {
            pool_gem[i] = colorCell((int)tetris.m_size.x, (int)tetris.m_size.y,
                hsv2rgb(gemColor[i], gemColorS, 0.6f),
                hsv2rgb(gemColor[i], gemColorS, 0.9f),
                hsv2rgb(gemColor[i], gemColorS, 0.4f));
        }
        pool_gem[8] = colorCell((int)tetris.m_size.x, (int)tetris.m_size.y,
            EGERGB(0x80,0x80,0x80), EGERGB(0xb0,0xb0,0xb0), EGERGB(0x60,0x60,0x60));
        for ( int i = 1; i < 8; ++i ) {
            pool_gem_dark[i] = colorCell((int)tetris.m_size.x, (int)tetris.m_size.y,
                hsv2rgb(gemColor[i], gemColorS, 0.3f),
                hsv2rgb(gemColor[i], gemColorS, 0.45f),
                hsv2rgb(gemColor[i], gemColorS, 0.2f));
        }
        pool_gem_dark[8] = colorCell((int)tetris.m_size.x, (int)tetris.m_size.y,
            EGERGB(0x40,0x40,0x40), EGERGB(0x50,0x50,0x50), EGERGB(0x30,0x30,0x30));
    }
    setbkmode(TRANSPARENT);
    tetris_draw_next(tetris, pool_gem);
    if ( tetris.alive() ) {
        gem = pool_gem;
    }
    int base_y = int(tetris.m_base.y - tetris.m_size.y);
    int min_y[32];
    if ( showGrid ) {
        setcolor(EGERGB(0x20, 0x20, 0x20));
        for ( int x = 1; x < tetris.poolw(); ++x) {
            int bx = int(tetris.m_base.x + tetris.m_size.x * (x + 5));
            int by = int(base_y + tetris.m_size.y * 3);
            line_f( bx, by - 5, bx, by + (int)(tetris.m_size.y * (tetris.poolh() - 2)) );
            line_f( bx-1, by - 5, bx-1, by + (int)(tetris.m_size.y * (tetris.poolh() - 2)) );
        }
        for ( int y = 3; y <= tetris.poolh(); ++y) {
            int bx = int(tetris.m_base.x + tetris.m_size.x * (5));
            int by = int(base_y + tetris.m_size.y * y);
            line_f( bx, by, bx + (int)(tetris.m_size.x * tetris.poolw()), by);
            line_f( bx, by-1, bx + (int)(tetris.m_size.x * tetris.poolw()), by-1);
        }
    }
    for ( int x = 0; x < tetris.poolw(); ++x) {
        min_y[x] = tetris.poolh() + 1;
        for ( int y = 0; y <= tetris.poolh(); ++y) {
            if (tetris.getPoolCell(x, y)) {
                int bx = int(tetris.m_base.x + tetris.m_size.x * (x + 5));
                int by = int(base_y + tetris.m_size.y * y);
                if ( y == 2 ) putimage(bx, by + (int)tetris.m_size.y - 5, (int)tetris.m_size.x, 5, gem[tetris.getPoolCell(x, y)], 0, (int)tetris.m_size.y - 5);
                else if ( y > 2 ) putimage(bx, by, gem[tetris.getPoolCell(x, y)]);
                min_y[x] = min( min_y[x], y );
            }
        }
    }

    setcolor(EGERGB(0xa0, 0x0, 0x80));
    for ( int i = 1; i <= 3; ++i ) {
        rectangle(int(tetris.m_base.x + tetris.m_size.x * (5)) - i,
                    int(tetris.m_base.y + tetris.m_size.y * 2) - 5 - i,
                    int(tetris.m_base.x + tetris.m_size.x * (tetris.poolw() + 5)) + i,
                    int(tetris.m_base.y + tetris.m_size.y * (tetris.poolh())) + i
                    );
    }

    if ( tetris.alive() && tetris.m_cur.num ) {
        int dy = 0;
        if ( ! tetris.m_pool.isCollide( tetris.m_cur_x, tetris.m_cur_y + dy, tetris.m_cur ) ) {
            while ( ! tetris.m_pool.isCollide( tetris.m_cur_x, tetris.m_cur_y + dy + 1, tetris.m_cur ) ) {
                ++dy;
            }
        }
        if ( dy > 0 ) {
            //setlinewidth(3);
            setfillcolor( hsv2rgb(gemColor[tetris.m_cur.num], gemColorS, 0.6f) );
            //setcolor(EGERGB(0xcf, 0x4f, 0xcf));
            for ( int y = 0; y < 4; ++y) {
                for ( int x = 0; x < 4; ++x) {
                    if ( y + dy + tetris.cury() <= 2 ) continue;
                    if (tetris.getCurGemCell(x, y)) {
                        int bx = int(tetris.m_base.x + tetris.m_size.x * (x + tetris.curx() + 5));
                        int by = int(base_y + tetris.m_size.y * (y + dy + tetris.cury()));
                        int bw = tetris.m_size.x;
                        int bh = tetris.m_size.y;
                        //putimage(bx, by, pool_gem_dark[tetris.m_cur.num]);
                        if ( x == 0 || tetris.getCurGemCell(x-1, y) == 0 ) { //L
                            bar( bx, by, bx + 3, by + bh );
                        }
                        if ( y == 0 || tetris.getCurGemCell(x, y-1) == 0 ) { //T
                            bar( bx, by, bx + bw, by + 3 );
                        }
                        if ( x == 3 || tetris.getCurGemCell(x+1, y) == 0 ) { //R
                            bar( bx + bw - 3, by, bx + bw, by + bh );
                        }
                        if ( y == 3 || tetris.getCurGemCell(x, y+1) == 0 ) { //B
                            bar( bx, by + bh - 3, bx + bw, by + bh);
                        }
                        bar( bx, by, bx + 3, by + 3 );
                        bar( bx + bw - 3, by, bx + bw, by + 3 );
                        bar( bx + bw - 3, by + bh - 3, bx + bw, by + bh );
                        bar( bx, by + bh - 3, bx + 3, by + bh );
                    }
                }
            }
            //setlinewidth(1);
        }
    }

    for ( int y = 0; y < 4; ++y) {
        for ( int x = 0; x < 4; ++x) {
            if ( tetris.cury() + y <= 1 ) continue;
            if ( tetris.getCurGemCell(x, y) ) {
                int bx = int(tetris.m_base.x + tetris.m_size.x * (x + tetris.curx() + 5));
                int by = int(base_y + tetris.m_size.y * (y + tetris.cury()));
                if ( tetris.cury() + y == 2 ) {
                    putimage(bx, by + (int)tetris.m_size.y - 5, (int)tetris.m_size.x, 5, pool_gem[tetris.m_cur.num], 0, (int)tetris.m_size.y - 5);
                } else {
                    putimage(bx, by, pool_gem[tetris.m_cur.num]);
                }
            }
        }
    }
    if ( ! tetris.accept_atts.empty() ) {
        int atts = 0;
        for ( std::vector<int>::const_iterator it = tetris.accept_atts.begin(); it != tetris.accept_atts.end(); ++it ) {
            atts += *it;
        }

        setfillcolor(hsv2rgb(0.0f, 1.0f, 1.0f));
        int bx = int(tetris.m_base.x + tetris.m_size.x * (4)) + tetris.m_size.x / 2;
        int by = int(base_y + tetris.m_size.y * (tetris.poolh() + 1) );
        bar(bx,
            by - (int)tetris.m_size.y * atts,
            bx + (int)tetris.m_size.x / 2,
            by);
        if ( showAttackLine ) {
            setcolor (EGERGB(0x80, 0x0, 0x0));
            setlinestyle( DOTTED_LINE );
            int lastbx = -1, lastby = 0;
            for ( int x = 0; x < tetris.poolw(); ++x) {
                int y = min_y[x];
                int bx = int(tetris.m_base.x + tetris.m_size.x * (x + 5));
                int by = int(base_y + tetris.m_size.y * (y - atts));
                if ( lastbx != -1 && lastby != by ) {
                    line( lastbx, lastby, bx, by);
                }
                line( bx, by, bx + (int)tetris.m_size.x, by);
                lastbx = bx + (int)tetris.m_size.x;
                lastby = by;
            }
            setlinestyle( SOLID_LINE );
        }
    }
    {
        setcolor (EGERGB(0xa0, 0xa0, 0xa0));
        int combo = tetris.m_pool.combo;
        if ( combo > 0 ) combo--;
        //xyprintf(int(tetris.m_base.x + tetris.m_size.x * (5)), int(tetris.m_base.y + tetris.m_size.y * ( tetris.poolh() + 0 ) + 1 ),
        //    "Win %2d Att %2d Combo %d/%d/%d", tetris.n_win, tetris.total_atts, combo, tetris.m_max_combo, tetris.last_max_combo);
        double apl = 0, app = 0, pps = 0, apm = 0;
        if ( tetris.total_clears > 0 ) {
            apl = double(tetris.total_atts)/tetris.total_clears;
        }
        if ( tetris.n_pieces > 0 ) {
            app = double(tetris.total_atts)/tetris.n_pieces;
        }
        if ( tetris.m_drop_frame > 0 ) {
            pps = (tetris.n_pieces - 1) * 60.0 / tetris.m_drop_frame;
        }
        if ( tetris.m_drop_frame > 0 ) {
            apm = tetris.total_atts * 60.0 * 60.0 / tetris.m_drop_frame;
        }
        rectprintf(
            //int(tetris.m_base.x + tetris.m_size.x * (5+tetris.poolw())) + 1,
            //int(tetris.m_base.y + tetris.m_size.y * ( tetris.poolh() - 5 ) ),
            int(tetris.m_base.x) + 2,
            int(tetris.m_base.y + tetris.m_size.y * ( 4 ) + 1 ),
            200,
            400,
#ifdef XP_RELEASE
            "Win %4d\nPPS %.2f\nAPM %4.1f\nAtt %4d\nSent %3d\nRecv %3d\nAPL %.2f\nAPP %.3f\nCombo %2d\nClear %2d\nCbA %4d\nB2B %4d\nT0 %5d\nT1 %5d\nT2 %5d\nT3 %5d\n1 %6d\n2 %6d\n3 %6d\n4 %6d\n",
            tetris.n_win, pps, apm, tetris.total_atts, tetris.total_sent, tetris.total_accept_atts, apl, app, tetris.m_max_combo,
            tetris.m_clear_info.total_pc, tetris.m_clear_info.total_cb_att, tetris.m_clear_info.total_b2b,
            tetris.m_clear_info.t[0], tetris.m_clear_info.t[1], tetris.m_clear_info.t[2], tetris.m_clear_info.t[3],
            tetris.m_clear_info.normal[1], tetris.m_clear_info.normal[2], tetris.m_clear_info.normal[3],tetris.m_clear_info.normal[4]
#else
            "Win %4d\nPPS %.2f\nAPM %4.1f\nAtt %4d\nSent %3d\nRecv %3d\nAPL %.2f\nAPP %.3f\nCombo %2d\nClear %2d\nCbA %4d\nB2B %4d\nT0 %5d\nT1 %5d\nT2 %5d\nT3 %5d\n1 %6d\n2 %6d\n3 %6d\n4 %6d\n",
            tetris.n_win, pps, apm, tetris.total_atts, tetris.total_sent, tetris.total_accept_atts, apl, app, tetris.m_max_combo,
            tetris.m_clear_info.total_pc, tetris.m_clear_info.total_cb_att, tetris.m_clear_info.total_b2b,
            tetris.m_clear_info.t[0], tetris.m_clear_info.t[1], tetris.m_clear_info.t[2], tetris.m_clear_info.t[3],
            tetris.m_clear_info.normal[1], tetris.m_clear_info.normal[2], tetris.m_clear_info.normal[3],tetris.m_clear_info.normal[4]
#endif
            );
    }
    if ( tetris.m_clear_info.attack > 0 ) {
        std::string info;
        char str[128];
        int att = tetris.m_clear_info.attack;
        int b2b = tetris.m_clear_info.b2b > 1;
        int combo = tetris.m_clear_info.combo;
        int wallkick_spin = tetris.m_clear_info.wallkick_spin;
        {
            sprintf(str, "+%d ", att);
            info += str;
        }
        att -= b2b;
        att -= AI::getComboAttack( combo );
        if ( tetris.m_clear_info.pc ) {
            att -= 6;
        }

        if ( wallkick_spin ) {
            char gemMap[] = " ITLJZSO";
            if ( wallkick_spin == 2 && (AI::isEnableAllSpin() || tetris.m_clear_info.clears == 1) ) {
            //if ( wallkick_spin == 2 ) {
                sprintf(str, "%c-mini ", gemMap[tetris.m_clear_info.gem_num]);
                info += str;
            } else {
                sprintf(str, "%c-spin%d ", gemMap[tetris.m_clear_info.gem_num], tetris.m_clear_info.clears);
                info += str;
            }
        } else if ( tetris.m_clear_info.gem_num == 1 && tetris.m_clear_info.clears == 4 ) {
            sprintf(str, "Quadruple ");
            info += str;
        } else if ( tetris.m_clear_info.clears == 3 ) {
            sprintf(str, "Triple ");
            info += str;
        } else if ( tetris.m_clear_info.clears == 2 ) {
            sprintf(str, "Double ");
            info += str;
        }
        if ( b2b ) {
            sprintf(str, "b2b ");
            info += str;
        }
        if ( combo > 1 ) {
            sprintf(str, "combo%d ", combo-1);
            info += str;
        }
        if ( tetris.m_clear_info.pc ) {
            sprintf(str, "Clear! ");
            info += str;
        }
        setcolor (EGERGB(0x00, 0xA0, 0xff));
        xyprintf(int(tetris.m_base.x + tetris.m_size.x * (5)), int(tetris.m_base.y + tetris.m_size.y * ( tetris.poolh() + 0 ) + 1 ),
            info.c_str() );
        tetris.m_att_info = info;
    } else {
        setcolor (EGERGB(0x40, 0x40, 0x40));
        xyprintf(int(tetris.m_base.x + tetris.m_size.x * (5)), int(tetris.m_base.y + tetris.m_size.y * ( tetris.poolh() + 0 ) + 1 ),
            tetris.m_att_info.c_str() );
    }
    setcolor (EGERGB(0xa0, 0xa0, 0xa0));
    if ( ! tetris.alive() ) {
        xyprintf(0, 0, "Press F2 to restart a new game. Press F12 to config your controls");
    }
    {
        int w = textwidth(tetris.m_name.c_str());
        if ( tetris.pTetrisAI ) setcolor(EGERGB(0xa0, 0x0, 0xff));
        xyprintf(int(tetris.m_base.x + tetris.m_size.x * ( 5 + tetris.poolw() / 2 )) - w / 2, int(tetris.m_base.y + tetris.m_size.y * 0 ),
            "%s", tetris.m_name.c_str() );
    }
}

void setkeyScene( int player_keys[] ) {
    cleardevice();
    while ( kbmsg() ) getkey();
    const char* name[] = {
        "move left",
        "move right",
        "soft drop",
        "counterclockwise",
        "clockwise",
        "hold",
        "hard drop",
        "180rotate",
    };
    int t_player_keys[16];
    for ( int i = 0; i < 8; ++i ) {
        for ( t_player_keys[i] = 0; is_run() && t_player_keys[i] == 0; delay_fps(60) ) {
            if ( kbmsg() ) {
                key_msg k = getkey();
                if ( k.key == key_esc ) {
                    return ;
                }
                if ( k.msg == key_msg_down ) {
                    t_player_keys[i] = k.key;
                }
            }
            cleardevice();
            xyprintf(0, 0, "press a key for %s (ESC to cancel & return)", name[i]);
        }
    }
    for ( int i = 0; i < 8; ++i ) {
        player_keys[i] = t_player_keys[i];
    }
}

void loadKeySetting( int player_keys[] ) {
    FILE* fp = fopen("misamino.cfg", "r");
    if ( fp ) {
        for ( int i = 0; i < 8; ++i) {
            fscanf(fp, "%d", &player_keys[i]);
        }
        fclose(fp);
    }
}

void saveKeySetting( int player_keys[] ) {
    FILE* fp = fopen("misamino.cfg", "w");
    if ( fp ) {
        for ( int i = 0; i < 8; ++i) {
            fprintf(fp, " %d", player_keys[i]);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
}

void onGameStart(TetrisGame& tetris, AI::Random& rnd, int index) {
    if ( GAMEMODE_4W ) {
        static int hole_index;
        for ( int j = 0; j < 39; ++j) {
            int v = ~(15 << 3) & tetris.m_pool.m_w_mask;
            tetris.addRow(v);
        }
        if ( index == 0 ) {
            hole_index = rnd.randint(4);
        }
        tetris.addRow((((1 << tetris.m_pool.m_w) - 1) & ~(1<<(hole_index + AI::gem_beg_x))) & tetris.m_pool.m_w_mask);
    }
#if !defined( XP_RELEASE ) //|| P1_AI
    if ( index == 0 ) tetris.addRow(15);
    else tetris.addRow(15 << 6);
#endif
    if (0)
    {
        char n[] = "ITSOJLITZJISOTZ";
        std::map<char, int> gemMap;
        gemMap[' '] = AI::GEMTYPE_NULL;
        gemMap['I'] = AI::GEMTYPE_I;
        gemMap['T'] = AI::GEMTYPE_T;
        gemMap['L'] = AI::GEMTYPE_L;
        gemMap['J'] = AI::GEMTYPE_J;
        gemMap['Z'] = AI::GEMTYPE_Z;
        gemMap['S'] = AI::GEMTYPE_S;
        gemMap['O'] = AI::GEMTYPE_O;
        for ( int k = 0; n[k]; ++k ) {
            tetris.m_next[k] = AI::getGem(gemMap[n[k]], 0);
        }
        tetris.m_pool.m_hold = gemMap['L'];
    }
	if (0)
	{
		int m[22][10] = {
			{0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
			{1, 1, 1, 1, 1, 0, 0, 1, 1, 0},
			{1, 1, 1, 1, 1, 0, 0, 0, 1, 0},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		};
		for ( int j = 0; j < 22; ++j) {
			int v = 0;
			for ( int k = 0; k < 10; ++k ) {
				if ( m[j][k] ) v |= 1 << k;
			}
			tetris.addRow(v);
		}
		tetris.m_next[0] = AI::getGem(AI::GEMTYPE_L, 0);
		tetris.m_next[1] = AI::getGem(AI::GEMTYPE_Z, 0);
		tetris.m_next[2] = AI::getGem(AI::GEMTYPE_T, 0);
		tetris.m_next[3] = AI::getGem(AI::GEMTYPE_S, 0);
		tetris.m_next[4] = AI::getGem(AI::GEMTYPE_I, 0);
	}
}
struct tetris_ai {
    int style;
    int level;
    int PieceMul;
    std::string plugin;
    tetris_ai() {
        style = 2;
        level = 4;
        PieceMul = 0;
        plugin = "dllai.dll";
    }
};

struct tetris_rule {
    int turnbase;
    int garbage;
    int spin180;
    int GarbageCancel;
    int GarbageBuffer;
    int GarbageBlocking;
    int combo_table_style;
    int samesequence;
    int turn;
    tetris_rule() {
        turnbase = 1;
        garbage = 0;
        spin180 = 0;
        GarbageCancel = 1;
        GarbageBuffer = 1;
        GarbageBlocking = 1;
        combo_table_style = 0;
        samesequence = 1;
        turn = 1;
    }
};
struct tetris_player {
    int tojsoftdrop;
    int das;
    int softdropdelay;
    int softdropdas;
    int sound_p1;
    int sound_p2;
    int sound_bgm;
    tetris_player() {
        tojsoftdrop = 1;
        das = 8;
        softdropdelay = 10;
        softdropdas = 10;
        sound_p1 = 1;
        sound_p2 = 0;
        sound_bgm = 1;
    }
};

void loadAI(CProfile& config, tetris_ai ai[]) {
    for ( int i = 0; i < 2; ++i ) {
        if ( i == 0 ) config.SetSection( "AI_P1" );
        else config.SetSection( "AI_P2" );
        if ( config.IsInteger( "style" ) ) {
            ai[i].style = config.ReadInteger( "style" );
            if ( i != 0 && ai[i].style == 0 ) ai[i].style = 2;
        }
        if ( config.IsInteger( "level" ) ) {
            ai[i].level = config.ReadInteger( "level" );
            if ( ai[i].level < 0 ) ai[i].level = 0;
            if ( ai[i].level > 10 ) ai[i].level = 10;
        }
        if ( config.IsInteger( "PieceMul" ) ) {
            ai[i].PieceMul = config.ReadInteger( "PieceMul" );
            if ( ai[i].PieceMul <= 0 ) ai[i].PieceMul = 1;
        }
        config.ReadString( "dllplugin", ai[i].plugin );
    }
}
void loadRule(CProfile& config, tetris_rule& rule) {
    config.SetSection( "Rule" );
    if ( config.IsInteger( "turnbase" ) ) {
        rule.turnbase = config.ReadInteger( "turnbase" );
    }
    if ( config.IsInteger( "KOS_turnbase" ) ) {
        int kos_turnbase = config.ReadInteger( "KOS_turnbase" );
        if ( kos_turnbase ) rule.turn = 7;
    }
    if ( config.IsInteger( "spin180" ) ) {
        rule.spin180 = config.ReadInteger( "spin180" );
        AI::setSpin180( rule.spin180 );
    }
    if ( config.IsInteger( "GarbageStyle" ) ) {
        rule.garbage = config.ReadInteger( "GarbageStyle" );
    }
    if ( config.IsInteger( "GarbageCancel" ) ) {
        rule.GarbageCancel = config.ReadInteger( "GarbageCancel" );
    }
    if ( config.IsInteger( "GarbageBuffer" ) ) {
        rule.GarbageBuffer = config.ReadInteger( "GarbageBuffer" );
    }
    if ( config.IsInteger( "GarbageBlocking" ) ) {
        rule.GarbageBlocking = config.ReadInteger( "GarbageBlocking" );
    }
    if ( config.IsInteger( "samesequence" ) ) {
        rule.samesequence = config.ReadInteger( "samesequence" );
    }
    if ( config.IsInteger( "combo_table_style" ) ) {
        rule.combo_table_style = config.ReadInteger( "combo_table_style" );
    }
}
void loadPlayerSetting(CProfile& config, tetris_player& player) {
    config.SetSection( "Player" );
    if ( config.IsInteger( "tojsoftdrop" ) ) {
        player.tojsoftdrop = config.ReadInteger( "tojsoftdrop" );
    }
    if ( config.IsInteger( "das" ) ) {
        player.das = config.ReadInteger( "das" );
        if ( player.das < 0 ) player.das = 0;
    }
    if ( config.IsInteger( "softdropdas" ) ) {
        player.softdropdas = config.ReadInteger( "softdropdas" );
        if ( player.softdropdas < 0 ) player.softdropdas = 0;
    }
    if ( config.IsInteger( "softdropdelay" ) ) {
        player.softdropdelay = config.ReadInteger( "softdropdelay" );
        if ( player.softdropdelay < 0 ) player.softdropdelay = 0;
    }
    config.SetSection( "Sound" );
    if ( config.IsInteger( "p1sfx" ) ) {
        player.sound_p1 = config.ReadInteger( "p1sfx" );
    }
    if ( config.IsInteger( "p2sfx" ) ) {
        player.sound_p2 = config.ReadInteger( "p2sfx" );
    }
    if ( config.IsInteger( "bgm" ) ) {
        player.sound_bgm = config.ReadInteger( "bgm" );
    }
}

std::string getpath( std::string path ) {
    for ( int i = path.size() - 1; path[i] != '/' && path[i] != '\\'; --i) path[i] = 0;
    return path.c_str();
}

void mainscene() {
    tetris_rule rule;
    tetris_player player;
    tetris_ai ai[2];
    int ai_first_delay = 30;
    int ai_move_delay = 20;
    int ai_4w = 1;


    bool showAttackLine = true;
    bool showGrid = true;
    int game_pause = 0;

    CProfile config;
#if !defined( XP_RELEASE ) || P1_AI || !PUBLIC_VERSION
    config.SetFile( "tetris_ai.ini" );
#else
    config.SetFile( "misamino.ini" );
#endif
    if ( 0 )
    {
        config.SetSection( "AI" );
        config.WriteInteger( "level", 6 );
        config.WriteInteger( "depth", 6 );
        config.SetSection( "Rule" );
        config.WriteInteger( "garbage", 0 );
    }
    else
    {
        config.SetSection( "AI" );
        if ( config.IsInteger( "delay" ) ) {
            ai_first_delay = config.ReadInteger( "delay" );
            if ( ai_first_delay < 0 ) ai_first_delay = 0;
        }
        if ( config.IsInteger( "move" ) ) {
            ai_move_delay = config.ReadInteger( "move" );
            if ( ai_move_delay < 0 ) ai_move_delay = 0;
        }
        if ( config.IsInteger( "4w" ) ) {
            ai_4w = config.ReadInteger( "4w" );
        }

        loadAI(config, ai);
        loadRule(config, rule);
        loadPlayerSetting(config, player);

        if ( rule.turn != 1 ) {
            ai[0].PieceMul = ai[1].PieceMul = 1;
        }
    }

    setfont(18, 0, "Courier New", 0, 0, 100, 0, 0, 0 );
    int players_num = 2;
    std::vector<TetrisGame> tetris(players_num);
    AI::Random rnd( (unsigned) time( 0 ) );
    int player_keys[8] = {
        key_left,
        key_right,
        key_down,
        'Z',
        'C',
        'V',
        key_space,
        'X'
    };
    loadKeySetting( player_keys );
#ifndef XP_RELEASE
    EvE eve;
    //eve.loadResult();
    if ( ! eve.loadResult() )
    {
        //std::deque<AI::AI_Param> _p;
        //_p.push_back(tetris[0].m_ai_param);
        //eve.init( _p );
        eve.ai.push_back(tetris[0].m_ai_param);
    }
#endif
#ifdef XP_RELEASE
    int ai_eve = 0;
    int ai_search_height_deep = DISPLAY_NEXT_NUM > 6 ? 6 : DISPLAY_NEXT_NUM;
    int player_stratagy_mode = !AI_SHOW;
    int mainloop_times = 1;
    int normal_delay = 1;
#else
    int ai_eve = 1;
    int ai_search_height_deep = AI_TRAINING_DEEP;
    int player_stratagy_mode = 1;
    int mainloop_times = ( AI_TRAINING_DEEP == 0 ? 1000 : (AI_TRAINING_SLOW ? 2 : 50) );
    int normal_delay = 1;
#endif
#if AI_SHOW == 0
    int player_accept_attack = 1;
    int player_begin_attack = 0; //17;
#else
    int player_accept_attack = 0;
    int player_begin_attack = 0; //17;
#endif
    //int ai_search_normal_deep = 0;
    int ai_mov_time = ( ai_eve | (1&&ai[0].style) ?
        (AI_TRAINING_SLOW ? 12 : 0)
        : (
        (AI_SHOW ) ? ( GAMEMODE_4W ? 2: 16 ) : ( PLAYER_WAIT ? 2 : 10 ) )
        );
    ai_mov_time /= 2; // fps=60
    int ai_mov_time_base = 13; // 实际值由ai_mov_time决定，初始值不用管

    int player_key_state[8] = {0};
    int player_last_key = 0;

    std::string game_info;
    int game_info_time = 0;
    tetris[0].m_name = "Human";
    GameSound::ins().loadSFX( );
    if ( player.sound_bgm ) GameSound::ins().loadBGM_wait( rnd );
    if ( player.sound_p1 ) tetris[0].soundon( true );
    if ( player.sound_p2 ) tetris[1].soundon( true );

    {
        typedef int (*AIDllVersion_t)();
        AIDllVersion_t pAIDllVersion = NULL;
        char path[MAX_PATH];
        ::GetCurrentDirectoryA(MAX_PATH, path);
        for ( int i = 0; i < players_num; ++i ) {
            if ( ai[i].style == -1 )
            {
                ::SetCurrentDirectoryA( (getpath(std::string(path) + "/" + ai[i].plugin)).c_str() );
                HMODULE hModule = ::LoadLibraryA(ai[i].plugin.c_str());
                if ( hModule ) {
                    pAIDllVersion = (AIDllVersion_t)GetProcAddress(hModule, "AIDllVersion");
                    if ( pAIDllVersion && pAIDllVersion() == AI_DLL_VERSION ) {
                        tetris[i].pAIName = (AI::AIName_t)GetProcAddress(hModule, "AIName");
                        tetris[i].pTetrisAI = (AI::TetrisAI_t)GetProcAddress(hModule, "TetrisAI");
                    }
                }
            }
        }
        ::SetCurrentDirectoryA(path);
    }

    if ( 1 )
    {
        AI::AI_Param param = {
//  47,  26,  70,   4,  46,  16,  26,  21,   7,  31,  14,  17,  69,  11,  53, 300

//  33,  25,  57,  19,  37,  11,  33,  10,   9,  38,  12,  13,  63,  11,  51, 300
//  37,  24,  50,  29,  53,  15,  26,  12,  13,  36,  11,   7,  69,  12,  53, 300
//  36,  25,  50,  20,  55,  15,  28,  12,  15,  36,  12,  10,  70,  12,  53, 300

//  40,  15,  50,  20,  56,  16,  17,  12,   7,  55,  99,  23,  78,  16,  67, 300
//  37,  16,  51,  18,  50,  16,  32,  11,   5,  32,  99,  21,  68,   8,  41,   0 //lv1 s lv2 b
//  26,  18,  46,  30,  53,  22,  29,  17,   9,  33,   3,  11,  84,   6,  50, -16 //lv1 b lv2 a
//  28,  21,  57,  28,  48,  17,  18,  12,   8,  29,  17,  22,  78,  10,  65, -10 //lv2 c

//  21,  20,  66,  40,  27,  22,  48,  26,   8,  71,  13,  24,  92,   7,  69, 300 // a
//  19,  24,  99,  35,  24,  20,  52,  30,   9,  77,  13,  32,  91,   9,  69,  83 // b
//  19,  22,  87,  37,  16,  13,  42,  19,   6,  73,  10,  33,  93,   5,  73,  83 // b+
//  21,  24,  54,  33,  23,  24,  36,  16,   8,  70,  12,  25,  73,   8,  67,  92 // c
//  23,  20,  66,  36,  24,  21,  46,  27,  14,  77,  15,  32,  93,   5,  67,  85 // s
//  20,  21,  71,  35,  16,  22,  44,  20,   6,  79,   9,  28,  74,   4,  67,  82 // s

//  49,  18,  76,  33,  39,  27,  32,  25,  22,  99,  41,  29,  96,  14,  60, 300 //s
//  31,  17,  69,  32,  21,  27,  49,  24,  18,  99,  78,  28,  99,   8,  62,  91 //c
//  52,  19,  69,  34,  35,  30,  34,  20,  17,  89,  37,  31,  83,   9,  55,  97 //c
//  53,  16,  73,  33,  26,  28,  30,  22,  22,  79,  18,  28,  93,   5,  63,  95 //a
//  60,  16,  72,  35,  29,  30,  41,  20,  20,  79,  55,  22,  97,   4,  60,  95 // b
//  40,  13,  80,  37,  16,  24,  38,  19,  21,  67,  99,  24,  96,   6,  42,  83 // a

//  49,  18,  76,  40,  39,  27,  32,  50,  22,  99,   0,  29,  96,  14,  60, 300 //s ogn

//  45,  28,  84,  21,  35,  27,  56,  30,   9,  64,  13,  18,  97,  11,  29,  300 // cmp
//  43,  16,  80,  20,  38,  26,  53,  30,   3,  70, -17,  19,  96,  18,  30,  300 // b
//  40,  18,  97,  25,  39,  18,  59,  30,   4,  64,  -9,  17,  97,  17,  31,  300 // b+
//  39,  28,  94,  24,  42,  23,  56,  31,   5,  78,  -8,  14,  96,  13,  33,  300 //b
//  38,  21,  94,  28,  48,  25,  54,  34,   8,  88, -21,  19,  80,  31,  32,  300 //a
//  43,  27,  87,  34,  50,  32,  68,  26, -10,  83,  -2,  14,  89,   6,  27,  300 //a
//  40,  20,  98,  13,  35,  22,  63,  29,   5,  68, -11,  15,  98,  14,  32,  300 //b

//  48,  27,  88,  25,  34,  23,  52,  26,   3,  63, -14,  19,  34,   5,  33, 300 // a+
//  47,  27,  92,  31,  38,  28,  52,  29,   5,  61,  -6,  25,  92,   9,  33, 300 // b
//  50,  26,  84,  29,  46,  25,  35,  29,   0,  68, -14,  17,  99,   3,  40, 300 // b-
//  37,  30,  95,  34,  32,  26,  44,  33,  11,  56, -11,  22,  37,  12,  35, 300 // a
//  44,  32,  96,  28,  42,  24,  49,  25,  -6,  58,  17,  20,  51,  10,  32, 300 // s
//  48,  27,  97,  22,  41,  29,  53,  27,   3,  60, -10,  19,  42,   6,  31, 300 // a+

//new param

//  41,  13,  99,  28,  42,  32,  47,  24,   6,  61,  19,  30,  41,  12,  27, 300 // b
//  41,  25,  91,  28,  41,  26,  54,  31,   7,  43,  16,  35,   8,  12,  24, 300 // b
//  35,   5,  98,  26,  43,  32,  52,  18,  16,  57,  24,  22,  38,  10,  28, 300 // b
//  41,  13,  98,  27,  45,  24,  51,  28,  13,  65,  27,  21,  39,  13,  39, 300 // s
//  39,  15,  99,  26,  41,  28,  50,  26,   8,  68,  23,  22,  36,  14,  28, 300 // a
//  36,  20,  99,  27,  41,  32,  28,  24,  11,  56,  26,  24,  43,  14,  27, 300 //s

//*
//  41,  13,  99,  25,  46,  27,  49,  27,   9,  73,  23,  26,  25,   9,  42,  300 // a
//  44,  13,  98,  31,  51,  30,  53,  27,  16,  56,  29,  27,  34,  16,  24,  300 // s
//  39,  13,  98,  29,  30,  34,  54,  28,  18,  58,  20,  28,  35,  11,  33,  300 // s-
//  36,  11,  90,  29,  31,  34,  52,  22,  20,  66,  26,  26,  38,  11,  30,  300 // a
//  34,  29,  97,  35,  24,  23,  55,  27,  14,  75,  19,  41,  32,   0,  46,  300 // a
//  39,  12,  98,  30,  32,  27,  55,  25,  15,  68,  22,  25,  30,   9,  33,  300 // s-
//*/
//  46,  10,  99,  86,  80,  29,  30,  24, -22,  71,  50,  36,  47,  37,  44,  83 // a
//  74,  39,  88,  99,  57,  33,  30,  23, -14,  73,  60,  43,  58,  39,  38,  96 // a
//  46,  38,  96,  98,  63,  37,  28,  26, -17,  69,  64,  43,  45,  42,  42,  99 // s
//  68,  34,  99,  99,  75,  38,  33,  20, -16,  80,  56,  31,  61,  36,  43,  88
//  42,  48,  99,  95,  63,  27,  -2,  29, -22,  60,  54,  31,  57,  38,  37,  99 // b
//  70,  30,  96,  99,  73,  41,  41,  18, -20,  82,  51,  35,  55,  50,  41,  89 // a
//  36,  30,  71,  67,  72,  48,  22,  16,  34,  60,   0,  34,  46,  35,  16,  33 //test
//  31,  43,  78,  80,  63,  46,  48,  14,   0,  99,  59,  24,  29,  30,  33,   7 //test2
//  77,  42,  92,  98,  98,  28,   1,  19,  -1,  75,  52,  50,  99,  27,  49,  65 //test3
//  70,  16,  99,  50,  95,  33,  21,  29,  38,  98,  10,  54,  91,  26,  42,  65
  13,   9,  17,  10,  29,  25,  39,   2,  12,  19,   7,  24,  21,  16,  14,  19,  200

        };
        tetris[0].m_ai_param = param;
        //ai_level_p1 = 4; ai_level_p2 = 4;
    }
    for ( int i = 0; i < players_num; ++i) {
        if ( ai[i].level <= 0 ) {
            AI::AI_Param param[2] = {
//  82,  69,  99,  61,  81,  36,  35,  22,  11,  71,  21,   6,  51,  90,  37, -999
//  21,  20,  66,  40,  27,  22,  48,  26,   8,  71,  13,  24,  92,   7,  69, 999
//  49,  18,  76,  33,  39,  27,  32,  25,  22,  99,  41,  29,  96,  14,  60, 999
    {

//  43,  47,  84,  62,  60,  47,  53,  21,   2,  98,  85,  13,  21,  37,  38,  0
//  47,  66,  86,  66, -79,  42,  38,  23,  -3,  95,  74,  13,  27,  36,  37,  0
//  45,  61,  99,  49,  63,  40,  42,  31, -27,  88,  80,  28,  28,  41,  33,  0
//  58,  61,  90,  82,  19,  27,  44,  17,  -4,  75,  47,  20,  38,  32,  41,  0
  47,  62,  94,  90,  11,  35,  48,  19, -21,  78,  64,  20,  42,  42,  39,  300
//  48,  65,  84,  59, -75,  39,  43,  23, -17,  92,  64,  20,  29,  37,  36,  0

    },
    {

//  43,  47,  84,  62,  60,  47,  53,  21,   2,  98,  85,  13,  21,  37,  38,  0
//  47,  66,  86,  66, -79,  42,  38,  23,  -3,  95,  74,  13,  27,  36,  37,  0
//  45,  61,  99,  49,  63,  40,  42,  31, -27,  88,  80,  28,  28,  41,  33,  0 // a
//  58,  61,  90,  82,  19,  27,  44,  17,  -4,  75,  47,  20,  38,  32,  41,  0 // b
//  47,  62,  94,  90,  11,  35,  48,  19, -21,  78,  64,  20,  42,  42,  39,  0 // s
//  48,  65,  84,  59, -75,  39,  43,  23, -17,  92,  64,  20,  29,  37,  36,  0 // s
  71,  12,  78,  52,  96,  37,  14,  24,  40,  99,  44,  49,  93,  25,  44,  380
    }
};
            tetris[i].m_ai_param = param[i];
        }
    }
    std::string ai_name[2] = {"T-spin AI", "T-spin AI"};
    for ( int i = 0; i < players_num; ++i) {
        if ( ai[i].style == 1 ) {
            ai_name[i] = "T-spin+ AI";
        } else if ( ai[i].style == 2 ) {
            AI::setAIsettings(i, "hash", 0);
            //
        } else if ( ai[i].style == 3 ) {
            tetris[i].m_ai_param.tspin = tetris[i].m_ai_param.tspin3 = -300;
            tetris[i].m_ai_param.clear_useless_factor *= 0.8;
            ai_name[i] = "Ren AI";
        } else if ( ai[i].style == 4 ) {
            tetris[i].hold = false;
            tetris[i].m_ai_param.clear_useless_factor *= 0.7;
            ai_name[i] = "non-Hold";
            // no 4w
            tetris[i].m_ai_param.strategy_4w = 0;
            AI::setAIsettings(i, "4w", 0);
        } else if ( ai[i].style != -1 ) { //if ( ai[i].style == 5 ) {
            AI::AI_Param param[2] = {
                {49, 918, 176,  33,-300, -0,   0,  25,  22,  99,  41,-300,   0,  14, 290,   0}, // defence AI
                {21, 920,  66,  40,-300, -2,   0,  26,   8,  71,  13,-300,   0,   7, 269,   0},
            };
            AI::setAIsettings(i, "combo", 0);
            tetris[i].m_ai_param = param[i];
            ai_name[i] = "Downstack";
        }
    }
    for ( int i = 0; i < players_num; ++i) {
        if ( ai[i].style || i > 0 )
        {
            char name[200];
            sprintf( name, "%s LV%d", ai_name[i].c_str(), ai[i].level);
            tetris[i].m_name = name;
        }
        if ( tetris[i].pAIName ) {
            tetris[i].m_name = tetris[i].pAIName(ai[i].level);
        }
        if ( ! ai_4w || ai_eve || ai[i].level < 6 ) {
            tetris[i].m_ai_param.strategy_4w = 0;
        }
        if ( tetris[i].m_ai_param.strategy_4w > 0 ) {
            AI::setAIsettings(i, "4w", 1);
        }
    }
    if ( rule.combo_table_style == 0 )
    {
        int a[] = {0,0,0,1,1,2};
        AI::setComboList( std::vector<int>(a, a + sizeof(a)/sizeof(*a)) );
        for ( int i = 0; i < players_num; ++i) {
            tetris[i].m_ai_param.strategy_4w = 0;
            AI::setAIsettings(i, "4w", 0);
        }
    }
    else if ( rule.combo_table_style == 1 )
    {
        int a[] = {0,0,0,1,1,2,2,3,3,4,4,4,5};
        AI::setComboList( std::vector<int>(a, a + sizeof(a)/sizeof(*a)) );
    }
    else if ( rule.combo_table_style == 2 )
    {
        int a[] = {0,0,0,1,1,1,2,2,3,3,4,4,4,5};
        AI::setComboList( std::vector<int>(a, a + sizeof(a)/sizeof(*a)) );
    }
    else
    {
        int a[] = {0,0,0,1,1,2};
        AI::setComboList( std::vector<int>(a, a + sizeof(a)/sizeof(*a)) );
    }
    if ( rule.GarbageBlocking == 0 || rule.GarbageBuffer == 0 || rule.GarbageCancel == 0 ) {
        for ( int i = 0; i < players_num; ++i) {
            tetris[i].m_ai_param.strategy_4w = 0;
            AI::setAIsettings(i, "4w", 0);
        }
    }
#ifdef XP_RELEASE
    tetris[0].m_state = AI::Tetris::STATE_OVER;
    tetris[1].m_state = AI::Tetris::STATE_OVER;
#endif
    if ( player.sound_p1 && player.sound_p2 ) {
        tetris[0].m_lr = 1;
        tetris[1].m_lr = 2;
    }
    GameSound::ins().setVolume( 0.5f );

    {
        int seed = (unsigned)time(0), pass = rnd.randint(1024);
        for ( int i = 0; i < players_num; ++i ) {
            tetris[i].m_base = AI::point(i * 400, 30);
            //tetris[i].reset( seed ^ ((!rule.samesequence) * i * 255), pass );
            //if ( ai_eve )
            //tetris[i].reset( (unsigned)time(0) + ::GetTickCount() * i );
            /*
            int m[20][12] = {
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
            {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
            {1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
            {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            };
            for ( int j = 0; j < 18; ++j) {
            int v = 0;
            for ( int k = 0; k < 12; ++k ) {
            if ( m[j][k] ) v |= 1 << k;
            }
            tetris[i].addrow(v);
            }
            tetris[i].m_next[0] = AI::getGem(2, 0);
            */
            /*
			int m[22][10] = {
				{0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
				{1, 1, 1, 1, 1, 0, 0, 1, 1, 0},
				{1, 1, 1, 1, 1, 0, 0, 0, 1, 0},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
			};
			for ( int j = 0; j < 22; ++j) {
				int v = 0;
				for ( int k = 0; k < 10; ++k ) {
					if ( m[j][k] ) v |= 1 << k;
				}
				tetris[i].addRow(v);
			}
			tetris[i].m_next[0] = AI::getGem(AI::GEMTYPE_L, 0);
			tetris[i].m_next[1] = AI::getGem(AI::GEMTYPE_Z, 0);
			tetris[i].m_next[2] = AI::getGem(AI::GEMTYPE_T, 0);
			tetris[i].m_next[3] = AI::getGem(AI::GEMTYPE_S, 0);
			tetris[i].m_next[4] = AI::getGem(AI::GEMTYPE_I, 0);
            // */
            //onGameStart( tetris[i], rnd, i );
            //tetris[i].acceptAttack(player_begin_attack);
        }
    }
    double ai_time = 0;
    int lastGameState = -1;
    for ( ; is_run() ; normal_delay ? delay_fps(60) : delay_ms(0) ) {
        for ( int jf = 0; jf < mainloop_times; ++jf) {
#ifndef XP_RELEASE
            if ( AI_TRAINING_SLOW == 0 ) {
                for ( int i = 0; i < players_num; ++i ) {
                    while ( tetris[i].ai_movs_flag != -1 ) {
                        ::Sleep(1);
                    }
                }
            }
            if ( ai_eve ) {
                eve.game(tetris[0], tetris[1], rnd);
            }
#endif
            if ( tetris[0].alive() && tetris[1].alive() ) {
                lastGameState = 0;
            } else {
                if ( lastGameState == 0 ) {
                    if ( tetris[1].alive() ) {
                        tetris[0].ko();
                        tetris[1].n_win++;
                    } else {
                        tetris[1].ko();
                        tetris[0].n_win++;
                    }
                    //GameSound::ins().stopBGM();
                    if ( player.sound_bgm ) GameSound::ins().loadBGM_wait( rnd );
                }
                lastGameState = -1;
            }
            if ( ! ai_eve ) {
                while ( kbmsg() ) {
                    key_msg k = getkey();
                    if ( k.msg == key_msg_down && k.key == key_pause ) {
                        game_pause = ! game_pause;
                    }
                    if ( game_pause ) {
                        continue;
                    }
                    if ( player_stratagy_mode && rule.turnbase
                        && (PLAYER_WAIT || rule.turn > 1)
                        && k.msg == key_msg_down
                        && (tetris[0].n_pieces - 1) / rule.turn > (tetris[1].n_pieces - 1) / rule.turn )
                    {
                        bool match = false;
                        for ( int i = 0; i < 8; ++i ) {
                            if ( k.key == player_keys[i] ) match = true;
                        }
                        player_key_state[0] = 0;
                        player_key_state[1] = 0;
                        player_key_state[2] = 0;
                        if ( match ) continue;
                    }
                    if ( k.msg == key_msg_up ) {
                        for ( int i = 0; i < 8; ++i ) {
                            if ( player_key_state[i] && (k.key == player_keys[i] ) ) {
                                player_key_state[i] = 0;
                            }
                        }
                    }
                    if ( k.msg != key_msg_down ) continue;
                    //if ( k.key != player_last_key ) {
                    //    player_key_state[0] = 0;
                    //    player_key_state[1] = 0;
                    //    player_key_state[2] = 0;
                    //    player_last_key = k.key;
                    //}
                    if ( k.key == player_keys[0] && player_key_state[0] == 0 ) {
                        tetris[0].tryXMove(-1);
                        player_key_state[0] = 1;
                        player_key_state[1] = 0;
                        //player_key_state[2] = 0;
                    }
                    if ( k.key == player_keys[1] && player_key_state[1] == 0 ) {
                        tetris[0].tryXMove( 1);
                        player_key_state[0] = 0;
                        player_key_state[1] = 1;
                        //player_key_state[2] = 0;
                    }
                    if ( k.key == player_keys[2] && player_key_state[2] == 0 ) {
                        tetris[0].tryYMove( 1);
                        //while ( tetris[0].tryYMove( 1) );
                        //player_key_state[0] = 0;
                        //player_key_state[1] = 0;
                        player_key_state[2] = 1;
                    }
                    if ( k.key == player_keys[3] && player_key_state[3] == 0 ) {
                        tetris[0].trySpin( 1);
                        player_key_state[3] = 1;
                    }
                    if ( k.key == player_keys[4] && player_key_state[4] == 0 ) {
                        tetris[0].trySpin( 3);
                        player_key_state[4] = 1;
                    }
                    if ( k.key == player_keys[5] && player_key_state[5] == 0 ) {
                        tetris[0].tryHold();
                        player_key_state[5] = 1;
                    }
                    if ( k.key == player_keys[6] && player_key_state[6] == 0 ) {
                        tetris[0].drop();
                        //player_key_state[0] = !!player_key_state[0];
                        //player_key_state[1] = !!player_key_state[1];
                        //player_key_state[2] = !!player_key_state[2];
                        player_key_state[6] = 1;
                    }
                    if ( k.key == player_keys[7] && player_key_state[7] == 0 && AI::spin180Enable() ) {
                        tetris[0].trySpin180();
                        player_key_state[7] = 1;
                    }
                    if ( k.key == key_f2 ) {
                        if ( !tetris[0].alive() || !tetris[1].alive() || tetris[0].n_pieces <= 20 ) {
                            int seed = (unsigned)time(0), pass = rnd.randint(1024);
                            for ( int i = 0; i < players_num; ++i ) {
                                tetris[i].reset( seed ^ ((!rule.samesequence) * i * 255), pass );
                                //tetris[i].reset( (unsigned)time(0) + ::GetTickCount() * i );
                                onGameStart( tetris[i], rnd, i );
                                tetris[i].acceptAttack(player_begin_attack);
                            }
                            if ( player.sound_bgm ) GameSound::ins().loadBGM( rnd );
                        }
                    }
                    if ( k.key == key_f12 ) {
                        setkeyScene( player_keys );
                        for ( int i = 0; i < 7; ++i)
                            player_key_state[i] = 0;
                    }
                    if ( k.key == key_f3 ) {
                        showAttackLine = !showAttackLine;
                    }
                    if ( k.key == key_f4 ) {
                        showGrid = !showGrid;
                    }
                    if ( k.key == key_f5 || k.key == key_f6 ) {
                        char str[64];
                        if ( k.key == key_f5 ) GameSound::ins().setVolumeAdd(-0.05f) ;
                        if ( k.key == key_f6 ) GameSound::ins().setVolumeAdd(+0.05f);
                        sprintf_s( str, 64, "%d %%", int(GameSound::ins().getVolume() * 100 + 0.5) );
                        game_info = "set volume = ";
                        game_info += str;
                        game_info_time = 240;
                    }
                    if ( PUBLIC_VERSION == 0 ) {
                        if ( k.key == key_f9 ) {
                            tetris[1].accept_atts.push_back(1);
                        }
                    }
                }
                if ( game_pause ) continue;
                for (int i = 0; i < 3; ++i) {
                    if ( player_key_state[i] > 0) {
                        ++player_key_state[i];
                        if ( i == 2 ) player_key_state[i] += 9;
                        if ( player.tojsoftdrop && i == 2 ) {
                            while ( player_key_state[i] > (player.softdropdas + 1) * 10 ) {
                                bool move = tetris[0].tryYMove( 1);
                                if ( ! move && player.softdropdelay <= 0) break;
                                player_key_state[i] -= player.softdropdelay;
                            }
                        } else if ( player_key_state[i] > player.das + 1 ) {
                            if ( i == 0 ) {
                                tetris[0].tryXXMove(-1);
                            } else if ( i == 1 ) {
                                tetris[0].tryXXMove( 1) ;
                            } else if ( i == 2 ) {
                                tetris[0].tryYYMove( 1) ;
                            }
                        }
                    }
                }
            }
            for ( int i = 0; i < players_num; ++i ) {
                if ( tetris[i].game() ) { // 游戏执行，如果丢下返回true
                    tetris[i].env_change = 1;
                    tetris[i].n_pieces += 1;

                    int att = tetris[i].m_attack;
                    int clearLines = tetris[i].m_clearLines;
#ifndef XP_RELEASE
                    if ( tetris[i].total_atts >= 200 && tetris[i].total_atts - tetris[1 - i].total_atts > 20 ) {
                        tetris[1 - i].m_state = AI::Tetris::STATE_OVER;
                    } else if ( tetris[i].total_atts >= 400 ) {
                        if ( (double)tetris[i].total_atts / tetris[i].total_clears > (double)tetris[1 - i].total_atts / tetris[1 - i].total_clears ) {
                            tetris[1 - i].m_state = AI::Tetris::STATE_OVER;
                        }
                    }
#endif
                    tetris[i].total_clears += clearLines;
                    tetris[i].m_clearLines = 0;
                    tetris[i].m_attack = 0;
                    tetris[i].clearSFX( );
                    if ( att > 0 ) { // 两行攻击
                        tetris[i].total_atts += att;
                        if ( rule.GarbageCancel ) {
                            while ( att > 0 && ! tetris[i].accept_atts.empty() ) {
                                int m = min( att, tetris[i].accept_atts[0]);
                                att -= m;
                                tetris[i].accept_atts[0] -= m;
                                if ( tetris[i].accept_atts[0] <= 0 ) {
                                    tetris[i].accept_atts.erase( tetris[i].accept_atts.begin() );
                                }
                            }
                        }
                        if ( att > 0 ) {
                            for ( int j = 0; j < players_num; ++j ) {
                                if ( i == j ) continue;
                                if ( rule.GarbageBuffer ) {
                                    tetris[j].accept_atts.push_back( att );
                                    tetris[i].total_sent += att;
                                    if ( rule.turnbase ) tetris[j].env_change = 2;
                                } else {
                                    if ( player_accept_attack )
                                        tetris[j].acceptAttack( att );
                                    if ( rule.turnbase ) tetris[j].env_change = 2;
                                }
                            }
                        }
                    }

                    if ( player_accept_attack && ( rule.GarbageBlocking == 0 || clearLines == 0) ) {
                        while ( ! tetris[i].accept_atts.empty() ) {
                            if ( rule.garbage == 0 ) {
                                tetris[i].acceptAttack( *tetris[i].accept_atts.begin() );
                            } else if ( rule.garbage == 1 ) {
                                for ( int n = *tetris[i].accept_atts.begin(); n > 0; n-= 2 ) {
                                    if ( n >= 2 ) tetris[i].acceptAttack( 2 );
                                    else tetris[i].acceptAttack( 1 );
                                }
                            } else { //if ( rule.garbage == 2 ) {
                                for ( int n = *tetris[i].accept_atts.begin(); n > 0; --n ) {
                                    tetris[i].acceptAttack( 1 );
                                }
                            }
                            tetris[i].accept_atts.erase( tetris[i].accept_atts.begin() );
                        }
                    }
                }
                if ( tetris[i].env_change && tetris[i].ai_movs_flag == -1) { // AI 计算
                    if ( (ai_eve || ai[i].style) && tetris[i].alive() ) {
                    //if ( i != 0 && tetris[i].alive() ) {
                        std::vector<AI::Gem> next;
                        for ( int j = 0; j < 32; ++j)
                            next.push_back(tetris[i].m_next[j]);
                        double beg = (double)::GetTickCount() / 1000;
                        int deep = ai_search_height_deep;
                        int upcomeAtt = 0;
                        for ( int j = 0; j < tetris[i].accept_atts.size(); ++j ) {
                            upcomeAtt += tetris[i].accept_atts[j];
                        }
                        int level = ai[i].level;
                        //if ( tetris[i].m_pool.row[6] ) {
                        //    deep = ai_search_height_deep;
                        //}
                        if ( i == 1 && rule.turn == 1 && rule.turnbase && level > 9 ) { // 防2P被超越太多
                            if ( tetris[0].n_pieces * ai[1].PieceMul - tetris[i].n_pieces * ai[0].PieceMul > 2 ) {
                                level = 9;
                            }
                        }
                        bool canhold = tetris[i].hold;

                        if ( tetris[i].pTetrisAI ) {
                            AI::RunAIDll(tetris[i].pTetrisAI, tetris[i].ai_movs, tetris[i].ai_movs_flag, tetris[i].m_ai_param, tetris[i].m_pool, tetris[i].m_hold,
                                tetris[i].m_cur,
                                tetris[i].m_cur_x, tetris[i].m_cur_y, next, canhold, upcomeAtt,
                                deep, tetris[i].ai_last_deep, level, i);
                        } else {
                            AI::RunAI(tetris[i].ai_movs, tetris[i].ai_movs_flag, tetris[i].m_ai_param, tetris[i].m_pool, tetris[i].m_hold,
                                tetris[i].m_cur,
                                tetris[i].m_cur_x, tetris[i].m_cur_y, next, canhold, upcomeAtt,
                                deep, tetris[i].ai_last_deep, level, i);
#if 1 && !defined(XP_RELEASE)
                            while ( tetris[i].ai_movs_flag != -1 ) ::Sleep(1);
#endif
                        }
                        ai_time = (double)::GetTickCount() / 1000 - beg;
                        if ( rule.turnbase && ai[0].style == 0 ) {
                            if ( rule.turn == 1 && tetris[0].n_pieces * ai[1].PieceMul - tetris[i].n_pieces * ai[0].PieceMul > 1 ) {
                                ai_mov_time_base = ai_mov_time / 4;
                            } else {
                                ai_mov_time_base = ai_mov_time;
                            }
                            tetris[i].ai_delay = ai_mov_time_base + ai_mov_time_base / 3;
                        } else {
                            tetris[i].ai_delay = ai_first_delay;
                        }
                        if ( tetris[i].env_change == 2 ) { // 被攻击就按已等待时间减少思索等待
                            tetris[i].ai_delay = max(0, tetris[i].ai_delay - tetris[i].m_piecedelay);
                        }
                    }
                    tetris[i].env_change = 0;
                }
            }
            for ( int i = 0; i < players_num; ++i ) { // 100 garbage buffer get lost
                if ( ! tetris[i].alive() ) continue;
                int total = 0;
                for ( int j = 0; j < tetris[i].accept_atts.size(); ++j ) {
                    total += tetris[i].accept_atts[j];
                }
                if ( total >= 100 * 1 ) { // TODO
                    tetris[i].m_state = AI::Tetris::STATE_OVER;
                }
            }
            for ( int i = 0; i < players_num; ++i ) {
                if ( tetris[i].ai_delay > 0 ) --tetris[i].ai_delay;
                if ( player_stratagy_mode ) {
                    if ( i != 0 && tetris[0].n_pieces == 1 ) continue;
                    if ( rule.turnbase ) {
                        if ( i == 0 && (tetris[0].n_pieces - 1) * ai[1].PieceMul / rule.turn >  (tetris[1].n_pieces - 1) * ai[0].PieceMul / rule.turn ) continue;
                        if ( i != 0 && (tetris[1].n_pieces - 1) * ai[0].PieceMul / rule.turn >= (tetris[0].n_pieces - 1) * ai[1].PieceMul / rule.turn ) continue;
                    } else {
                        if ( i == 0 && !tetris[1].alive() ) continue;
                        if ( i != 0 && !tetris[0].alive() ) continue;
                    }
                }
                //if ( tetris[i].mov_llrr ) {
                //    if (0) ;
                //    else if (tetris[i].mov_llrr == AI::Moving::MOV_LL) {if ( ! tetris[i].tryXMove(-1) ) tetris[i].mov_llrr = 0;}
                //    else if (tetris[i].mov_llrr == AI::Moving::MOV_RR) {if ( ! tetris[i].tryXMove( 1) ) tetris[i].mov_llrr = 0;}
                //} else
                {
                    do
                    {
                        if ( tetris[i].ai_delay > 0 ) ;
                        else if ( tetris[i].ai_movs_flag == -1 && ! tetris[i].ai_movs.movs.empty() ){
                            if ( rule.turnbase && ai[0].style == 0 ) {
                                tetris[i].ai_delay = ai_mov_time_base;
                            } else {
                                tetris[i].ai_delay = ai_move_delay;
                            }
                            int mov = tetris[i].ai_movs.movs[0];
                            if ( tetris[i].ai_movs.movs.size() > 1 ) {
                                int next_mov = tetris[i].ai_movs.movs[1];
                                if ( mov != next_mov ) tetris[i].ai_delay = tetris[i].ai_delay * 8 / 12;
                            }
                            tetris[i].ai_movs.movs.erase( tetris[i].ai_movs.movs.begin() );
                            if (0) ;
                            else if (mov == AI::Moving::MOV_L) tetris[i].tryXMove(-1);
                            else if (mov == AI::Moving::MOV_R) tetris[i].tryXMove( 1);
                            else if (mov == AI::Moving::MOV_D) tetris[i].tryYMove( 1);
                            else if (mov == AI::Moving::MOV_LSPIN) tetris[i].trySpin(1);
                            else if (mov == AI::Moving::MOV_RSPIN) tetris[i].trySpin(3);
                            else if (mov == AI::Moving::MOV_LL) { tetris[i].tryXXMove(-1); } //{ tetris[i].mov_llrr = AI::Moving::MOV_LL; }
                            else if (mov == AI::Moving::MOV_RR) { tetris[i].tryXXMove( 1); } //{ tetris[i].mov_llrr = AI::Moving::MOV_RR; }
                            else if (mov == AI::Moving::MOV_DD) tetris[i].tryYYMove( 1) ;
                            else if (mov == AI::Moving::MOV_DROP) tetris[i].drop();
                            else if (mov == AI::Moving::MOV_HOLD) {
                                tetris[i].tryHold();
                            } else if (mov == AI::Moving::MOV_SPIN2) {
                                if ( AI::spin180Enable() ) {
                                    tetris[i].trySpin180();
                                }
                            } else if (mov == AI::Moving::MOV_REFRESH) {
                                tetris[i].env_change = 1;
                            }
                        }
                    } while ( tetris[i].ai_movs_flag == -1 && tetris[i].ai_delay == 0 && !tetris[i].ai_movs.movs.empty() );
                }
            }
            if ( ! ai_eve ) break;
        }
        cleardevice();
        for (std::vector<TetrisGame>::iterator it = tetris.begin();
            it != tetris.end();
            ++it) {
                tetris_draw(*it, showAttackLine, showGrid);
        }
        //if(0)
        if ( ! PUBLIC_VERSION )
        {
            for ( int i = 0; i < players_num; ++i ) {
                setcolor(EGERGB(0x0, 0xa0, 0x0));
                //xyprintf(0,0, "%.3f", ai_time * 1000);
                xyprintf((int)tetris[i].m_base.x, (int)(tetris[i].m_base.y - tetris[i].m_size.y), "d = %d", tetris[i].ai_last_deep);
            }
        }
        if ( game_info_time > 0 ) {
            --game_info_time;
            xyprintf( 0, getheight() - textheight("I"), "%s", game_info.c_str());
        }
#ifndef XP_RELEASE
        if ( ai_eve ) {
            double d = 0;
            if ( eve.m_p2 > 0 ) {
                d = eve.m_p2_score / (double)eve.m_p2;
                d *= d;
                d = eve.m_p2_sqr_score / (double)eve.m_p2 - d;
                d = eve.m_p2_score / (double)eve.m_p2 - sqrt(d);
            }
            if ( (eve.m_p1 + eve.m_p2) % 2 == 0)
                xyprintf(0, 0, "(%d) %d : %d", eve.ai.size(), eve.m_p2, eve.m_p1);
            else
                xyprintf(0, 0, "(%d) %d : %d", eve.ai.size(), eve.m_p1, eve.m_p2);
            //xyprintf(0, 0, "%.2f %d %d", d * eve.round, eve.m_p2, eve.m_p2_score + tetris[1].total_atts);
        }
#endif
        {
            char buff[16];
            sprintf(buff, "%.2ffps", getfps());
            outtextxy(getwidth() - textwidth(buff), 0, buff);
        }
    }
    saveKeySetting( player_keys );
}

void CenterWindow(HWND hWnd)
{
    HWND hParentOrOwner;
    RECT rc, rc2;
    int  x,y;
    if ( (hParentOrOwner = GetParent(hWnd)) == NULL )
    {
        SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
    }
    else
    {
        GetClientRect(hParentOrOwner, &rc);
    }
    GetWindowRect(hWnd, &rc2);
    x = ((rc.right-rc.left) - (rc2.right-rc2.left)) / 2 +rc.left;
    y = ((rc.bottom-rc.top) - (rc2.bottom-rc2.top)) / 2 +rc.top;
    SetWindowPos(hWnd,HWND_TOP,x, y,0, 0,SWP_NOSIZE);
}

int main () {
#if PUBLIC_VERSION == 0
    setinitmode(INIT_ANIMATION & ~INIT_WITHLOGO);
#else
    setinitmode(INIT_ANIMATION);
#endif
    initgraph(800, 500);
    //CenterWindow( getHWnd() );
#if PUBLIC_VERSION == 0
    setcaption("Tetris AI Demo");
#else
    setcaption("MisaMino V1.4.5 ---- by Misakamm ( misakamm.com )");
#endif
    mainscene();
    return 0;
}
