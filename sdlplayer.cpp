#include "sdlplayer.h"
#include <QTime>
using namespace std;

SDLPlayer::SDLPlayer(QObject *parent) : QObject(parent)
  , sdl_window_(NULL)
  , sdl_render_(NULL)
  , sdl_texture_(NULL)
  , m_bAudioInited(false)
  , m_bDumpAudio(false)
  , m_fileWav(NULL)
  , m_sAudioFmt()
  , m_rect()
  , m_fRatio(1.0f)
{
    ZeroMemory(&m_sAudioFmt, sizeof(SFgAudioFrame));
    ZeroMemory(&m_rect, sizeof(SDL_Rect));
    m_mutexAudio = CreateMutex(NULL, FALSE, NULL);
    m_mutexVideo = CreateMutex(NULL, FALSE, NULL);

    connect(this, SIGNAL(play_video_event(unsigned int, unsigned int)), SLOT(OnPlayVideoEvent(unsigned int, unsigned int)), Qt::QueuedConnection);
}

SDLPlayer::~SDLPlayer()
{
    unInit();

    CloseHandle(m_mutexAudio);
    CloseHandle(m_mutexVideo);
}

bool SDLPlayer::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return false;
    }

    /* Clean up on exit, exit on window close and interrupt */
    atexit(SDL_Quit);

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);

    return true;
}

void SDLPlayer::unInit()
{
    unInitVideo();
    unInitAudio();

    SDL_Quit();
}

void SDLPlayer::outputVideo(SFgVideoFrame *data)
{
    if (data->width == 0 || data->height == 0) {
        return;
    }

    if (data->width != m_rect.w || data->height != m_rect.h) {
        {
//            //AutoLock oLock(m_mutexVideo, "unInitVideo");
//            if (NULL != m_yuv) {
//                SDL_Tex(m_yuv);
//                m_yuv = NULL;
//            }
        }
        if(!flag)
        {
            emit play_video_event(data->width, data->height);
            flag = true;
        }
        return;
    }

    //AutoLock oLock(m_mutexVideo, "outputVideo");
    if (sdl_texture_ == NULL) {
        return;
    }


    SDL_UpdateYUVTexture(sdl_texture_, &m_rect,
                    data->data, data->pitch[0],
                    data->data + data->dataLen[0], data->pitch[1],
                    data->data + data->dataLen[0] + data->dataLen[1] , data->pitch[2]);


    SDL_RenderClear(sdl_render_);
    SDL_RenderCopy( sdl_render_, sdl_texture_,  NULL, &m_rect);
    SDL_RenderPresent(sdl_render_);

    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = data->width;
    m_rect.h = data->height;

//    free(y_data);
//    free(u_data);
//    free(v_data);
}

void SDLPlayer::outputAudio(SFgAudioFrame *data)
{
    if (data->channels == 0) {
        return;
    }

    initAudio(data);

    if (m_bDumpAudio) {
        if (m_fileWav != NULL) {
            fwrite(data->data, data->dataLen, 1, m_fileWav);
        }
    }

    SDemoAudioFrame* dataClone = new SDemoAudioFrame();
    dataClone->dataTotal = data->dataLen;
    dataClone->pts = data->pts;
    dataClone->dataLeft = dataClone->dataTotal;
    dataClone->data = new uint8_t[dataClone->dataTotal];
    memcpy(dataClone->data, data->data, dataClone->dataTotal);

    {
//        AutoLock oLock(m_mutexAudio, "outputAudio");
        m_queueAudio.push(dataClone);
    }
}

void SDLPlayer::initVideo(int width, int height)
{
    if(sdl_window_)
        return;

    QString window_name = "AirPlay Demo" + QTime::currentTime().toString();
    sdl_window_ = SDL_CreateWindow(window_name.toStdString().c_str(),
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   width, height,
                                   SDL_WINDOW_OPENGL);
    if(!sdl_window_)
    {
        printf("SDL: could not create window - exiting:%s\n",SDL_GetError());
        return;

    }

    sdl_render_ = SDL_CreateRenderer(sdl_window_, -1, 0);


    //        AutoLock oLock(m_mutexVideo, "initVideo");
    sdl_texture_ = SDL_CreateTexture(sdl_render_, SDL_PIXELFORMAT_IYUV,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     width, height);
    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = width;
    m_rect.h = height;
}

void SDLPlayer::unInitVideo()
{
    if (NULL != sdl_window_) {
        SDL_DestroyWindow(sdl_window_);
        sdl_window_ = NULL;
    }

    //        AutoLock oLock(m_mutexVideo, "unInitVideo");
    if (NULL != sdl_texture_)
    {
        SDL_DestroyTexture(sdl_texture_);
        sdl_texture_ = NULL;
    }

    if (NULL != sdl_render_)
    {
        SDL_DestroyRenderer(sdl_render_);
        sdl_render_ = NULL;
    }

    m_rect.w = 0;
    m_rect.h = 0;

    unInitAudio();
}

void SDLPlayer::initAudio(SFgAudioFrame *data)
{
    if ((data->sampleRate != m_sAudioFmt.sampleRate || data->channels != m_sAudioFmt.channels)) {
        unInitAudio();
    }
    if (!m_bAudioInited) {
        SDL_AudioSpec wanted_spec, obtained_spec;
        wanted_spec.freq = data->sampleRate;
        wanted_spec.format = AUDIO_S16SYS;
        wanted_spec.channels = data->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = 1920;
        wanted_spec.callback = sdlAudioCallback;
        wanted_spec.userdata = this;

        if (SDL_OpenAudio(&wanted_spec, &obtained_spec) < 0)
        {
            printf("can't open audio.\n");
            return;
        }

        SDL_PauseAudio(1);

        m_sAudioFmt.bitsPerSample = data->bitsPerSample;
        m_sAudioFmt.channels = data->channels;
        m_sAudioFmt.sampleRate = data->sampleRate;
        m_bAudioInited = true;

        if (m_bDumpAudio) {
            m_fileWav = fopen("demo-audio.wav", "wb");
        }
    }
    if (m_queueAudio.size() > 5) {
        SDL_PauseAudio(0);
    }
}

void SDLPlayer::unInitAudio()
{
    SDL_CloseAudio();
    m_bAudioInited = false;
    memset(&m_sAudioFmt, 0, sizeof(m_sAudioFmt));

    {
//        AutoLock oLock(m_mutexAudio, "unInitAudio");
        while (!m_queueAudio.empty())
        {
            SDemoAudioFrame* pAudioFrame = m_queueAudio.front();
            delete[] pAudioFrame->data;
            delete pAudioFrame;
            m_queueAudio.pop();
        }
    }

    if (m_fileWav != NULL) {
        fclose(m_fileWav);
        m_fileWav = NULL;
    }
}

void SDLPlayer::sdlAudioCallback(void *userdata, Uint8 *stream, int len)
{
    SDLPlayer* pThis = (SDLPlayer*)userdata;
    int needLen = len;
    int streamPos = 0;
    memset(stream, 0, len);

//    AutoLock oLock(pThis->m_mutexAudio, "sdlAudioCallback");
    while (!pThis->m_queueAudio.empty())
    {
        SDemoAudioFrame* pAudioFrame = pThis->m_queueAudio.front();
        int pos = pAudioFrame->dataTotal - pAudioFrame->dataLeft;
        int readLen = min(pAudioFrame->dataLeft, needLen);

        //SDL_MixAudio(stream + streamPos, pAudioFrame->data + pos, readLen, 100);
        memcpy(stream + streamPos, pAudioFrame->data + pos, readLen);

        pAudioFrame->dataLeft -= readLen;
        needLen -= readLen;
        streamPos += readLen;
        if (pAudioFrame->dataLeft <= 0) {
            pThis->m_queueAudio.pop();
            delete[] pAudioFrame->data;
            delete pAudioFrame;
        }
        if (needLen <= 0) {
            break;
        }
    }
}

void SDLPlayer::OnPlayVideoEvent(unsigned int width, unsigned int height)
{
    if (width != m_rect.w || height != m_rect.h || sdl_window_ == NULL || sdl_texture_ == NULL || sdl_render_ == NULL) {
        initVideo(width, height);
    }
}
