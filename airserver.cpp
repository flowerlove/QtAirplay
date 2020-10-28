#include "airserver.h"
#include <Windows.h>

AirServer::AirServer(QObject *parent) : QObject(parent)
{
    m_pCallback = new AirServerCallback();
    m_pServer = NULL;
}

AirServer::~AirServer()
{
    delete m_pCallback;
    Stop();
}

void AirServer::Start()
{
    Stop();
    char hostName[512];
    memset(hostName, 0, sizeof(hostName));
    GetHostName(hostName);
    char serverName[1024] = { 0 };
    sprintf_s(serverName, 1024, "FgAirplay[%s]", hostName);
    m_pServer = fgServerStart(serverName, 5001, 7001, m_pCallback);
}

void AirServer::Stop()
{
    if (m_pServer != NULL) {
        fgServerStop(m_pServer);
        m_pServer = NULL;
    }
}

float AirServer::SetVideoScale(float ratio)
{
    return fgServerScale(m_pServer, ratio);
}

bool AirServer::GetHostName(char hostName[512])
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    gethostname(hostName, 511);
    if (strlen(hostName) > 0)
    {
        return true;
    }
    else
    {
        DWORD n = 511;

        if (::GetComputerNameA(hostName, &n))
        {
            if (n > 2)
            {
                hostName[n] = '\0';
            }
        }
        return true;
    }
}
