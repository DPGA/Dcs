#ifndef IPC_H
#define IPC_H
#include <sys/msg.h>
#include <QDebug>
#include "ipcdaq.h"

class ipc
{
public:
    ipc();
    void ipcSend(eCmd cmd, const char *message);
    void ipcSend(eCmd cmd,long val);
    void ipcReceived();
private:
    int g_answer;
    int g_request;
    unsigned long index;
};

#endif // IPC_H
