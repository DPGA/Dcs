//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Thor.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//====================================================================
//====================================================================
//====================================================================
#include <iostream>
#include <fstream>
using namespace std;
#include <QtXml>

#include <unistd.h>

#include "MyDefs.h"
#include "Tools.h"
#include "Thor.h"
#include "command_error.h"
//====================================================================
//====================================================================
CThor::CThor(int verbose)
{
	m_verbose = verbose;
	m_address = 0;
	m_cfg_addr = 0;
	m_mask_addr = 0;
	p_tcp = NULL;
	p_mbox = NULL;
	p_bar = NULL;
	if(m_verbose >= VERBOSE)
		cout << "Create CThor Class" << endl;
	memset((void*)&thordt, 0, sizeof(THORDATA));

}	// Constructor

//====================================================================
//====================================================================
CThor::~CThor()
{
	if(m_verbose >= VERBOSE)
		cout << "Delete CThor Class" << endl;

}	// Destructor

//====================================================================
//====================================================================
//====================================================================
//====================================================================
void CThor::WriteMsg(QString str)
{
	if(p_mbox) {
		p_mbox->append(str);
	}
}   // WriteMsg
//====================================================================
void CThor::WriteMsg(QString str, QColor color)
{
	if(p_mbox) {
		QColor sav = p_mbox->textColor();
		p_mbox->setTextColor(color);
		p_mbox->append(str);
		p_mbox->setTextColor(sav);
	}
}   // WriteMsg
//====================================================================
//====================================================================
//====================================================================
//====================================================================
int CThor::Dump (void)
{
	if(m_verbose) {
		cout << "sizeof(THORDATA)            " << (sizeof(THORDATA))  << endl;
		cout << "sizeof(THORDATA.READ_ONLY)  " << (sizeof(thordt.rd)) << endl;
		cout << "siezof(THORDATA.READ_WRITE) " << (sizeof(thordt.rw)) << endl;
	
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			  << "Thor Dump" << FgColor::white << CtColor::none << endl;
	}
	return (ReadCmd(BROADCAST_FE, sizeof(THORDATA)/2, 0, (u16*)&thordt));
}	// Dump

//====================================================================
//====================================================================
void CThor::DisplayReadOnly(void)
{
	if(m_verbose == QUIET) return;
	cout << endl << "Read Only Part" << endl;
	cout << "Board Id     : " << CFormat("%c", (thordt.rd.BoardID >> 24) & 0xff) << CFormat("%c", (thordt.rd.BoardID >> 16) & 0xff)
							  << CFormat("%c", (thordt.rd.BoardID >> 8) & 0xff)  << CFormat("%c", (thordt.rd.BoardID ) & 0xff)
							  << "\t(0x" << hex << thordt.rd.BoardID << ")" << dec << endl;
	cout << "Hard Rev     : " << thordt.rd.HwRev_Maj << "." << thordt.rd.HwRev_Min << endl;
	cout << "Firm Rev     : " << thordt.rd.FwRev_Maj << "." << thordt.rd.FwRev_Min << endl;
	cout << "Build Date   : " << CFormat("%02x/", (thordt.rd.BuildDate >> 24) & 0xff)
							  << CFormat("%02x/", (thordt.rd.BuildDate >> 16) &0xff)
							  << CFormat("%04x\t(0x", thordt.rd.BuildDate & 0xFFFF)
							  << hex << thordt.rd.BuildDate << ")" << dec << endl;					// Rd_Register : Build Date
	cout << "Build Time   : " << CFormat("%02x:", (thordt.rd.BuildTime >> 16) & 0xFFFF)
							  << CFormat("%02x:", (thordt.rd.BuildTime >> 8) & 0xFF)
							  << CFormat("%02x\t(0x", thordt.rd.BuildTime  & 0xFF)
							  << hex << thordt.rd.BuildTime << ")" << dec << endl;
	cout << "Addr Config  : " << CFormat("0x%08x",thordt.rd.Addr_Conf) << endl;
	cout << "GXB Status   : " << CFormat("0x%04x",thordt.rd.GXB_Status) << endl;
	cout << "Global Status: " << CFormat("0x%04x",thordt.rd.Global_Status) << endl;
	cout << "Trigger_TT   : " << CFormat("0x%04x",thordt.rd.Trigger_TT) << endl;
	cout << "Trig Pattern : " << CFormat("0x%04x",thordt.rd.Trig_Pattern[0]) << CFormat("%04x",thordt.rd.Trig_Pattern[1]) << CFormat("%04x",thordt.rd.Trig_Pattern[2]) << endl;
	cout << "Trig Stamp   : " << CFormat("0x%04x",thordt.rd.Trig_Stamp) << endl;
	cout << "PreTrig FUsed: " << CFormat("0x%04x",thordt.rd.PreTrig_FifoUsed) << endl;
}	// DisplayReadOnly
//====================================================================
//====================================================================
void CThor::DisplayReadWrite(void)
{
	if(m_verbose == QUIET) return;
	cout << endl << "Read/Write Part" << endl;
	cout << "Front End    : " << CFormat("0x%04x",thordt.rw.FrontEnd) << endl;
	cout << "Trigger Type : " << CFormat("0x%04x",thordt.rw.Trigger_Type) << endl;
	cout << "Config Strobe: " << CFormat("0x%04x",thordt.rw.Conf_Strobe) << endl;
	cout << "Jitter Clean : " ;
	for (int i=0; i<NB_CDCE_REG ; i++) {
		cout << CFormat("0x%08x  ", thordt.rw.Jitter_Clean[i]);
	}
	cout << endl;
	cout << "PreTrig Mask : " << CFormat("0x%04x",thordt.rw.PreTrig_Mask) << endl;
	cout << "Pedestal Freq: " << CFormat("0x%04x",thordt.rw.PreTrig_PedFreq) << endl;
	cout << "Trigger Shape: " << CFormat("0x%04x",thordt.rw.Trig_Shape) << endl;
	cout << "Trigger Delay: " << CFormat("0x%04x",thordt.rw.Trig_Retardment) << endl;

}	// DisplayReadWrite
//====================================================================
//====================================================================
void CThor::Display(void)
{
	if(m_verbose) cout << CtColor::underscore << CtColor::reverse << FgColor::blue << "Thor Display" << FgColor::white << CtColor::none << endl;

	DisplayReadOnly();
	DisplayReadWrite();
//    cout << endl << endl;

}	// Display

//====================================================================
//====================================================================
u16 CThor::ComputeParity (u16 us)
{
	u16 parity = 0;
	for(u16 i=0; i<16; i++)
		if(us & (1 << i))
			parity = ~parity;
	return (parity & 0x1);

}	// ComputeParity

//====================================================================
//====================================================================
int CThor::Manage (int man ,int nbarg)
{
	QString err;
	QString title = QString("THOR Manage");
	if(man !=-1) {
		if(p_tcp->frm_cmd.indic == p_tcp->frm_rep.indic) {
			if(m_verbose) {
				cout << "Reply->Stat : " << p_tcp->frm_rep.stat << endl;
				cout << "Reply->NbArg: " << p_tcp->frm_rep.nbarg << endl;
			}
			if(p_tcp->frm_rep.stat == 0) {
				if (p_tcp->frm_rep.nbarg == nbarg) {
					return (NO_ERROR);
				} else if(p_tcp->frm_rep.nbarg == 1)  {
					return(ReadAck());
				//} else if(p_tcp->frm_rep.nbarg > nbarg) {
					//p_tcp->frm_rep.nbarg = nbarg;
					//m_str = QString("Bad Nb Arg :  Expected(%1) received (%2) with Truncated Frame").arg(nbarg).arg(p_tcp->frm_rep.nbarg);
					//if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
					//return (NO_ERROR);
				} else {
					m_str = QString("Bad Nb Arg :  Expected(%1) received (%2)").arg(nbarg).arg(p_tcp->frm_rep.nbarg);
					WriteMsg(m_str,"red");
//					TimerMessage(title, m_str);
					if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
					return (BAD_NBARG);
				}
			} else {
				switch(p_tcp->frm_rep.stat)
				{
					case NO_ERROR                 : err = "Success full"; break;
					case UNKNOW_CMD               : err = "Unknown Command"; break;
					case NOT_IMPLEMENTED_CMD      : err = "Not Implemented Command"; break;
					case NBARG_ERROR              : err = "Wrong Number of Arguments"; break;
					case INVALID_CHAN             : err = "Invalid Channel Error"; break;
					case CDCE_SPI_ERROR           : err = "CDCE SPI Error"; break;
					case BROADCAST_NOTALLOWED_CMD : err = "Command not allowed in Broadcast Mode"; break;
					case FIFO_FULL_ERROR          : err = "Fifo Full Error"; break;
					case DCS_TMO_ERROR            : err = "Dcs Time Out Reached"; break;
					case BAD_ACK                  : err = "Bad Acknowledge Link"; break;
					default                       : err = "Undefined Error";
				}
				m_str = (QString("Bad Reply : Stat (%1)...").arg( p_tcp->frm_rep.stat)) + err;
				WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
				if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
				return (BAD_STATUS);
			}
		} else {
			m_str = QString("Bad Replay : Expected Indic (%1) -> Received (%2)").arg(p_tcp-> frm_cmd.indic).arg(p_tcp->frm_rep.indic);
			WriteMsg(m_str,"red");
//			TimerMessage(title, m_str);
			if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white <<endl;
			return (BAD_INDIC);
		}
	} else {
		m_str = QString("Client can't send command to server ") + p_tcp->GetServer() + (QString(" (errno=%1) : %2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white <<endl;
		return (BAD_CONNECTION);
	}
}	// Manage

//====================================================================
//====================================================================
int CThor::ReadCmd(u16 FrontEnd, u16 NbToRead, u16 FirstAddr, u16* Dest, bool Connected)
{
	u16 cmd;
	u16 parity;
	u16 crc;

	if(m_verbose) {
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor Read Command" << FgColor::white << CtColor::none << endl;
		cout << "ReadCmd : FrontEnd (0x" << hex << FrontEnd
			 << ") NbToRead (0x" << NbToRead
			 << ") Fist Addr (0x" << FirstAddr
			 << ")" << dec << endl;
	}
	cmd    = K28_2  | ((FrontEnd & 0x007F)<< 8);
	parity = ComputeParity(cmd)<< 15;
	cmd   |= parity;
	crc    = 0;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;						// Export Command
	p_tcp->frm_cmd.nbarg = 4;									// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;					// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = NbToRead;
	p_tcp->frm_cmd.arg16[2] = FirstAddr;
	crc ^= p_tcp->frm_cmd.arg16[1];
	crc ^= p_tcp->frm_cmd.arg16[2];
	p_tcp->frm_cmd.arg16[3] = crc;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, NbToRead + 4);
	if(m_verbose > VERBOSE) {
		cout << "indic 0x" << hex << p_tcp->frm_rep.indic << endl;
		cout << "stat  0x" << p_tcp->frm_rep.stat << endl;
		cout << "cmd   0x" << p_tcp->frm_rep.cmd  << endl;
		cout << "nbarg 0x" << p_tcp->frm_rep.nbarg  << endl;
		cout << "mask  0x" << p_tcp->frm_rep.mask   << endl;
		int max = (p_tcp->frm_rep.nbarg <=10) ? p_tcp->frm_rep.nbarg : 10;
		for(int i=0; i<max; i++)
			cout << "arg " << i << " 0x" << p_tcp->frm_rep.arg16[i]  << endl;
		cout << dec << endl;
	}
	if(ret == NO_ERROR)
		ret = ReadReply(Dest);
	return (ret);

}	// ReadCmd

//====================================================================
//====================================================================
int CThor::ReadReply(u16* StartAddr)
{
	int ret;
	u16  crc;
	u16* addr;
	u16* src;
	u16* vv;
	size_t siz;
	QString title = QString("THOR Read Reply");

	if((p_tcp->frm_rep.nbarg == 1) && ((p_tcp->frm_rep.arg16[0] & 0xff) == K28_0))
	{
		ret = p_tcp->frm_rep.arg16[0] & 0xf;
		m_str = QString("Problem inside THOR acknowledge (%1)").arg(ret);
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (ret);
	}
	else if ((p_tcp->frm_rep.nbarg >= 3) && ((p_tcp->frm_rep.arg16[0] & 0xff) == K28_1))
	{
		addr = (u16*)&thordt;
		addr += p_tcp->frm_rep.arg16[2];
		src = (u16*)&(p_tcp->frm_rep.arg16[3]);
		siz = p_tcp->frm_rep.arg16[1] * sizeof(unsigned short);
		if(m_verbose) cout << "Read Data " << addr << " " << src << " " << siz << endl;

		vv = src;
		crc = p_tcp->frm_rep.arg16[1] ^ p_tcp->frm_rep.arg16[2];
		for(u32 i=0; i<p_tcp->frm_rep.arg16[1]; i++) {
			crc ^= *vv;
//			cout << i << " " << hex << crc << " " << *vv << endl;
			vv++;
		}
		if(crc == *vv) {
			if(m_verbose)
				cout << FgColor::green << "Good CRC Calcul 0x" << hex << crc << " Send 0x" << *vv << dec << FgColor::white << endl;
			memcpy(StartAddr, src , siz);
			return (NO_ERROR);
		} else {
			m_str = (QString("Bad CRC Calcul 0x")) + QString::number(crc,16) + (QString(" Send 0x")) + QString::number(*vv,16);
			WriteMsg(m_str,"red");
//			TimerMessage(title, m_str);
			if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (BAD_CRC);
		}
	} else {
		m_str = (QString("Unknow Problem : nbArg %1 ").arg(p_tcp->frm_rep.nbarg)) + QString::number(p_tcp->frm_rep.arg16[0],16);
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString()  << FgColor::white << endl;
		return (BAD_UNKNOW);
	}
}	// ReadReply

//====================================================================
//====================================================================
int CThor::WriteCmd(u16 FrontEnd, u16 NbToWrite, u16 StartAddr, u16* val, bool Connected)
{
	u16 cmd;
	u16 parity;
	u16 crc = 0;
	int i;
//    QString title = QString("THOR Write Command");

	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor Write Command : FrontEnd (0x" << hex << FrontEnd << dec << ")"
			 << FgColor::white << CtColor::none << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;						// Export Command
	p_tcp->frm_cmd.nbarg = NbToWrite  + 4;					// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;					// Destination Mask

	cmd     = K28_1 | ((FrontEnd & 0x007F) << 8);
	parity  = ComputeParity(cmd)<< 15;
	cmd    |= parity;
	crc     = 0;

	p_tcp->frm_cmd.arg16[0] = cmd;				 			// K28_1, FrontEnd;
	p_tcp->frm_cmd.arg16[1] = NbToWrite;
	crc ^= NbToWrite;
	p_tcp->frm_cmd.arg16[2] = StartAddr;
	crc ^= StartAddr;
	for(i=0; i<NbToWrite; i++)
	{
		p_tcp->frm_cmd.arg16[i+3] = *val;
		crc ^= *val;
		val ++;
	}
	p_tcp->frm_cmd.arg16[i+3] = crc;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
//	cout << "ret 1 : " << ret << endl; 
	ret = Manage(ret, 1);
//		cout << "ret 2 : " << ret << endl; 
	if(ret == NO_ERROR){
		ret = ReadAck();
//			cout << "ret 3 : " << ret << endl; 
//		cout << "Command executed" << endl;
	}
//	cout << "ret 4 : " << ret << endl;
	return(ret);

}	// WriteCmd

//====================================================================
//====================================================================
int CThor:: ReadAck(void)
{
	u16 ack;
	int ret = NO_ERROR;
	QString title = QString("THOR Read Acknowledge");

//	cout << i<< " ack " << hex << p_tcp->frm_rep.arg16[0] << dec << endl;
	if((p_tcp->frm_rep.arg16[0] & 0xff)!= K28_0) {
		if((p_tcp-> frm_rep.arg16[0] & 0xff) == DCS_TMO_ERROR) {
			ret = BAD_ACKNOWLEDGE;
			m_str = (QString("DCS_TMO_ERROR on Thor (0x")) + QString::number(p_tcp->frm_rep.arg16[0] & 0xff,16) + QString(")");
			WriteMsg(m_str,"red");
//			TimerMessage(title, m_str);
			if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
		} else {
			ret = BAD_ACKNOWLEDGE;
			m_str = (QString("Bad Acknowledge on Thor (0x")) + QString::number(p_tcp->frm_rep.arg16[0] & 0xff,16) + QString(")");
			WriteMsg(m_str,"red");
//			TimerMessage(title, m_str);
			if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
		  }
	} else {
		ack = (p_tcp->frm_rep.arg16[0] >> 8) & 0x7f;
		switch (ack) {
			case 0                : ret = NO_ERROR;       m_str = (QString("Good Acknowledge on Thor")); break; 
			case (1 << PARITYERR) : ret = BAD_ACK_PARITY; m_str = (QString("Bad Parity Acknowledge on Thor (0x")) + QString::number(ack,16) + (QString(")")); break;
			case (1 << LENERR)    : ret = BAD_ACK_LEN;    m_str = (QString("Bad Len Acknowledge on Thor (0x"))    + QString::number(ack,16) + (QString(")")); break;
			case (1 << CRCERR)    : ret = BAD_ACK_CRC;    m_str = (QString("Bad Len Acknowledge on Thor (0x"))    + QString::number(ack,16) + (QString(")")); break;
			case (1 << ENDERR)    : ret = BAD_ACK_END;    m_str = (QString("Bad End Acknowledge on Thor (0x"))    + QString::number(ack,16) + (QString(")")); break;
			case (1 << STARTERR)  : ret = BAD_ACK_START;  m_str = (QString("Bad Start Acknowledge on Thor (0x"))  + QString::number(ack,16) + (QString(")")); break;
			case (1 << FENOERR)   : ret = BAD_ACK_FENO;   m_str = (QString("Bad FE N° Acknowledge on Thor (0x"))  + QString::number(ack,16) + (QString(")")); break;
			case (1 << EPCSEERR)  : ret = BAD_ACK_EPCS;   m_str = (QString("Bad EPCS Acknowledge on Thor (0x"))   + QString::number(ack,16) + (QString(")")); break;
			default               : ret = BAD_ACK_BIZZ;   m_str = (QString("Unknown Acknowledge on Thor (0x"))    + QString::number(ack,16) + (QString(")")); break;
		}
		if(ack) {
			WriteMsg(m_str,"red");
//			TimerMessage(title, m_str);
			if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white <<endl;
		} else {
			WriteMsg(m_str,"green");
			if(m_verbose) cout << FgColor::green << m_str.toStdString() << FgColor::white <<endl;
		}
	}
	return(ret);

}	// ReadAck

//=========================================================================
//=========================================================================
int CThor::ResetCmd(u16 FrontEnd, bool Connected)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor ResetCmd : FrontEnd (0x" << hex << FrontEnd << dec << ")"
			 << FgColor::white << CtColor::none << endl;

	u16 cmd = K28_3  | ((FrontEnd & 0x007F) << 8);
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_SILENT_EXPORT;			// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;      		// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = CMDRST;

	int ret = (Connected) ? p_tcp->Exchange(false) :  p_tcp->Connect_Exchange(false) ;
	return (ret);
}	// Reset

//====================================================================
//====================================================================
int CThor::RunCmd(u16 FrontEnd, u16 Run, bool Connected)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor RunCmd : FrontEnd (0x" << hex << FrontEnd << dec << ")"
			 << FgColor::white << CtColor::none << endl;

	u16 cmd = K28_3  | ((FrontEnd & 0x007F) << 8);
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;				// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	if(Run== 1)
		p_tcp->frm_cmd.arg16[1] = CMDSTARTRUN;
	else
		p_tcp->frm_cmd.arg16[1] = CMDSTOPRUN;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, 1);
	if(ret == 0){
		ret = ReadAck();
//		cout << "Command executed" << endl;
	}
	return(ret);

}	// RunCmd

//====================================================================
//====================================================================
int CThor::ChangCmd(u16 FrontEnd, bool Connected)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor ChangCmd : FrontEnd (0x" << hex << FrontEnd << dec << ")"
			 << FgColor::white << CtColor::none << endl;

	u16 cmd = K28_3 | ((FrontEnd & 0x007F) << 8);
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;				// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = CMDCHANGE;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, 1);
	if(ret == 0){
		ret = ReadAck();
//		cout << "Command executed" << endl;
	}
	return(ret);

}	// RunChang

//====================================================================
//====================================================================
int CThor::ConfigCmd(u16 FrontEnd, QString f_name, bool DoIt)
{
	const u16 cnf[] = { 1<<THOR_Reset40M , 1<<THOR_InitPll40M};
	u32 nbtry;

	if(f_name.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			f_name = QString("%1/ConfigDcs/RegThor.xml").arg(dirdcs); 
		} else {
#ifdef Q_OS_OSX
			f_name = "../../../../config/RegThor.xml";
#else
			f_name =  "../../ConfigDcs/RegThor.xml";
#endif
		}
	}
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			 << "Thor ConfigCmd : FrontEnd (0x" << hex << FrontEnd << dec << ") : " << f_name.toStdString()
			 << FgColor::white << CtColor::none << endl;
	int ret = ParseXmlReg(f_name);
	if (ret == NO_ERROR) { //XML_SUCCESS) {
		DisplayReadWrite();
		if(m_verbose) cout << "Thor Configuration " << endl;
		ret = p_tcp->Connect();
		if(ret == NO_ERROR) {
			ret = WriteCmd(FrontEnd,(sizeof(thordt.rw) /2), m_address, &(thordt.rw.FrontEnd), true);
			if(m_verbose) cout << "Config WriteCmd Ret " << ret << endl << endl;
			if(ret == NO_ERROR) {
				if(DoIt) {
					for(int i=0; i<2; i++) {
						nbtry = 0;
						do {
							ret =  WriteCmd(FrontEnd, 1, m_cfg_addr,  (u16*) &cnf[i], true);
							if(ret != NO_ERROR) {
								nbtry ++;
								if(nbtry >= 3) {
									p_tcp->Disconnect();
									return(ret);
								}
							}
						} while (ret != NO_ERROR);
						nbtry = 0;
						do {
							ret = ChangCmd(FrontEnd, true);
							if(ret != NO_ERROR) {
								nbtry++;
								if(nbtry >= 3) {
									p_tcp->Disconnect();
									return(ret);
								}
							}
						} while (ret != NO_ERROR);
					}
				}
				if((ret == NO_ERROR) && (m_verbose))
					cout << FgColor::green << "Command executed" << FgColor::white  << endl;
			}
		}
		p_tcp->Disconnect();
	}
	return (ret);

}	// Config

//====================================================================
//====================================================================
int CThor::ParseXmlReg(QString f_name)
{
	QString title = "THOR Parse XML";
	QDomDocument xmlDoc;
	QFile f(f_name);
	if(!f.open(QIODevice::ReadOnly)) {
		 m_str = QString("Error while loading File : ") + f_name + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		 WriteMsg(m_str,"red");
//		 TimerMessage(title, m_str);
		 if(m_verbose) cerr << m_str.toStdString() << endl;
		 return(errno);
	}
	bool ret = xmlDoc.setContent(&f);
	if(!ret) {
		f.close();
		m_str = QString("Error while Reading File : ") + f_name + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << m_str.toStdString() << endl;
		return(errno);
	}
	f.close();
	if(m_verbose) cout << "Thor ParseXML SetContent :" << ret << endl;

	int i = 0;
	QDomElement root = xmlDoc.documentElement();
	if(!root.isNull()) {
		QDomElement child = root.firstChildElement("REG");
		while(!child.isNull()) {
		  i++;
		  if(m_verbose > VERBOSE) {

			  cout << i << " " << root.tagName().toStdString()
				   << " " << child.tagName().toStdString()
				   << " TITLE :" << child.attribute("TITLE").toStdString()
				   << " ADDRESS :" << child.attribute("ADDRESS").toStdString()
				   << " VALUE :" << child.attribute("VALUE").toStdString()
				   << " SIZE :" << child.attribute("SIZE").toStdString()
				   << " RW :" << child.attribute("RW").toStdString()
				   << " COMMENT :" << child.attribute("COMMENT").toStdString() << endl;
		  }
		  if(child.attribute("TITLE") == "FrontEnd") m_address = child.attribute("ADDRESS").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Conf_Strobe") m_cfg_addr = child.attribute("ADDRESS").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "PreTrig_Mask") m_mask_addr = child.attribute("ADDRESS").toUInt(&ret,16);

		  if     (child.attribute("TITLE") == "FrontEnd")          thordt.rw.FrontEnd = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Trigger_Type")      thordt.rw.Trigger_Type = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Conf_Strobe")       thordt.rw.Conf_Strobe = child.attribute("VALUE").toUInt(&ret,16);

		  else if(child.attribute("TITLE") == "Jitter_Clean_0")    thordt.rw.Jitter_Clean[0] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_1")    thordt.rw.Jitter_Clean[1] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_2")    thordt.rw.Jitter_Clean[2] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_3")    thordt.rw.Jitter_Clean[3] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_4")    thordt.rw.Jitter_Clean[4] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_5")    thordt.rw.Jitter_Clean[5] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_6")    thordt.rw.Jitter_Clean[6] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_7")    thordt.rw.Jitter_Clean[7] = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Jitter_Clean_8")    thordt.rw.Jitter_Clean[8] = child.attribute("VALUE").toUInt(&ret,16);

//		  else if(child.attribute("TITLE") == "PreTrig_Mask")      thordt.rw.PreTrig_Mask = child.attribute("VALUE").toUInt(&ret,16);
          else if(child.attribute("TITLE") == "PreTrig_Mask")
              thordt.rw.PreTrig_Mask = child.attribute("VALUE").toULongLong(&ret,16);
		  else if(child.attribute("TITLE") == "PreTrig_PedFreq")   thordt.rw.PreTrig_PedFreq = child.attribute("VALUE").toUInt(&ret,16);
          else if(child.attribute("TITLE") == "Trig_Shape")        thordt.rw.Trig_Shape      = child.attribute("VALUE").toUInt(&ret,16);
		  else if(child.attribute("TITLE") == "Trig_Delay")        thordt.rw.Trig_Retardment = child.attribute("VALUE").toUInt(&ret,16);
          else if(child.attribute("TITLE") == "BusyInLevel")       thordt.rw.BusyInLevel     = child.attribute("VALUE").toUInt(&ret,16);
          else if(child.attribute("TITLE") == "TriggerInLevel")    thordt.rw.TriggerInLevel  = child.attribute("VALUE").toUInt(&ret,16);
          else if(child.attribute("TITLE") == "InLvdsLevel")       thordt.rw.InLvdsLevel     = child.attribute("VALUE").toUInt(&ret,16);

          child = child.nextSiblingElement();
	   }
	} else {
		m_str = QString("Error while Parsing File : ") + f_name + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << m_str.toStdString() << endl;
		return(errno);
	}

	return(NO_ERROR);

}	// ParseXmlReg

//====================================================================
//====================================================================
//====================================================================
//====================================================================
int CThor::EraseEpcs(u16 FrontEnd)
{
	u32 Status;
	if(m_verbose)
		cout << "Thor Erasing Epcs (" << hex << FrontEnd << dec << ")" << endl;

	u16 cmd = (K28_3 | ((FrontEnd & 0x007F) << 8));
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;      		// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] =  CMDERASEEPCS;
	int ret = Manage(p_tcp->Connect_Exchange(),1);
	if(ret == NO_ERROR) {
		ret = ReadAck();
	} else {
		WriteMsg("Can't Erase EPCS !", "red");
		if(m_verbose) cerr << FgColor::red << "Can't Erase EPCS !"<< FgColor::white << endl;
		return (ret);
	}
	usleep(1000);
	ret = WaitEpcsBusy (FrontEnd,&Status, 5);
	WriteMsg(" Status : 0x " + QString::number(Status,16));
//    cout << " Status : 0x" << hex << Status << dec << endl;
	return (ret);

}	// EraseEpcs

//====================================================================
//====================================================================
int CThor::StatusEpcs(u16 FrontEnd, u32* Status, bool Connected)
{
	int ret = ReadCmd(FrontEnd, 2, THOR_EPCS_STATUS_ADDR, (u16*) Status, Connected);
	if(m_verbose)
		cout << "Epcs Status 0x" << hex << *Status << dec << endl;
	return(ret);

}	// StatusEpcs

//====================================================================
//====================================================================
int CThor::WaitEpcsBusy (u16 FrontEnd,u32* Status, u16 nbtry)
{
	int verb = m_verbose;
	u16 ess  = nbtry;
	int ret;
	int bar = 0;

	m_verbose = 0;
	if(p_bar) {
		p_bar->setValue(bar);
		p_bar->setMaximum(1000);
	}

	ret = p_tcp->Connect();
	if (ret == NO_ERROR) {
		do {
			ret= StatusEpcs(FrontEnd, Status,true);
			if(ret != NO_ERROR ) {
				ess --;
				if(ess == 0)
				return (ret);
			} else {
				ess = nbtry;
			}
			bar++;
			if(p_bar) p_bar->setValue(bar);
			if(m_verbose) cout << ".";
			usleep(1000);
		}	while( *Status & 0x1000000);				// EEprom Busy
	}
	p_tcp->Disconnect();
	m_verbose = verb;
	return (ret);

}	// WaitEpcsBusy

//====================================================================
//====================================================================
int CThor::WritePage(u16 FrontEnd, std::vector <u8> *Buf)
{
	u32 Addr = 0;
	u32 size = Buf->size();
	u32 max;
	u16 crc;
	union {
		u16 s;
		u8 c[2];
	}tmp;
	int ret;
	bool prog;
	bool ErrProg;

	ret = p_tcp->Connect();
	if(ret != 0) return (ret);
	if(p_bar) {
		p_bar->setValue(0);
		p_bar->setMaximum(size);
	}
	while (Addr <size) {
		crc = Addr >> 8;
		max = ((Addr+256) > size) ? (size-Addr) : 256;
			m_str.append(QString("Write : 0x%1...%2").arg(Addr, 6,16, QChar('0')).arg(size, 6, 16, QChar('0')));
		WriteMsg(m_str);
		if(m_verbose) cout << CFormat("Write : 0x%06x",Addr) << CFormat("\t 0x%06x", size) << endl;
		prog = false;
		for(u32 k=0; k<max; k++) {
			if(Buf->at(k+Addr) != 0xff) {
				prog = true;
				break;
			}
		}
		if(prog) {
			u16 cmd = (K30_7 | ((FrontEnd & 0x007F) <<8));
			u16 parity = ComputeParity(cmd)<< 15;
			cmd |= parity;

			p_tcp->ClearBuf();
			p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
			p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
			p_tcp->frm_cmd.cmd   = CMD_EXPORT;						// Export Command
			p_tcp->frm_cmd.nbarg = 3 + (max/2);						// Number of Arguments
			p_tcp->frm_cmd.mask  = 1 << THOR_CHAN;					// Destination Mask
			p_tcp->frm_cmd.arg16[0] = cmd;
			p_tcp->frm_cmd.arg16[1] = (u16) (Addr >> 8);
			u32 j = 2;
			for(u32 i=0; i<max; i+=2) {
				tmp.c[0] = Reverse(Buf->at(i+Addr));
				tmp.c[1] = Reverse(Buf->at(i+Addr+1));
				p_tcp->frm_cmd.arg16[j] = tmp.s;
//				cout << " buf " << hex << (u16)(Buf->at(i+Addr)) << " " << (u16)(Buf-> at(i+1+Addr)) << "...." << tmp.s << dec << endl;
				crc ^= tmp.s;
				j++;
			} // for(u32 i=0; i<max; i+=2)
			p_tcp->frm_cmd.arg16[j] = crc;

			ret = p_tcp->Exchange();
			if(ret == NO_ERROR) {
				ret = Manage(ret,1);
				if(ret == NO_ERROR) {
					ret = ReadAck();
					if(ret) {
						WriteMsg("Error Acknowlege Programming", "red");
						if(m_verbose) cerr << FgColor::red << "Error Acknowlege Programming" << FgColor::white << endl;
						ErrProg = true;
					} else {
						ErrProg = false;
					}
				} else {
					WriteMsg("Error TCP Programming", "red");
					if(m_verbose) cerr << FgColor::red << "Error TCP Programming" << FgColor::white << endl;
					ErrProg = true;
				}
			} else {
				m_str = "Client Can't Send to Server " + p_tcp->GetServer();
				WriteMsg(m_str, "red");
				p_tcp->Disconnect();
				return(ret);
			}
		}	// if(prog)
		if(ErrProg == false) Addr+=256;
		if(p_bar) p_bar->setValue(Addr);
	} // while (Addr <size)
	p_tcp->Disconnect();
//    cout << FgColor::green << "End of Programming" << FgColor::white << endl;
	return (NO_ERROR);

}	// WritePage

//====================================================================
//====================================================================
int CThor::SendProg(u16 FrontEnd, QString f_name)
{
	int ret;
	u32 Status;
	bool EpcsOk = false;
	std::vector <u8> Memory;
	time_t t1,t2;

	t1 = time(NULL);
	if(f_name.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			f_name = QString("%1/ConfigDcs/thor.rpd").arg(dirdcs);
		} else {
			f_name = "../../ConfigDcs/thor.rpd";
		}
	}
	ret = WaitEpcsBusy(FrontEnd, &Status, 3);
	if(ret != NO_ERROR)
		return(ret);
	switch (Status & 0xff) {
		case EPCS1 : cout << FgColor::green << "EPCS1  (0x" << hex << (Status & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
		case EPCS4 : cout << FgColor::green << "EPCS4  (0x" << hex << (Status & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
		case EPCS16: cout << FgColor::green << "EPCS16 (0x" << hex << (Status & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
		case EPCS64: cout << FgColor::green << "EPCS64 (0x" << hex << (Status & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
		default    : cout << FgColor::red   << "Memory (0x" << hex << (Status & 0xff) << dec << ") not found" << FgColor::white << endl;EpcsOk=false;break;
	}
	if(EpcsOk) {
		ifstream f(f_name.toStdString());
		if(!f.is_open()) {
			cerr << FgColor::red << "Error During loading file : " << f_name.toStdString() << endl;
			return (BAD_PROG_NAME);
		}
		ret = EraseEpcs(FrontEnd);
		if(ret != NO_ERROR) {
			f.close();
			return (ret);
		}
		cout << "********************************************" << endl;
		while(f.good())
			Memory.push_back((u8) f.get());
		f.close();
		ret = WritePage(FrontEnd,&Memory);
		if(ret != NO_ERROR) {
			cerr << FgColor::red << "Programming Error" << FgColor::white << endl;
			return (ret);
		}
		cout << "********************************************" << endl;
	} else {
		cerr << FgColor::red << "Can't Identify EPCS Type !" << endl;
		return(BAD_EPCS_TYPE);
	}
	t2 = time(NULL);
	cout << FgColor::green << "Programming duration : " << difftime(t2,t1) << "S" << FgColor::white << endl;
	return (NO_ERROR);

}	// SendProg

//====================================================================
//====================================================================
//====================================================================
//====================================================================
//====================================================================
