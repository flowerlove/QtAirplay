#ifndef CASTSCREENMANAGER_H
#define CASTSCREENMANAGER_H

#include <QObject>
#include <QMap>
#include "mainwindow.h"
#include "sdlplayer.h"

struct DeviceInfo{
    SDLPlayer* player_;
    std::wstring device_name_;
    char m_chRemoteDeviceId[128];
};

class CastScreenManager : public QObject
{
    Q_OBJECT
public:

    static CastScreenManager *GetInstance();

    MainWindow *mainWindow() const;
    void setMainWindow(MainWindow *mainWindow);

    QMap<char*, DeviceInfo*> *deviceMap() const;
    void setDeviceMap(QMap<char*, DeviceInfo*> *device_map);

    void EmitCreatePlayer(char* id);
    void EmitDestroyPlayer(char* id);
private:
    explicit CastScreenManager(QObject *parent = nullptr);

signals:
    void create_player(char* id);
    void destroy_player(char* id);
public slots:
    void OnCreatePlayer(char* id);
    void OnDestroyPlayer(char* id);
private:
    static CastScreenManager *instance_;
    MainWindow* mainWindow_;
    QMap<char*, DeviceInfo*> *device_map_;
};

#endif // CASTSCREENMANAGER_H
