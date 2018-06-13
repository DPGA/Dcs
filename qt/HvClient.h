//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	HvClient.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//====================================================================
//====================================================================
#ifndef CHVCLIENT_H
#define CHVCLIENT_H
//====================================================================
//====================================================================
#include <QObject>
#include <QTextEdit>

#include "MyDefs.h"
#include "Tcp.h"
//====================================================================
//====================================================================
#define NB_HVMOD        (4)
#define NB_HVCHAN_MOD   (16)
#define NB_HVCHAN       (NB_HVMOD * NB_HVCHAN_MOD)
#define NB_COEF         (4)

//====================================================================
//====================================================================
typedef union _HVSTAT
{
	u16 global;
	struct {
		u16 actived:1;
		u16 positive:1;
		u16 on:1;
	};
} HVSTAT;
//====================================================================
//====================================================================
typedef struct HVCHAN HVCHAN;
struct HVCHAN
{
	short val;
	short order;
}__attribute__((__packed__));

//====================================================================
//====================================================================
typedef struct HVMOD HVMOD;
struct HVMOD
{
	u16    addr;
	HVSTAT hvstat;
	HVCHAN hvchan[NB_HVCHAN_MOD];
}__attribute__((__packed__));
//====================================================================
//====================================================================
typedef struct HVDATA HVDATA;
struct HVDATA
{
	HVMOD  hvmod[NB_HVMOD];
}__attribute__((__packed__));
//====================================================================
//====================================================================


//====================================================================
//====================================================================
class CHvClient
{
public:
    CHvClient(int verbose = 0);
    ~CHvClient();
    
   HVDATA *    GetData       (void);
   int         Dump          (bool Connected = false);
   int         SwitchHV      (u32 mask, bool onoff , bool Connected = false);
   int         ParseXmlOrder (QString f_name);
   int         LoadOrder     (int mod, bool Connected = false);
   int         ServerStop    (bool Connected = false);

   void        SetVerbose (int verbose);
   void        SetMsgBox  (QTextEdit * msgbox);
   QTextEdit * GetMsgBox  (void);
   void        WriteMsg   (QString str);
   void        WriteMsg   (QString str, QColor col);
   bool        GetHvSat   (int mod);
   void        SetIP      (QString ip, int port);
   QString     GetServer  (void);
   int         GetPort    (void);

private:
    HVDATA hvdt;
    int  m_verbose;
    QTextEdit * p_mbox;
    QString m_str;
    CTCP tcp;
        
    int Manage(int man, int nbarg);
//    int ReadAck(void);
};

//====================================================================
//====================================================================

inline HVDATA * CHvClient::GetData(void)               {return (&hvdt); }

inline void    CHvClient::SetVerbose (int verbose)     { m_verbose = verbose; }
inline void    CHvClient::SetMsgBox (QTextEdit * msgbox) { p_mbox = msgbox; }
inline QTextEdit * CHvClient::GetMsgBox (void)         { return p_mbox; }
inline bool    CHvClient::GetHvSat (int mod)           {return hvdt.hvmod[mod].hvstat.global; }

inline void    CHvClient::SetIP(QString ip, int port)  { tcp.SetIP(ip, port); }
inline QString CHvClient::GetServer(void)              { return (tcp.GetServer()); }
inline int     CHvClient::GetPort(void)                { return (tcp.GetPort()); }

//====================================================================
//====================================================================
//====================================================================
//====================================================================
#endif // CHVCLIENT_H
