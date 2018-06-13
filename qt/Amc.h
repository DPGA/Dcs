//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Amc.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef CAMC_H
#define CAMC_H
//===============================================
#include <QObject>
#include <QTextEdit>

#include "MyDefs.h"
#include "Tools.h"
#include "Tcp.h"
#include "AmcData.h"

//===============================================
//===============================================
#define TEMP_LSB       (0.125)
//===============================================
//===============================================
class CAmc
{
public:
	CAmc(int verbose = 0);
	virtual ~CAmc();

protected:
	AMCDATA amcdt;
	CTCP *p_tcp;

private:
	int m_verbose;
	QString m_str;
	QTextEdit *p_mbox;

	int Manage          (int man, int nbarg);
	int WriteManageCDCE (u32 chip, u32 reg);
	int ReadManageCDCE  (u32 chip);
	void WriteMsg         (QString str);
	void WriteMsg         (QString str, QColor col);

public:
//    AMCDATA amcdt;
	void SetVerbose         (int verbose);
	void SetTcp				(CTCP* tcp);
	void SetMsgBox          (QTextEdit *MsgBox);

	int    GetVerbose		(void);
	u32    GetPrjId 		(void);
	u32    GetSysId 		(void);
	u32    GetFirmRev		(void);
	u32    GetHardRev		(void);
	u32    GetSN			(void);
	QString GetBTime        (void);
	QString GetBDate        (void);
	u32    GetCnfg			(void);
	u32    GetStatus		(void);
	u32    GetSysStamp      (void);
	AMCDATA * GetData       (void);
	QTextEdit * GetMsbBox   (void);


	int  Dump				(void);
	void Display			(void);
	void DisplayId			(void);
	void Reboot				(void);
	int  GetIdentity		(void);
	int  ReadRegister		(u32 regaddr, u32* regval);
	int  WriteRegister	(u32 regaddr, u32 regval);
	int  ReadCDCE			(u32 chip, QString xml);
	int  WriteCDCE			(u32 chip,  u32 val , QString xml);
	int  SaveCDCE			(u32 chip);
	int  ResetCmd			(u32 select);
	int  RunCmd				(u32 Run, u32 Export);
	int  JtagCmd			(u32 Level);

};
//===============================================
//===============================================
inline void CAmc::SetVerbose (int verbose)    { m_verbose = verbose; }
inline void CAmc::SetTcp (CTCP* tcp)          { p_tcp = tcp; }
inline void CAmc::SetMsgBox (QTextEdit *MsgBox) { p_mbox = MsgBox; }
//===============================================
//===============================================
inline int    CAmc::GetVerbose (void)	{ return (m_verbose); }
inline u32    CAmc::GetPrjId   (void)	{ return (amcdt.prjid); }
inline u32    CAmc::GetSysId   (void)	{ return (amcdt.sysid); }
inline u32    CAmc::GetFirmRev (void)	{ return (amcdt.firmrev); }
inline u32    CAmc::GetHardRev (void)	{ return (amcdt.hardrev); }
inline u32    CAmc::GetSN      (void)	{ return (amcdt.sn); }
inline QString CAmc::GetBTime   (void)	{ return ((char*)amcdt.btime); }
inline QString CAmc::GetBDate   (void)	{ return ((char*)amcdt.bdate); }
inline u32    CAmc::GetCnfg    (void)	{ return (amcdt.cnfg.global); }
inline u32    CAmc::GetStatus  (void)	{ return (amcdt.status.global); }
inline u32    CAmc::GetSysStamp(void)   { return (amcdt.sysstamp); }

inline AMCDATA * CAmc::GetData (void)   { return (&amcdt); }
inline QTextEdit * CAmc::GetMsbBox(void){ return (p_mbox); }

//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================

#endif // CAMC_H
