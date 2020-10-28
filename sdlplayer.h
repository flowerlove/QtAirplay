#ifndef SDLPLAYER_H
#define SDLPLAYER_H

#pragma once
#include <Windows.h>
#include "Airplay2Head.h"
#include <QObject>
#include <queue>
#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#undef main
#include "airserver.h"

typedef void sdlAudioCallback(void* userdata, Uint8* stream, int len);

typedef struct SDemoAudioFrame {
    unsigned long long pts;
    unsigned int dataTotal;
    unsigned int dataLeft;
    unsigned char* data;
} SDemoAudioFrame;

typedef std::queue<SDemoAudioFrame*> SDemoAudioFrameQueue;
typedef std::queue<SFgVideoFrame*> SFgVideoFrameQueue;

#define VIDEO_SIZE_CHANGED_CODE 1

class SDLPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SDLPlayer(QObject *parent = nullptr);
    ~SDLPlayer();
    bool init();
    void unInit();

    void outputVideo(SFgVideoFrame* data);
    void outputAudio(SFgAudioFrame* data);

    void initVideo(int width, int height);
    void unInitVideo();
    void initAudio(SFgAudioFrame* data);
    void unInitAudio();
    static void sdlAudioCallback(void* userdata, Uint8* stream, int len);

    void setDeviceName(std::wstring name);
signals:
    void play_video_event(unsigned int width, unsigned int height);
public slots:
    void OnPlayVideoEvent(unsigned int width, unsigned int height);

public:
    SDL_Window* sdl_window_;
    SDL_Renderer* sdl_render_;
    SDL_Texture* sdl_texture_;
    SDL_Rect m_rect;

    SFgAudioFrame m_sAudioFmt;
    bool m_bAudioInited;
    SDemoAudioFrameQueue m_queueAudio;
    SDL_mutex* audio_mutex_;
    SDL_mutex* video_mutex_;

    SDL_Event m_evtVideoSizeChange;

    bool m_bDumpAudio;
    FILE* m_fileWav;
    float m_fRatio;
    bool flag = false;
    static int window_x;
    std::wstring device_name_;
};

#endif // SDLPLAYER_H
