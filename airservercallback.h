#ifndef AIRSERVERCALLBACK_H
#define AIRSERVERCALLBACK_H

#include "Airplay2Head.h"
#include <map>

class AirServerCallback : public IAirServerCallback
{
public:
    AirServerCallback();

    virtual void connected(const char *remoteName, const char *remoteDeviceId) override;
    virtual void disconnected(const char *remoteName, const char *remoteDeviceId) override;
    virtual void outputAudio(SFgAudioFrame *data, const char *remoteName, const char *remoteDeviceId) override;
    virtual void outputVideo(SFgVideoFrame *data, const char *remoteName, const char *remoteDeviceId) override;
    virtual void videoPlay(char *url, double volume, double startPos) override;
    virtual void videoGetPlayInfo(double *duration, double *position, double *rate) override;
    virtual void log(int level, const char *msg) override;
};

#endif // AIRSERVERCALLBACK_H
