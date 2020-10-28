#include "sdlplayer.h"
#include <QTime>
using namespace std;

int SDLPlayer::window_x = 0;

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
    audio_mutex_ = SDL_CreateMutex();
    video_mutex_ = SDL_CreateMutex();
    connect(this, SIGNAL(play_video_event(unsigned int, unsigned int)), SLOT(OnPlayVideoEvent(unsigned int, unsigned int)), Qt::QueuedConnection);
}

SDLPlayer::~SDLPlayer()
{
    unInit();
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

    SDL_DestroyMutex(audio_mutex_);
    SDL_DestroyMutex(video_mutex_);

    SDL_Quit();
    flag = false;
}

void SDLPlayer::outputVideo(SFgVideoFrame *data)
{
    if (data->width == 0 || data->height == 0) {
        return;
    }

    if (data->width != m_rect.w || data->height != m_rect.h)
    {
        if(!flag)
        {
            flag = true;
            emit play_video_event(data->width, data->height);
        }
        return;
    }

    SDL_LockMutex(video_mutex_);
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

    SDL_UnlockMutex(video_mutex_);

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

    SDL_LockMutex(audio_mutex_);
    m_queueAudio.push(dataClone);
    SDL_UnlockMutex(audio_mutex_);
}

void SDLPlayer::initVideo(int width, int height)
{
    if(sdl_window_)
        return;

    QString window_name = QString::fromStdWString(device_name_);
    sdl_window_ = SDL_CreateWindow(window_name.toStdString().c_str(),
                                   window_x, 100,
                                   width, height,
                                   SDL_WINDOW_OPENGL);
    if(!sdl_window_)
    {
        printf("SDL: could not create window - exiting:%s\n",SDL_GetError());
        return;

    }
    //window_x += width;
    sdl_render_ = SDL_CreateRenderer(sdl_window_, -1, 0);


    SDL_LockMutex(video_mutex_);
    sdl_texture_ = SDL_CreateTexture(sdl_render_, SDL_PIXELFORMAT_IYUV,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     width, height);
    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = width;
    m_rect.h = height;
    SDL_UnlockMutex(video_mutex_);
    flag = false;
}

void SDLPlayer::unInitVideo()
{
    SDL_LockMutex(video_mutex_);
    if (NULL != sdl_window_) {
        SDL_DestroyWindow(sdl_window_);
        sdl_window_ = NULL;
    }

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
    SDL_UnlockMutex(video_mutex_);
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


    SDL_LockMutex(audio_mutex_);
    while (!m_queueAudio.empty())
    {
        SDemoAudioFrame* pAudioFrame = m_queueAudio.front();
        delete[] pAudioFrame->data;
        delete pAudioFrame;
        m_queueAudio.pop();
    }
    SDL_UnlockMutex(audio_mutex_);

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

    SDL_LockMutex(pThis->audio_mutex_);
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
    SDL_UnlockMutex(pThis->audio_mutex_);
}

void SDLPlayer::setDeviceName(wstring name)
{
    device_name_ = name;
}

void SDLPlayer::OnPlayVideoEvent(unsigned int width, unsigned int height)
{
    if (width != m_rect.w || height != m_rect.h || sdl_window_ == NULL || sdl_texture_ == NULL || sdl_render_ == NULL) {
        unInitVideo();
        initVideo(width, height);
    }
}
