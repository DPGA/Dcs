///===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Asm.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//=========================================================================
//=========================================================================
#ifndef CASM_H
#define CASM_H
//=========================================================================
#include <QObject>
#include <QTextEdit>
#include <QProgressBar>
#include <QLineEdit>

#include "MyDefs.h"
#include "Tcp.h"

//=========================================================================
//=========================================================================
#define PhaseClkAdc            	(0)
#define Spare2					(1)
#define Spare3               	(2)
#define Spare4                  (3)
#define InitCm1             	(4)
#define InitCm2             	(5)
#define InitCm3             	(6)
#define InitDacRoFs 			(7)
#define InitT1					(8)
#define InitT2					(9)
#define InitT3					(10)
#define RESETDrs    			(11)
#define InitDrs             	(12)
#define StepClockAdc			(13)
#define StopCalibAdc			(14)
#define CalibAdc9222			(15)
#define WRITECDCE               (16)
#define READCDCE                (17)
#define COPYEECDCE              (18)
#define CYCLEPOWERCDCE          (19)
#define SYNCCDCE				(20)
#define INITCFGDRS              (21)



const u32 conf[] = { 1 << PhaseClkAdc, 1 << Spare2, 1 << Spare3, 1 << Spare4, 1<<InitCm1, 1<<InitCm2, 1<<InitCm3, 1<<InitDacRoFs,
                     1 << InitT1, 1<<InitT2, 1<<InitT3, 1<<RESETDrs, 1<<InitDrs, 1<<StepClockAdc,1 << StopCalibAdc, 1 << CalibAdc9222,
                     1 << WRITECDCE,1 << READCDCE, 1 << COPYEECDCE, 1 <<CYCLEPOWERCDCE ,1 << SYNCCDCE,1 << INITCFGDRS};
//#define EraseEPCS			(16)
//=========================================================================
//=========================================================================
#define ASM_NB_CHAN 		(24)
#define ASM_NB_ROFS			(6)

//#define ASM_CONF_STROBE_ADDR (0x0022)
#define ASM_EPCS_STATUS_ADDR (0x0010)
//=========================================================================
//=========================================================================
typedef struct S_RegAsm S_RegAsm;
struct S_RegAsm
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
typedef struct ASMREAD_ONLY ASMREAD_ONLY;
struct ASMREAD_ONLY {
	u32 BoardId;					// Rd_Register : Board Identifier
	u32 BoardSN;					// Rd_Register : Board Serial Number
	u16 HwRev_Maj;					// Rd_Register : Hardware revision ex : 1.00
	u16 HwRev_Min;					// Rd_Register : Hardware revision ex : 1.00
	u16 FwRev_Maj;					// Rd_Register : Firmware revision ex : 1.00
	u16 FwRev_Min;					// Rd_Register : Firmware revision ex : 1.00
	u32 BuildDate;					// Rd_Register : Build Date
	u32 BuildTime;					// Rd_Register : Build Time
	u32 AddrConf;					// Rd_Register : Address Configuration
	u32 Stat_GXB;					// Rd_Register : Status GBX

	u16 Cpt_Single_Msb;             // Rd_Register : Single Counter
	u16 Cpt_Single_Mid;         	// Rd_Register : Single Counter
	u16 Cpt_Single_Lsb;             // Rd_Register : Single Counter
	u16 Cpt_Sgl_Veto_Msb;           // Rd_Register : Single Veto Counter
	u16 Cpt_Sgl_Veto_Mid;           // Rd_Register : Single Veto Counter
	u16 Cpt_Sgl_Veto_Lsb;           // Rd_Register : Single Veto Counter
	u16 Cpt_Clk64_Msb;              // Rd_Register : Clock64 Counter
	u16 Cpt_Clk64_Mid;          	// Rd_Register : Clock64 Counter
	u16 Cpt_Clk64_Lsb;              // Rd_Register : Clock64 Counter
	u16 Cpt_64Inib_Msb;         	// Rd_Register : Inhibit64 Counter
	u16 Cpt_64Inib_Mid;         	// Rd_Register : Inhibit64 Counter
	u16 Cpt_64Inib_Lsb;         	// Rd_Register : Inhibit64 Counter
	u16 Cpt_Trigin_Msb;         	// Rd_Register : Trigger In Counter
	u16 Cpt_Trigin_Mid;             // Rd_Register : Trigger In Counter
	u16 Cpt_Trigin_Lsb;             // Rd_Register : Trigger In Counter
	u16 Pll_Lock;					// Rd_Register : Pll Locked
} __attribute__((__packed__));

//====================================================================
//====================================================================
typedef struct ASMREAD_WRITE ASMREAD_WRITE;
struct ASMREAD_WRITE
{
	u16 FrontEnd;					// Rw_Register : Front End Identifier
	u16 Mode;						// Rw_Register : Acquisition Mode
	u32 Conf_Strobe;				// Rw_Register	: Configuration Strobe

	u16 Dac_Cm[ASM_NB_CHAN];        // Rw_Register : Dac Cm Channel 0..23
	struct
	{
		u16 Low;					// Rw_Register : Low  Threshold Channel 0..23
		u16 High;					// Rw_Register : High Threshold Channel 0..23
	}  Threshold[ASM_NB_CHAN];
	u32 Jitter_Clean[NB_CDCE_REG];  // Rw_Register : Jitter Cleaner MSB Register 0..8
	u16 DacRofs[ASM_NB_ROFS];       // Rw_Register : Dac Rofs 0..5

	u16 NbSamples;					// Rw_Register : Number of Samples
	u16 ChannelRead;				// Rw_Register : Channel to Read
	u32 Width_Delay;				// Rw_Register	: Width & Delay High State
	u32 Filter_Thr_H;				// Rw_Register	: Filter Threshold High State
	u32 Filter_Thr_L;				// Rw_Register	: Filter Threshold Low State
	u16 Blonde_Offset;              // Rw_Register : Blonde TimeStamp Offset
    u16 Pattern;                    // Rw_Register : Shift DRSAdc and Pattern
    u32 ShiftClockData;             // Rw_Register : void CAsm::decodeStatus(QLineEdit *Asmstatus,QLineEdit *AsmstatusPll)Shift Clock Deserialize data
    u16 ShiftPllAdc;                // Rw_Register : Select shift clock PLL ADC
    u16 FreqPied;                   // Rw_Register : Spare1
    u16 RegSpare2;                  // Rw_Register : Spare2
    u16 RegSpare3;                  // Rw_Register : Spare3
    u16 RegSpare4;                  // Rw_Register : Spare4
    u16 RegSpare5;                  // Rw_Register : Spare5
}__attribute__((__packed__));
//====================================================================
//====================================================================
typedef struct ASMDATA ASMDATA;
struct ASMDATA
{
	struct ASMREAD_ONLY rd;
	ASMREAD_WRITE rw;
}__attribute__((__packed__));
//=========================================================================
//=========================================================================

enum eTypeCal {START=0,STOP=1,STEPCLK=2,PHASECLOCKADC=3,CALIBVCO=4};
	
//=========================================================================
//=========================================================================	
class CAsm
{
	public:
		CAsm(int verbose = 0);
		virtual ~CAsm();

	protected:
		ASMDATA asmdt[NB_ASM];
		CTCP *p_tcp;
		std::list <S_RegAsm> regasmlist;


	private:
		int m_verbose;
		int m_channel;
		u16 m_address;
		u16 m_cfg_addr;
		u16 m_epcs_stat_addr;
		u32 m_usemask;
        QTextEdit *m_console;
		QString m_str;
		QTextEdit * p_mbox;
		QProgressBar *p_bar;
		u16 RandomErase;
        bool VerifCdce[8];

		u16 ComputeParity     (u16 us);
		int SingleMask        (u32 mask);
		int Manage            (int man, int nbarg);
 //     int ReadReply         (u32 channel, u16 FrontEnd, u16 NbToRead, u16* StartAddr);
		int ReadReply         (u32 channel, u16* StartAddr);
		int ReadAck           (int nback);
        int ReadAckProg       (int nback,bool *Err);
		int ParseXmlReg       (int channel, QString f_name);
        int StatusEpcs        (u32 Mask,u16 FrontEnd, u16* Status, bool Connected=false);
		void WriteMsg         (QString str);
		void WriteMsg         (QString str, QColor col);


	public:
		void SetVerbose       (int verbose);
		void SetMsgBox        (QTextEdit * msgbox);
		void SetTcp           (CTCP* tcp);
		void SetUse           (u32 use);
		int  GetVerbose       (void);
		QTextEdit * GetMsgBox (void);
		int  GetChannel       (void);
		int  Dump             (u32 mask);
		void Display          (u32 mask);
		void DisplayReadWrite (u32 mask);
		void DisplayReadOnly  (u32 mask);
		int  ReadCmd          (u32 Mask, u16 FrontEnd, u16 NbToRead, u16 FirstAddr, u16* Dest, bool Connected =false);
		int  WriteCmd         (u32 Mask, u16 FrontEnd, u16 NbToWrite, u16 StartAddr, u16* val, bool Connected =false);
		int  ResetCmd         (u32 Mask, u16 FrontEnd, bool Connected=false);
		int  RunCmd           (u32 Mask, u16 FrontEnd, u16 Run, bool Connected =false);
		int  RunCmdMulti      (u32 Mask, u16 FrontEnd, u16 Run,bool *Err, bool Connected =false);
		int  ChangCmd         (u32 Mask, u16 FrontEnd, bool Connected=false);
		int  ConfigCmd        (u32 Mask, u16 FrontEnd, QString f_name, bool DoIt =true);
        int  CalibCmd		  (u32 Mask, u16 FrontEnd, uint8_t TypeCalib);
		int  SendProg         (u32 Mask, u16 FrontEnd, QString f_name);
		int  WaitEpcsBusy     (u32 Mask, u16 FrontEnd, u16* Status, u16 nbtry,u32 TimeWait);
		int  EraseEpcs        (u32 Mask, u16 FrontEnd,bool Confirmation);
		int  WritePage        (u32 Mask, u16 FrontEnd, std::vector <u8> *Buf);
		ASMDATA * GetData     (u32 Channel);
		void  SetBar          (QProgressBar *bar);
        void Message(int ret, QString message, const QString endmessage="");
        void setConsole       (QTextEdit *console);
        void decodeStatus(QLineEdit *Asmstatus, QLineEdit *AsmstatusPll, u32 m_mask, u32 m_chan, const bool m_read=true);
        int ConfigCdce62005(u32 Mask, u16 FrontEnd, u16 addrReg, u16 mchannel, const bool connected=true);

};
//=========================================================================
//=========================================================================
inline void CAsm::SetVerbose (int verbose)    { m_verbose = verbose; }
inline void CAsm::SetTcp (CTCP* tcp)          { p_tcp = tcp; }
inline void CAsm::SetUse (u32 use)            { m_usemask = use; }
inline void CAsm::SetMsgBox(QTextEdit* msgbox){ p_mbox = msgbox; }
inline void CAsm::SetBar (QProgressBar *bar)  { p_bar = bar; }

inline int  CAsm::GetChannel(void)            { return m_channel; }
inline int  CAsm::GetVerbose(void)            { return m_verbose; }
inline QTextEdit * CAsm::GetMsgBox (void)     { return p_mbox; }
inline ASMDATA * CAsm::GetData(u32 Channel)  {return (&asmdt[Channel]); }
//=========================================================================
//=========================================================================
#endif // CASM_H
