//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Tcp.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef CTCP_H
#define CTCP_H
//-----------------------------------------------

#include <QObject>

#include <string>
#include "MyDefs.h"

//-----------------------------------------------
//-----------------------------------------------
#define TCP_BUF8_LEN         (1500)
#define TCP_BUF16_LEN        (TCP_BUF8_LEN >> 1)
#define TCP_BUF32_LEN        (TCP_BUF8_LEN >> 2)
#define TCP_NB_ARG_16        (TCP_BUF16_LEN - 6)
#define TCP_NB_ARG_32        (TCP_BUF32_LEN - 3)
//-----------------------------------------------
//-----------------------------------------------
typedef union _TCP_MSG {
    u8  buf8  [TCP_BUF8_LEN];
    u16 buf16 [TCP_BUF16_LEN];
    u32 buf32 [TCP_BUF32_LEN];
    struct {
        u16 indic;           // Frame Index
        u16 stat;            // Command Status
        u16 cmd;             // Command Number
        u16 nbarg;           // Number of Arguments
        u32 mask;            // Destination Mask
        union {
            u16 arg16 [TCP_NB_ARG_16]; // List of Arguments in Short Word
            u32 arg32 [TCP_NB_ARG_32]; // List of Arguments in Long Word
        };
    };
}TCP_msg;

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
class CTCP
{
public:
    CTCP(int verbose = 0);
    virtual ~CTCP ();

    void    SetIP (QString ip, int port);
    void    SetVerbose (int verbose);
    QString GetServer (void);
    int     GetPort (void);
    void    ClearBuf (void);
    int     Connect_Exchange (bool returnflag = true);
    int     Exchange (bool returnflag = true);
    int     Connect (void);
    void    Disconnect (void);
    int     Send_Frame (char* frame, const size_t length);
    int     WaitRequest (char* buffer, const size_t length);
    bool    IsConnected (void);

    TCP_msg frm_cmd;
    TCP_msg frm_rep;

 private:
    int m_port;
//    QString m_server;
    std::string m_server;
    int m_sock;
    int m_verbose;
    QString m_str;
    bool m_connect;
};
//-----------------------------------------------
//-----------------------------------------------
//inline void CTCP::SetIP(QString ip, int port)  { m_server = ip; m_port = port; }
inline void CTCP::SetIP(QString ip, int port)  { m_server = ip.toStdString(); m_port = port; }
inline void CTCP::SetVerbose(int verbose)      { m_verbose = verbose; }

//inline QString CTCP::GetServer(void)           { return (m_server); }
inline QString CTCP::GetServer(void)           { return (QString::fromStdString(m_server)); }
inline int     CTCP::GetPort(void)             { return (m_port); }
inline bool    CTCP::IsConnected (void)        { return (m_connect); }
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
#endif // CTCP_H
