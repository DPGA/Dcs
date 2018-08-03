//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Thor.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//====================================================================
//====================================================================
#ifndef CTHOR_H
#define CTHOR_H
//====================================================================
#include <QObject>
#include <QTextEdit>
#include <QProgressBar>

#include "MyDefs.h"
#include "Tcp.h"

//====================================================================
//====================================================================
#define THOR_Reset40M 		(0)
#define THOR_InitPll40M		(1)
//====================================================================
//====================================================================
//#define THOR_CONF_STROBE_ADDR		(0x0020)
#define THOR_EPCS_STATUS_ADDR		(0x0020)
//====================================================================
//====================================================================
typedef struct S_RegThor S_RegThor;
struct S_RegThor
{
    QString Title;
    u16 Addr;
    union {
        u16 Val16;
        u32 Val32;
        u64 Val64;
    };
    u16 Size;
    u16 Rw;
    QString Comment;
};
//====================================================================
//====================================================================
typedef struct THORREAD_ONLY THORREAD_ONLY;
struct THORDREAD_ONLY
{
    u32  BoardID;
    u32  BoardSN;
    u16  HwRev_Maj;
    u16  HwRev_Min;
    u16  FwRev_Maj;
    u16  FwRev_Min;
    u32  BuildDate;
    u32  BuildTime;
    u32  Addr_Conf;
    u16  GXB_Status;
    u16  Global_Status;
    u16  Trigger_TT;
    u16  Trig_Pattern[3];
    u16  Trig_Stamp[3];
    u16  PreTrig_FifoUsed;
}__attribute__((__packed__));
//====================================================================
//====================================================================
//====================================================================
//====================================================================
typedef struct THORREAD_WRITE THORREAD_WRITE;
struct THORREAD_WRITE
{
    u16 FrontEnd;
    u16 Trigger_Type;
    u16 Conf_Strobe;
    u32 Jitter_Clean[NB_CDCE_REG];
    u64 PreTrig_Mask;
    u16 PreTrig_PedFreq;
    u16 Trig_Shape;
    u16 Trig_Retardment;
    u32 BusyInLevel;
    u32 TriggerInLevel;
    u32 InLvdsLevel;
}__attribute__((__packed__));
//====================================================================
//====================================================================
typedef struct THORDATA THORDATA;
struct THORDATA
{
    THORDREAD_ONLY rd;
    THORREAD_WRITE rw;
}__attribute__((__packed__));
//====================================================================
//====================================================================
//====================================================================
//====================================================================
class CThor
{
    public:
        CThor(int verbose = 0);
        virtual ~CThor();

    protected:

        THORDATA thordt;
        CTCP * p_tcp;
        std::list <S_RegThor> regthorlist;

    private:
        int  m_verbose;
        u16  m_address;
        u16  m_cfg_addr;
        u16  m_mask_addr;
        QString m_str;
        QTextEdit * p_mbox;
        QProgressBar *p_bar;

        u16  ComputeParity    (u16 us);
        int  Manage           (int man, int nbarg);
        int  ReadReply        (u16* StartAddr);
        int  ReadAck          (void);
        int  ParseXmlReg      (QString f_name);
        int  StatusEpcs       (u16 FrontEnd, u32* Status, bool Connected = false);
//        int  WritePage        (u16 FrontEnd, std::vector <u8> *Buf);
//        int  EraseEpcs        (u16 FrontEnd);
//        int  WaitEpcsBusy     (u16 FrontEnd,u32* Status, u16 nbtry);
         void WriteMsg         (QString str);
         void WriteMsg         (QString str, QColor col);


    public:
        void SetVerbose       (int verbose);
        void SetMsgBox        (QTextEdit * msgbox);
        void SetBar           (QProgressBar *bar);
        void SetTcp           (CTCP* tcp);
        int  GetVerbose       (void);
        QTextEdit * GetMsgBox (void);
        u16  GetMaskAddr      (void);
        int  Dump             (void);
        void Display          (void);
        void DisplayReadOnly  (void);
        void DisplayReadWrite (void);
        int  ReadCmd          (u16 FrontEnd, u16 NbToRead, u16 FirstAddr, u16* Dest, bool Connected = false);
        int  WriteCmd         (u16 FrontEnd, u16 NbToWrite, u16 StartAddr, u16* val, bool Connected = false);
        int  ResetCmd         (u16 FrontEnd, bool Connected = false);
        int  RunCmd           (u16 FrontEnd, u16 Run, bool Connected = false);
        int  ChangCmd         (u16 FrontEnd, bool Connected = false);
        int  ConfigCmd        (u16 FrontEnd, QString f_name, bool DoIt = true);
        int  SendProg         (u16 FrontEnd, QString f_name);
        int  WaitEpcsBusy     (u16 FrontEnd,u32* Status, u16 nbtry);
        int  EraseEpcs        (u16 FrontEnd);
        int  WritePage        (u16 FrontEnd, std::vector <u8> *Buf);

        THORDATA * GetData    (void);

};
//====================================================================
//====================================================================
inline void CThor::SetVerbose (int verbose)    { m_verbose = verbose; }
inline void CThor::SetMsgBox  (QTextEdit * msgbox) { p_mbox = msgbox; }
inline void CThor::SetBar     (QProgressBar *bar)  { p_bar = bar; }
inline void CThor::SetTcp (CTCP* tcp)          { p_tcp = tcp; }

 inline THORDATA * CThor::GetData(void)        { return (&thordt); }
 inline QTextEdit * CThor::GetMsgBox (void)    { return p_mbox; }
 inline u16 CThor::GetMaskAddr(void)           { return m_mask_addr; }
//====================================================================
//====================================================================

//====================================================================
//====================================================================
#endif // CTHOR_H
