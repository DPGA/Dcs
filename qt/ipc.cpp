#include "ipc.h"



ipc::ipc()
{
 g_request = msgget( KEYREQUEST, 0644 | IPC_CREAT );
 if ( -1 == g_request )  {
     qDebug() << "msgget request opening error";
 }
 g_answer = msgget( KEYANSWER, 0644 | IPC_CREAT );
 if ( -1 == g_answer )  {
     qDebug() << "msgget answer opening error";
 }
 index = 0;
 ipcSend(IPCNONE,"Dcs ok");
}

void ipc::ipcSend(eCmd cmd,long val)
{
    MESSAGE msg;
    msg.nMsgType = 1;
    msg.cmd =  cmd;
    msg.arg.val = val;
    g_request = msgget( KEYREQUEST, 0644 | IPC_CREAT );
    if ( -1 == g_request )  {
     qDebug() << "msgget request opening error";
    }

   if ( -1 == msgsnd( g_request, &msg, sizeof(msg), 0 ) )
          qDebug() << "error send " << errno;
    else
        qDebug() << "send succesfull" ;

}

void ipc::ipcSend(eCmd cmd,const char *message)
{
    MESSAGE msg;
    msg.nMsgType = 1;
    msg.cmd =  cmd;
    sprintf(&msg.arg.sText[0],"%s",message);
    g_request = msgget( KEYREQUEST, 0644 | IPC_CREAT );
	if ( -1 == g_request )  {
     qDebug() << "msgget request opening error";
	}
   
   if ( -1 == msgsnd( g_request, &msg, sizeof(msg), 0 ) )
          qDebug() << "error send " << errno;
    else
        qDebug() << "send succesfull" ;
}

void ipc::ipcReceived()
{

}
