#include "airservercallback.h"
#include "castscreenmanager.h"

std::wstring UTF8_To_UTF16(const char* utf8Text)
{
    unsigned long len = ::MultiByteToWideChar(CP_UTF8, NULL, utf8Text, -1, NULL, NULL);
    if (len == 0)
        return std::wstring(L"");
    wchar_t *buffer = new wchar_t[len];
    ::MultiByteToWideChar(CP_UTF8, NULL, utf8Text, -1, buffer, len);

    std::wstring dest(buffer);
    delete[] buffer;

    return dest;
}

AirServerCallback::AirServerCallback()
{
}

void AirServerCallback::connected(const char *remoteName, const char *remoteDeviceId)
{
    CastScreenManager* manager = CastScreenManager::GetInstance();
    DeviceInfo* device = new DeviceInfo();
    if (remoteDeviceId != NULL) {
        strncpy(device->m_chRemoteDeviceId, remoteDeviceId, 128);
    }

    setlocale(LC_CTYPE, "");
    device->device_name_ = UTF8_To_UTF16(remoteName);
    manager->deviceMap()->insert((char*)remoteDeviceId, device);
    manager->EmitCreatePlayer((char*)remoteDeviceId);
}

void AirServerCallback::disconnected(const char *remoteName, const char *remoteDeviceId)
{
    CastScreenManager* manager = CastScreenManager::GetInstance();

    if(manager->deviceMap()->contains((char*)remoteDeviceId))
    {
        DeviceInfo* device = manager->deviceMap()->value((char*)remoteDeviceId);
        manager->EmitDestroyPlayer((char*)remoteDeviceId);
    }
}

void AirServerCallback::outputAudio(SFgAudioFrame *data, const char *remoteName, const char *remoteDeviceId)
{
    CastScreenManager* manager = CastScreenManager::GetInstance();

    if(manager->deviceMap()->contains((char*)remoteDeviceId))
    {
    }
}

void AirServerCallback::outputVideo(SFgVideoFrame *data, const char *remoteName, const char *remoteDeviceId)
{
    CastScreenManager* manager = CastScreenManager::GetInstance();

    if(manager->deviceMap()->contains((char*)remoteDeviceId))
    {
        DeviceInfo* device = manager->deviceMap()->value((char*)remoteDeviceId);
        device->player_->outputVideo(data);
    }
}

void AirServerCallback::videoPlay(char *url, double volume, double startPos)
{
    printf("Play: %s", url);
}

void AirServerCallback::videoGetPlayInfo(double *duration, double *position, double *rate)
{
    *duration = 1000;
    *rate = 1.0;
}

void AirServerCallback::log(int level, const char *msg)
{
    printf("%s\n", msg);
}
