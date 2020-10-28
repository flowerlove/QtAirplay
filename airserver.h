#ifndef AIRSERVER_H
#define AIRSERVER_H

#include <QObject>
#include "airservercallback.h"

class AirServer : public QObject
{
    Q_OBJECT
public:
    explicit AirServer(QObject *parent = nullptr);
    ~AirServer();

    void Start();
    void Stop();
    float SetVideoScale(float ratio);
    bool GetHostName(char hostName[512]);
signals:

private:
    AirServerCallback* m_pCallback;
    void* m_pServer;
};

#endif // AIRSERVER_H
