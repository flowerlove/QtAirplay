#include "castscreenmanager.h"

CastScreenManager *CastScreenManager::instance_ = Q_NULLPTR;

CastScreenManager *CastScreenManager::GetInstance()
{
    if(!instance_)
    {
        instance_ = new CastScreenManager();
    }
    return instance_;
}

MainWindow *CastScreenManager::mainWindow() const
{
    return mainWindow_;
}

void CastScreenManager::setMainWindow(MainWindow *mainWindow)
{
    mainWindow_ = mainWindow;
}

QMap<char*, DeviceInfo *> *CastScreenManager::deviceMap() const
{
    return device_map_;
}

void CastScreenManager::setDeviceMap(QMap<char*, DeviceInfo *> *device_map)
{
    device_map_ = device_map;
}

void CastScreenManager::EmitCreatePlayer(char* id)
{
    emit create_player(id);
}

CastScreenManager::CastScreenManager(QObject *parent) : QObject(parent)
{
    device_map_ = new QMap<char*, DeviceInfo*>;
    QObject::connect(this, SIGNAL(create_player(char*)), SLOT(OnCreatePlayer(char*)));
}

void CastScreenManager::OnCreatePlayer(char* id)
{
    if(device_map_->contains(id))
    {
        DeviceInfo* device = device_map_->value(id);
        device->player_ = new SDLPlayer();
        device->player_->init();
    }
}
