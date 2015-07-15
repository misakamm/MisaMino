#pragma once

#include "bass.h"
#pragma comment(lib, "bass.lib")
#include <deque>

class GameSound {
protected:
    GameSound() {
        mVolume = 0.5f;
        mBgmVolume = 0.3f;
        BASS_Init(-1,44100,0,0,NULL);
    }
    ~GameSound() {
    }
public:
    class sound {
    public:
        sound( ) {
            for ( int i = 0; i < 16; ++i)
                m[i] = NULL;
            mOpen = false;
            mIndex = 0;
            mMax = 1;
        }
        ~sound() {
            for ( int i = 0; i < 16; ++i) {
                if ( m[i] ) {
                    BASS_StreamFree(m[i]);
                    m[i] = NULL;
                }
            }
            BASS_Free();
        }
        int open( float * volume, const char* path, int nMax = 2, int loop = 0 ) {
            mVolume = volume;
            if ( m[0] ) {
                for ( int i = 0; i < 16; ++i) {
                    if ( m[i] ) {
                        BASS_StreamFree(m[i]);
                        m[i] = NULL;
                    }
                }
            }
            m[0] = BASS_StreamCreateFile(FALSE, path, 0, 0, loop?BASS_SAMPLE_LOOP:0);
            int ret = m[0];
            mMax = nMax;
            for ( int i = 1; i < mMax; ++i) {
                m[i] = BASS_StreamCreateFile(FALSE, path, 0, 0, loop?BASS_SAMPLE_LOOP:0);
            }
            mOpen = ret != 0;
            return ret;
        }
        bool isOpen() {
            return mOpen;
        }
        int play( int lr = 0) {
            mIndex = (mIndex+1) % mMax;
            //m[mIndex]->Stop();
            setVolume( *mVolume );
            if ( lr == 1 ) {
                BASS_ChannelSetAttribute(m[mIndex], BASS_ATTRIB_PAN, -0.7);
            } else if ( lr == 2 ){
                BASS_ChannelSetAttribute(m[mIndex], BASS_ATTRIB_PAN,  0.7);
            }
            return BASS_ChannelPlay(m[mIndex], TRUE);
        }
        int stop() {
            return BASS_ChannelStop(m[mIndex]);
        }
        void setVolume( float volume ) {
            if ( m[mIndex] ) {
                BASS_ChannelSetAttribute( m[mIndex], BASS_ATTRIB_VOL, volume );
            }
        }
    private:
        HSTREAM m[16];
        int mIndex;
        int mMax;
        bool mOpen;
        float * mVolume;
    };
    static GameSound& ins() {
        static GameSound gamesound;
        return gamesound;
    }
    void loadSFX () {
#if !defined(XP_RELEASE)
        return;
#endif
        std::string basePath = "sound/sfx/default/";
        mSFX_ko.open(&mVolume, (basePath + "sfx_ko.wav").c_str());
        mSFX_gameover.open(&mVolume, (basePath + "sfx_gameover.wav").c_str());
        mSFX_pc.open(&mVolume, (basePath + "sfx_perfectclear.wav").c_str());
        mSFX_move.open(&mVolume, (basePath + "sfx_move.wav").c_str(), 4);
        mSFX_hold.open(&mVolume, (basePath + "sfx_hold.wav").c_str());
        mSFX_rotate.open(&mVolume, (basePath + "sfx_rotate.wav").c_str());
        mSFX_softdrop.open(&mVolume, (basePath + "sfx_softdrop.wav").c_str(), 8);
        mSFX_harddrop.open(&mVolume, (basePath + "sfx_harddrop.wav").c_str());
        mSFX_lockdown.open(&mVolume, (basePath + "sfx_lockdown.wav").c_str());
        mSFX_lineattack.open(&mVolume, (basePath + "sfx_lineattack.wav").c_str());
        mSFX_clears[0].open(&mVolume, (basePath + "sfx_single.wav").c_str());
        mSFX_clears[1].open(&mVolume, (basePath + "sfx_double.wav").c_str());
        mSFX_clears[2].open(&mVolume, (basePath + "sfx_triple.wav").c_str());
        mSFX_clears[3].open(&mVolume, (basePath + "sfx_tetris.wav").c_str());
        mSFX_b2b_tetris.open(&mVolume, (basePath + "sfx_b2b_tetris.wav").c_str());
        for ( int i = 0; i < 20; ++i) {
            char name[16];
            sprintf( name, "sfx_combo%d.wav", i + 1);
            mSFX_combo[i].open(&mVolume, (basePath + name).c_str());
        }
        mSFX_tspin[0].open(&mVolume, (basePath + "sfx_tspin_mini.wav").c_str());
        mSFX_tspin[1].open(&mVolume, (basePath + "sfx_tspin_single.wav").c_str());
        mSFX_tspin[2].open(&mVolume, (basePath + "sfx_tspin_double.wav").c_str());
        mSFX_tspin[3].open(&mVolume, (basePath + "sfx_tspin_triple.wav").c_str());
        mSFX_b2b_tspin[0].open(&mVolume, (basePath + "sfx_b2b_tspin_mini.wav").c_str());
        mSFX_b2b_tspin[1].open(&mVolume, (basePath + "sfx_b2b_tspin_single.wav").c_str());
        mSFX_b2b_tspin[2].open(&mVolume, (basePath + "sfx_b2b_tspin_double.wav").c_str());
        mSFX_b2b_tspin[3].open(&mVolume, (basePath + "sfx_b2b_tspin_triple.wav").c_str());
    }
    void loadBGM_wait( AI::Random& rnd ) {
        std::string basePath = "sound/music/default/";
        int n = rnd.randint(2);
        if ( n == 0 ) {
            mbgm.open(&mBgmVolume, (basePath + "waiting1.ogg").c_str(), 1, 1);
        } else {
            mbgm.open(&mBgmVolume, (basePath + "waiting2.ogg").c_str(), 1, 1);
        }
        mbgm.play();
    }
    void loadBGM( AI::Random& rnd ) {
        std::string basePath = "sound/music/default/";
        int n = rnd.randint(3);
        if ( n == 0 ) {
            mbgm.open(&mBgmVolume, (basePath + "bgm_01.ogg").c_str(), 1, 1);
        } else if ( n == 1 ) {
            mbgm.open(&mBgmVolume, (basePath + "bgm_02.ogg").c_str(), 1, 1);
        } else if ( n == 2 ) {
            mbgm.open(&mBgmVolume, (basePath + "bgm_03.ogg").c_str(), 1, 1);
        }
        mbgm.play();
    }
    void stopBGM() {
        mbgm.stop();
    }
    void setVolume( float volume ) {
        mVolume = volume;
        if ( mVolume < 0.0 ) mVolume = 0;
        if ( mVolume > 1.0 ) mVolume = 1.0f;
        mBgmVolume = mVolume * 0.6f;
        mbgm.setVolume(mBgmVolume);
    }
    void setVolumeAdd( float add ) {
        mVolume += add;
        if ( mVolume < 0.0 ) mVolume = 0;
        if ( mVolume > 1.0 ) mVolume = 1.0f;
        mBgmVolume = mVolume * 0.6f;
        mbgm.setVolume(mBgmVolume);
    }
    float getVolume() const {
        return mVolume;
    }
public:
    sound mSFX_move;
    sound mSFX_hold;
    sound mSFX_rotate;
    sound mSFX_softdrop;
    sound mSFX_harddrop;
    sound mSFX_lockdown;
    sound mSFX_lineattack;
    sound mSFX_clears[4];
    sound mSFX_combo[20];
    sound mSFX_tspin[4];
    sound mSFX_b2b_tspin[4];
    sound mSFX_b2b_tetris;
    sound mSFX_ko;
    sound mSFX_gameover;
    sound mSFX_pc;
    sound mbgm;
    float mVolume;
    float mBgmVolume;
};
