//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Asm.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//===============================================
//===============================================
#include <iostream>
#include <fstream>
using namespace std;
#include <QtXml>

#include <unistd.h>

#include "MyDefs.h"
#include "Tools.h"
#include "Asm.h"
#include "command_error.h"

#define cad(var) ((var ^-1) +1)

//=========================================================================
//=========================================================================
CAsm::CAsm(int verbose)
{
	m_verbose = verbose;
	m_channel = 0;
	m_address = 0;
	m_cfg_addr = 0;
	m_epcs_stat_addr = 0;
	p_tcp = NULL;
	p_mbox = NULL;
   m_console=NULL;
   for (int i=0;i<8;++i) VerifCdce[i] = false;
	if(m_verbose >= VERBOSE)
		cout << "Create CAsm Class" << endl;
	for(int i=0; i<NB_ASM; i++)
		memset((void*)&asmdt[i], 0, sizeof(ASMDATA));
}	// Constructor



//=========================================================================
//=========================================================================
CAsm::~CAsm()
{
	if(m_verbose >= VERBOSE)
		cout << "Delete CAsm Class" << endl;
}	// Destructor

//====================================================================
//====================================================================


void CAsm::setConsole(QTextEdit *console)
//==========================================================================
//==========================================================================
{
    m_console = console;
}

void CAsm::Message(int ret,QString message,const QString endmessage)
//==========================================================================
//==========================================================================
{
    if (m_console) {
        if(ret == NO_ERROR) {
            m_console->setTextColor("green");
            m_console->append(message + " => Operation Successfull  " + endmessage);
        } else {
            m_console->setTextColor("red");
            m_console->append(message + " => failed " + endmessage + "Ret = " + QString::number(ret,10));
        }
    }
}



void CAsm::decodeStatus(QLineEdit *Asmstatus,QLineEdit *AsmstatusPll,u32 m_mask,u32 m_chan,const bool m_read)
//================================================================================
//================================================================================
{

    int ret;
    ASMDATA *p_asmdt = GetData(m_chan);
    u16 reg[20];
    reg[17] = p_asmdt->rd.Pll_Lock;
    u32 stat = p_asmdt->rd.Stat_GXB;

    int index = 0;
 //   m_mask = 1 << m_chan;
    if (m_read)
        do {
            Message(ret=ReadCmd(m_mask, m_chan, 18, 0x0e, &reg[0]),"Read Value 0x" + QString::number(reg[0],16));
            stat = reg[1] << 16 | reg[0];
        }  while ((ret != NO_ERROR) && (index++ <4));



    if (Asmstatus) Asmstatus->setText("0x" + QString::number(stat,16));
    QString str = QString("<b>Global Status</b> 0x%1").arg(stat,8, 16, QChar('0'));
    str.append("<ul>");
    if(stat & (1 <<  0)) str.append("<li>Reconfiguration : <font color='red'>Busy</font></li>");   else str.append("<li>Reconfiguration : <font color='green'>Done</font></li>");
    str.append(QString("<li>Rx Bit Slip : 0x%1</li>").arg((stat & 0x3E) >> 1, 2 , 16 ,QChar('0')));
    if(stat & (1 <<  6)) str.append("<li>Transnmit : <font color='red'>Fault</font></li>");        else str.append("<li>Transmit : <font color='green'>Ok</font></li>");
    if(stat & (1 <<  7)) str.append("<li>Receive : <font color='red'>Loss</font></li>");           else str.append("<li>Receive : <font color='green'>Ok</font></li>");
    if(stat & (1 <<  8)) str.append("<li>Module : <font color='red'>Absent</font></li>");          else str.append("<li>Module : <font color='green'>Present</font></li>");
    if(stat & (1 <<  9)) str.append("<li>Synchred : <font color='green'>Yes</font></li>");           else str.append("<li>Synnchred : <font color='red'>No</font></li>");
    str.append(QString("<li>nRx Bit Slip : 0x%1</li>").arg((stat & (0x1F << 10)) >> 10, 2 , 16 ,QChar('0')));
    if(stat & (1 << 15)) str.append("<li>Control 0 : <font color='green'>Detected</font></li>");     else str.append("<li>Control 0 : <font color='red'>Not</font></li>");
    if(stat & (1 << 16)) str.append("<li>Control 1 : <font color='green'>Detected</font></li>");     else str.append("<li>Control 1 : <font color='red'>Not</font></li>");
    if(stat & (1 << 17)) str.append("<li>Rx Frequency : <font color='green'>Locked</font></li>");    else str.append("<li>RXx Freqency : <font color='red'>Unlocked</font></li>");
    if(stat & (1 << 18)) str.append("<li>Synchro 0 : <font color='green'>Detected</font></li>");     else str.append("<li>Synchro 0 : <font color='red'>Not</font></li>");
    if(stat & (1 << 19)) str.append("<li>Synchro 1 : <font color='green'>Detected</font></li>");     else str.append("<li>Synchro 1 : <font color='red'>Not</font></li>");
    str.append(QString("<li>Rx Running Disp : 0x%1</li>").arg((stat & (0x3 << 20)) >> 20, 2 , 16 ,QChar('0')));
    str.append(QString("<li>Rx Disp Error : 0x%1</li>").arg((stat & (0x3 << 22)) >> 22, 2 , 16 ,QChar('0')));
    if(stat & (1 << 24)) str.append("<li>Channel Read : <font color='green'>ON/<li>");        else str.append("<li>Channel Read: <font color='red'>Off</font></li>");
    if(stat & (1 << 25)) str.append("<li>PLL Gx 0 : <font color='green'>Locked</font></li>");           else str.append("<li>PLL Gx 0 : <font color='red'>Unlocked</font></li>");
    if(stat & (1 << 26)) str.append("<li>PLL Gx 1 : <font color='green'>Locked</font></li>");           else str.append("<li>PLL Gx 1 : <font color='red'>Unlocked</font></li>");
    if(stat & (1 << 27)) str.append("<li>Reconfiguration : <font color='red'>Busy</font></li>");   else str.append("<li>Reconfiguration : <font color='green'>Done</font></li>");
    if(stat & (1 << 28)) str.append("<li>Calibration Adc Mezza 0 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(stat & (1 << 29)) str.append("<li>Calibration Adc Mezza 1 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(stat & (1 << 30)) str.append("<li>Calibration Adc Mezza 2 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(stat & (1 << 31)) str.append("<li>Daq Status : <font color='green'>On</font></li>");        else str.append("<li>Daq Status : <font color='red'>Off</font></li>");
    if(stat & 0x70000000) str.append("<li>Error : <font color='green'>Unknown</font></li>");      // else str.append("<li>Error : <font color='red'>No Error</font></li>");
    str.append("</ul>");
    if (Asmstatus) Asmstatus->setToolTip(str);
//    if(stat & (1 << 31)) ui->AsmstatusDaq->setText("Started"); else ui->AsmstatusDaq->setText("Stopped");

    if (AsmstatusPll) AsmstatusPll->setText(QString("0x%1").arg(reg[17],4, 16, QChar('0')));
    str = QString("<b>Pll Locked</b> 0x%1").arg(reg[17],4, 16, QChar('0'));
    str.append("<ul>");

    if(reg[17] & (1 << 0)) str.append("<li>Pll Drs 0			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 0        : <font color='red'>not Locked</font></li>");
    if(reg[17] & (1 << 1)) str.append("<li>Pll Drs 1			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 1        : <font color='red'>not Locked</font></li>");
    if(reg[17] & (1 << 2)) str.append("<li>Pll Drs 2			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 2        : <font color='red'>not Locked</font></li>");
    if(reg[17] & (1 << 3)) str.append("<li>Pll Cdce62005		: <font color='green'>Locked</li>");else str.append("<li>Pll Cdce62005    : <font color='red'>not Locked</font></li>");

    if(reg[17] & (1 << 4)) str.append("<li>Pll 40Mhz to		: <font color='green'>Locked</li>");else str.append("<li>Pll 40Mhz to     : <font color='red'>not Locked</font></li>");
    if(reg[17] & (1 << 5)) str.append("<li>Pll Config (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Config (FPGA): <font color='red'>not Locked</font></li>");
    if(reg[17] & (1 << 6)) str.append("<li>Pll Sample (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Sample (FPGA): <font color='red'>not Locked</font></li>");

    if(reg[17] & (1 << 7)) str.append("<li>Reg0 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg0 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 8)) str.append("<li>Reg1 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg1 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 9)) str.append("<li>Reg2 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg2 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 10)) str.append("<li>Reg3 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg3 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 11)) str.append("<li>Reg4 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg4 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 12)) str.append("<li>Reg5 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg5 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 13)) str.append("<li>Reg6 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg6 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 14)) str.append("<li>Reg7 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg7 Cdce62005: <font color='red'>not ok</font></li>");
    if(reg[17] & (1 << 15)) str.append("<li>Active clock : <font color='red'>c_clk</li>");else str.append("<li>Active Clock : <font color='green'>f_clkadc</font></li>");

    if (AsmstatusPll) AsmstatusPll->setToolTip(str);

}



void CAsm::WriteMsg(QString str)
{
	if(p_mbox) {
		p_mbox->append(str);
    }
}   // WriteMsg
//====================================================================
void CAsm::WriteMsg(QString str, QColor color)
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
u16 CAsm::ComputeParity (u16 us)
{
	u16 parity = 0;
	for(u16 i=0; i<16; i++)
		if(us & (1 << i))
			parity = ~parity;
	return (parity & 0x1);

}	// ComputeParity

//=========================================================================
//=========================================================================
int CAsm:: Dump(u32 mask)
{
	if(m_verbose) {
		cout << "sizeof(ASMDATA)            " << (sizeof(ASMDATA)) << endl;
		cout << "sizeof(ASMDATA.READ_ONLY)  " << (sizeof(asmdt[0].rd)) << endl;
		cout << "sizeof(ASMDATA.READ_WRITE) " << (sizeof(asmdt[0].rw)) << endl;
	}
	SingleMask(mask);
	u16* Dest = (u16*) &asmdt[m_channel];
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::blue
			 << "Asm (Mask 0x" << hex << mask << dec << ") Dump Command" << FgColor::white << CtColor::none << endl;
	return (ReadCmd(mask,BROADCAST_FE, sizeof(ASMDATA)/2 , 0, Dest));

}	// Dump

//=========================================================================
//=========================================================================
void CAsm::DisplayReadWrite(u32 channel)
{
	if(m_verbose == QUIET) return;
	if(channel>= NB_ASM) {
		cerr << FgColor::red << "Asm Dump Command Wrong Asm Number (" << channel << ")" << FgColor::white << endl;
		return;
	}

	cout << endl << "Read Write Part Channel " << channel << endl;
	cout << "Front End Nb          : 0x" << hex << asmdt[channel].rw.FrontEnd << dec << endl;		// Rw_Register : Front End Identifier
	cout << "Acquisition Mode      : 0x" << hex << asmdt[channel].rw.Mode << dec << endl ;			// Rw_Register : Acquisition Mode
	cout << "Configuration Strobe  : 0x" << hex << asmdt[channel].rw.Conf_Strobe << dec << endl;	// Rw_Register	: Configuration Strobe
	for(int j=0; j<3 ; j++ )
	{
		cout << "Dac Cm Channel " << CFormat ("%2d", j*8) << ".." << CFormat("%2d", (j*8)+7) << " : ";
		for(int i=(j*8); i<(j*8)+8; i++)
			cout << CFormat ("0x%08x   ", asmdt[channel].rw.Dac_Cm[i]);						// Rw_Register : Dac Cm Channel 0..7
		cout << endl;
	}
	for(int j=0; j<3; j++)
	{
		cout << "Thres Low:High " << CFormat ("%2d", j*8) << ".." << CFormat("%2d", (j*8)+7) << " : ";
		for(int i=(j*8); i<(j*8)+8; i++)
			cout << CFormat("0x%04x:", asmdt[channel].rw.Threshold[i].Low) << CFormat("0x%04x  ", asmdt[channel].rw.Threshold[i].High);
		cout << endl;
	}
	cout << "Jitter Cleaner 0..8   : ";
	for(int i=0; i<NB_CDCE_REG; i++)
		cout << CFormat("0x%08x   ", asmdt[channel].rw.Jitter_Clean[i]);						// Rw_Register : Jitter Cleaner MSB Register 0..8
	cout << endl;
	cout << "Dac Rofs 0..5         : ";
	for(int i=0; i<ASM_NB_ROFS; i++)
		cout << CFormat("0x%08x   ", asmdt[channel].rw.DacRofs[i]);							// Rw_Register : Dac Rofs 0..5
	cout << endl;
	cout << "Number of Samples     : " << asmdt[channel].rw.NbSamples << endl;					// Rw_Register : Number of Samples
	cout << "Channel to Read       : " << asmdt[channel].rw.ChannelRead << endl;				// Rw_Register : Channel to Read
	cout << "Width & Delay H State : 0x" << hex << asmdt[channel].rw.Width_Delay << dec << endl;				// Rw_Register	: Width & Delay High State
	cout << "Filter Thres H State  : 0x" << hex << asmdt[channel].rw.Filter_Thr_H << dec << endl;				// Rw_Register	: Filter Threshold High State
	cout << "Filter Thres L State  : 0x" << hex << asmdt[channel].rw.Filter_Thr_L << dec << endl;				// Rw_Register	: Filter Threshold Low State
	cout << "Thor TimeStamp Offset : 0x" << hex << asmdt[channel].rw.Blonde_Offset << dec <<endl;				// Rw_Register : Blonde TimeStamp Offset

//	cout << endl << endl;

} // DisplayReadWrite

//=========================================================================
//=========================================================================
int CAsm::SingleMask(u32 mask)
{
	int ret = -1;
	for(int i= 0; i<32; i++) {
		if((mask >> i) & 0x1) {
			m_channel = i;
			ret ++;
		}
	}
	if(m_verbose > VERBOSE)
		cout << "Mask (0x" << hex << mask << dec << ") : " << ret << endl;
	return (ret);

}	// SingleMask

//=========================================================================
//=========================================================================
int CAsm::ReadCmd(u32 Mask, u16 FrontEnd, u16 NbToRead, u16 FirstAddr, u16* Dest, bool Connected )
{
	QString title = QString("ASM Read Command");
	if((Mask < 1) || (Mask>0xfff))
	{
		m_str = QString("Asm Read Command Wrong Asm Mask (0x") + QString::number(Mask,16) + QString(")");
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
	if(SingleMask(Mask))
	{
		m_str = QString("Asm Read Command Invalid for Multi Channel Mode");
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << "Asm Read Command Invalid for Multi Channel Mode" << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
	if(m_verbose) {
		cout << CtColor::underscore << CtColor::reverse << FgColor::blue
			 << "Asm (Mask 0x" << hex << Mask << dec << ") Read Command" << FgColor::white << CtColor::none << endl;
		cout << "Asm ReadCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd
			 << ") NbToRead (0x" << NbToRead
			 << ") Fist Addr (0x" << FirstAddr
			 << ")" << dec << endl;
	}
	u16 cmd = K28_2  | ((FrontEnd & 0x7F) << 8);
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;
	u16 crc = 0;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
	p_tcp->frm_cmd.nbarg = 4;							// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = NbToRead;
	p_tcp->frm_cmd.arg16[2] = FirstAddr;
	crc ^= p_tcp->frm_cmd.arg16[1];
	crc ^= p_tcp->frm_cmd.arg16[2];
	p_tcp->frm_cmd.arg16[3] = crc;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, NbToRead + 4);
	cout << "Apres manage" << endl;
	if(ret == NO_ERROR)
//        ret = ReadReply(m_channel, FrontEnd, NbToRead, Dest);
	   ret = ReadReply(m_channel, Dest);
	return (ret);

}	// ReadCmd

//=========================================================================
//=========================================================================
int CAsm::Manage(int man, int nbarg)
{
	QString err;
	QString title = QString("ASM Manage");
	if(man !=-1) {
		if(p_tcp->frm_cmd.indic == p_tcp->frm_rep.indic) {
			if(m_verbose) {
				cout << "Reply->Stat : " << p_tcp->frm_rep.stat << endl;
				cout << "Reply->NbArg: " << p_tcp->frm_rep.nbarg << endl;
			}
			if(p_tcp->frm_rep.stat == 0) {
				if (p_tcp->frm_rep.nbarg == nbarg) {
					return (NO_ERROR);
				} else if (p_tcp->frm_rep.nbarg == 1) {
					return (ReadAck(1));
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

//=========================================================================
//=========================================================================
//int CAsm::ReadReply(u32 channel, u16 FrontEnd,u16 NbToread, u16* StartAddr)
int CAsm::ReadReply(u32 channel, u16* StartAddr)
{
	u16 crc = 0;
	u16 *vv;
	QString title = QString("ASM Read Reply");
	if((p_tcp->frm_rep.nbarg == 1) && ((p_tcp->frm_rep.arg16[0] & 0xFF) == K28_0))
	{
		int ret = p_tcp->frm_rep.arg16[0] & 0xf;
		m_str = QString("Problem inside ASM (%1) acknowledge (%2)").arg(channel).arg(ret);
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (ret);
	} else if ((p_tcp->frm_rep.nbarg >= 3 ) && ((p_tcp->frm_rep.arg16[0] & 0xFF) == K28_1)) {
		u16* addr = (u16*) &asmdt;
		addr += p_tcp->frm_rep.arg16[2];
		u16* src = (u16*)&(p_tcp->frm_rep.arg16[3]);
		size_t siz = p_tcp->frm_rep.arg16[1] * sizeof(unsigned short);
		if(m_verbose > VERBOSE) cout << "Read Data " << addr << " " << src << " " << siz << endl;
		vv = src;
		crc = p_tcp->frm_rep.arg16[1] ^ p_tcp->frm_rep.arg16[2];
		for(u32 i=0; i<p_tcp->frm_rep.arg16[1]; i++){
			crc ^= *vv;
//			cout << i << " " << hex << crc << " " << *vv << endl;
			vv++;
		}
		if( crc== *vv) {
			if(m_verbose)
				cout << FgColor::green << "Good CRC Calcul 0x" << hex << crc << " Send 0x" << *vv << dec << FgColor::white << endl;
			memcpy(StartAddr, src, siz);
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
	return 0;

}	// ReadReply

//=========================================================================
//=========================================================================
int  CAsm::WriteCmd   (u32 Mask, u16 FrontEnd, u16 NbToWrite, u16 StartAddr, u16* val, bool Connected)
{
	int i=0;
	QString title = QString("ASM Write Command");
	if((Mask < 1) || (Mask>0xfff))
	{
		m_str = QString("Asm Write Command Wrong Asm Mask (0x") + QString::number(Mask,16) + QString(")");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
	int nb = SingleMask(Mask) +1;
	if(m_verbose)
		cout << "Asm WriteCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd
			 << ") NbToWrite (0x" << NbToWrite
			 << ") Fist Addr (0x" << StartAddr
			 << ") Value (0x" << *val << dec
			 << ") NbofChannel (" << nb << ")" << endl;


	u16 cmd = (nb==1)? (K28_1 | ((FrontEnd & 0x007F) <<8)) : (K28_1 | (BROADCAST_FE << 8));
	u16 parity = ComputeParity(cmd)<< 15;
	cmd |= parity;
	u16 crc = 0;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;						// Export Command
	p_tcp->frm_cmd.nbarg = 4 + NbToWrite;					// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;								// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = NbToWrite;
	p_tcp->frm_cmd.arg16[2] = StartAddr;
	crc ^= p_tcp->frm_cmd.arg16[1];
	crc ^= p_tcp->frm_cmd.arg16[2];
	for(i=0; i<NbToWrite; i++) {
		p_tcp->frm_cmd.arg16[i+3] = *val;
		crc ^= p_tcp->frm_cmd.arg16[i+3];
		val++;
	}
	p_tcp->frm_cmd.arg16[i+3] = crc;

//	cout << "Connected  :" << Connected << endl;
 //   if (!p_tcp->IsConnected()) p_tcp->Connect();
	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, nb);
//	cout << "Manage " << ret << " " << nb << endl;
	if(ret == NO_ERROR) {
		ret = ReadAck(nb);
	}
	return (ret);

}	// WriteCmd

//=========================================================================
//=========================================================================
int CAsm::ReadAckProg(int nback,bool *Err)
{
    u16 ack;
    int ret = NO_ERROR;
    QString title = QString("Asm Read Acknowledge");
    for(int i=0; i<nback; i++) {
        Err[i] = false;
//		cout << i<< " ack " << hex << p_tcp->frm_rep.arg16[i] << dec << endl;
        if((p_tcp->frm_rep.arg16[i] & 0xff)!= K28_0) {
            if((p_tcp-> frm_rep.arg16[i] & 0xff) == DCS_TMO_ERROR) {
                ret = BAD_ACKNOWLEDGE;
                m_str = (QString("DCS_TMO_ERROR on ack %1 (0x").arg(i)) + QString::number(p_tcp->frm_rep.arg16[i] & 0xff,16) + QString(")");
                WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
                if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
            } else {
                ret = BAD_ACKNOWLEDGE;
                m_str = (QString("Bad Acknowledge on ack %1 (0x").arg(i)) + QString::number(p_tcp->frm_rep.arg16[i] & 0xff,16) + QString(")");
                WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
                if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
            }
          Err[i] = true;
        } else {
            ack = (p_tcp->frm_rep.arg16[i] >> 8) & 0x7f;
            switch (ack) {
                case 0                : ret = NO_ERROR;       m_str = (QString("Good Acknowledge on ack %1").arg(i)); break;
                case (1 << PARITYERR) : ret = BAD_ACK_PARITY; m_str = (QString("Bad Parity Acknowledge on ack %1 (0x").arg(i)) + QString::number(ack,16) + (QString(")")); break;
                case (1 << LENERR)    : ret = BAD_ACK_LEN;    m_str = (QString("Bad Len Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
                case (1 << CRCERR)    : ret = BAD_ACK_CRC;    m_str = (QString("Bad Len Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
                case (1 << ENDERR)    : ret = BAD_ACK_END;    m_str = (QString("Bad End Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
                case (1 << STARTERR)  : ret = BAD_ACK_START;  m_str = (QString("Bad Start Acknowledge on ack %1 (0x").arg(i))  + QString::number(ack,16) + (QString(")")); break;
                case (1 << FENOERR)   : ret = BAD_ACK_FENO;   m_str = (QString("Bad FE Nb Acknowledge on ack %1 (0x").arg(i))  + QString::number(ack,16) + (QString(")")); break;
                case (1 << EPCSEERR)  : ret = BAD_ACK_EPCS;   m_str = (QString("Bad EPCS Acknowledge on ack %1 (0x").arg(i))   + QString::number(ack,16) + (QString(")")); break;
                default               : ret = BAD_ACK_BIZZ;   m_str = (QString("Unknown Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
            }
            if(ack) {
                WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
                if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white <<endl;
            } else {
                WriteMsg(m_str,"green");
                if(m_verbose) cout << FgColor::green << m_str.toStdString() << FgColor::white <<endl;
            }
        }
    }
    return (ret);

}	// ReadAckProg




//=========================================================================
//=========================================================================
int CAsm::ReadAck(int nback)
{
	u16 ack;
	int ret = NO_ERROR;
	QString title = QString("Asm Read Acknowledge");
	for(int i=0; i<nback; i++) {
//		cout << i<< " ack " << hex << p_tcp->frm_rep.arg16[i] << dec << endl;
		if((p_tcp->frm_rep.arg16[i] & 0xff)!= K28_0) {
			if((p_tcp-> frm_rep.arg16[i] & 0xff) == DCS_TMO_ERROR) {
				ret = BAD_ACKNOWLEDGE;
				m_str = (QString("DCS_TMO_ERROR on ack %1 (0x").arg(i)) + QString::number(p_tcp->frm_rep.arg16[i] & 0xff,16) + QString(")");
				WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
				if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
			} else {
				ret = BAD_ACKNOWLEDGE;
				m_str = (QString("Bad Acknowledge on ack %1 (0x").arg(i)) + QString::number(p_tcp->frm_rep.arg16[i] & 0xff,16) + QString(")");
				WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
				if(m_verbose) cerr << FgColor::red  << m_str.toStdString() << FgColor::white << endl;
			}
		} else {
			ack = (p_tcp->frm_rep.arg16[i] >> 8) & 0x7f;
			switch (ack) {
				case 0                : ret = NO_ERROR;       m_str = (QString("Good Acknowledge on ack %1").arg(i)); break;
				case (1 << PARITYERR) : ret = BAD_ACK_PARITY; m_str = (QString("Bad Parity Acknowledge on ack %1 (0x").arg(i)) + QString::number(ack,16) + (QString(")")); break;
				case (1 << LENERR)    : ret = BAD_ACK_LEN;    m_str = (QString("Bad Len Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
				case (1 << CRCERR)    : ret = BAD_ACK_CRC;    m_str = (QString("Bad Len Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
				case (1 << ENDERR)    : ret = BAD_ACK_END;    m_str = (QString("Bad End Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
				case (1 << STARTERR)  : ret = BAD_ACK_START;  m_str = (QString("Bad Start Acknowledge on ack %1 (0x").arg(i))  + QString::number(ack,16) + (QString(")")); break;
				case (1 << FENOERR)   : ret = BAD_ACK_FENO;   m_str = (QString("Bad FE Nb Acknowledge on ack %1 (0x").arg(i))  + QString::number(ack,16) + (QString(")")); break;
				case (1 << EPCSEERR)  : ret = BAD_ACK_EPCS;   m_str = (QString("Bad EPCS Acknowledge on ack %1 (0x").arg(i))   + QString::number(ack,16) + (QString(")")); break;
                case  0x41            : ret = BAD_ACK_EPCS;   m_str = (QString("Error Protect key Erase on ack %1 (0x").arg(i))   + QString::number(ack,16) + (QString(")")); break;
            default                   : ret = BAD_ACK_BIZZ;   m_str = (QString("Unknown Acknowledge on ack %1 (0x").arg(i))    + QString::number(ack,16) + (QString(")")); break;
			}
			if(ack) {
				WriteMsg(m_str,"red");
//				TimerMessage(title, m_str);
				if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white <<endl;
			} else {
				WriteMsg(m_str,"green");
				if(m_verbose) cout << FgColor::green << m_str.toStdString() << FgColor::white <<endl;
			}
		}
	}
	return (ret);

}	// ReadAck

//=========================================================================
//=========================================================================
int CAsm::ResetCmd(u32 Mask, u16 FrontEnd, bool Connected)
{
    if((Mask < 1) || (Mask>0xfff))  //0xfff
	{
		QString title = QString("ASM Reste Command");
		m_str = (QString("Asm Reset Command Wrong Asm Number (0x")) + QString::number(Mask,16) + (QString(")"));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
//	Mask = Mask & m_usemask;
	if(m_verbose)
		cout << "Asm ResetCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd 			 << ")" << dec << endl;

	int nb = SingleMask(Mask) +1;

	u16 cmd = (nb==1)? (K28_3 | ((FrontEnd & 0x00FE) <<8)) : (K28_3 | (BROADCAST_FE << 8));
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_SILENT_EXPORT;			// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = CMDRST;

	int ret = (Connected) ? p_tcp->Exchange(false) :  p_tcp->Connect_Exchange(false) ;
	return (ret);

}	// Reset



//=========================================================================
//=========================================================================
int CAsm::RunCmd(u32 Mask, u16 FrontEnd, u16 Run, bool Connected)
{
	if((Mask < 1) || (Mask>0xfff))
	{
		QString title = QString("ASM Run Command");
		m_str = (QString("Asm Run Command Wrong Asm Number (0x")) + QString::number(Mask,16) + (QString(")"));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
//	Mask = Mask & m_usemask;
	int nb = SingleMask(Mask) +1;
	if(m_verbose)
		cout << "Asm RunCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd << dec
			 << ") Run (" << Run << ") Nb (" << nb << ")" << endl;

	u16 cmd = (nb==1)? (K28_3 | ((FrontEnd & 0x007F) <<8)) : (K28_3 | (BROADCAST_FE << 8));
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
    if(Run== 1)
		p_tcp->frm_cmd.arg16[1] = CMDSTARTRUN;
	else
		p_tcp->frm_cmd.arg16[1] = CMDSTOPRUN;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, nb);
	if(ret == NO_ERROR) {
        ret = ReadAck(nb);
	}
	return (ret);

}	// RunCmd

//=========================================================================
//=========================================================================
int CAsm::RunCmdMulti(u32 Mask, u16 FrontEnd, u16 Run,bool *Err, bool Connected)
{
    if((Mask < 1) || (Mask>0xfff))
    {
        QString title = QString("ASM Run Command");
        m_str = (QString("Asm Run Command Wrong Asm Number (0x")) + QString::number(Mask,16) + (QString(")"));
        WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
        if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
        return(BAD_ASM_NB);
    }
//	Mask = Mask & m_usemask;
    int nb = SingleMask(Mask) +1;
    if(m_verbose)
        cout << "Asm RunCmd : mask (0x" << hex << Mask << dec
             << ") Channel (" << m_channel
             << ") FrontEnd (0x" << hex << FrontEnd << dec
             << ") Run (" << Run << ") Nb (" << nb << ")" << endl;

    u16 cmd = (nb==1)? (K28_3 | ((FrontEnd & 0x007F) <<8)) : (K28_3 | (BROADCAST_FE << 8));
    u16 parity = ComputeParity(cmd) << 15;
    cmd |= parity;

    p_tcp->ClearBuf();
    p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
    p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
    p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
    p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
    p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
    p_tcp->frm_cmd.arg16[0] = cmd;
    if(Run== 1)
        p_tcp->frm_cmd.arg16[1] = CMDSTARTRUN;
    else
        p_tcp->frm_cmd.arg16[1] = CMDSTOPRUN;

    int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
    ret= Manage(ret, nb);
    if(ret == NO_ERROR) {
        ret = ReadAckProg(nb,Err);
    }
    return (ret);

}	// RunCmd


//=========================================================================
//=========================================================================
int CAsm::ChangCmd(u32 Mask, u16 FrontEnd, bool Connected)
{
	if((Mask < 1) || (Mask>0xfff))
	{
		QString title = QString("ASM Change Command");
		m_str = (QString("Asm Change Command Wrong Asm Number (0x")) + QString::number(Mask,16) + (QString(")"));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
//	Mask = Mask & m_usemask;
	if(m_verbose)
		cout << "Asm ChangCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd << dec
			 << ")" << endl;

	int nb = SingleMask(Mask) +1;

	u16 cmd = (nb==1)? (K28_3 | ((FrontEnd & 0x007F) <<8)) : (K28_3 | (BROADCAST_FE << 8));
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
    p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export CommandCalibCmd
	p_tcp->frm_cmd.nbarg = 2;							// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
	p_tcp->frm_cmd.arg16[1] = CMDCHANGE;

	int ret = (Connected) ? p_tcp->Exchange() :  p_tcp->Connect_Exchange() ;
	ret= Manage(ret, nb);
	if(ret == NO_ERROR) {
		ret = ReadAck(nb);
	}
	return (ret);

}	// ChangCmd


int CAsm::ConfigCdce62005(u32 Mask, u16 FrontEnd, u16 addrReg,u16 mchannel, const bool connected)
//=========================================================================
//=========================================================================
{
  int nbtry = 0;
  int ret = NO_ERROR;
  qDebug() << "Config Cdce62005" ;
  if (!p_tcp->IsConnected()) ret = p_tcp->Connect();
  if(ret == NO_ERROR) {
    cout << FgColor::yellow << " Command Config  0x" << hex << conf[WRITECDCE] << dec << FgColor::white <<endl;
    Message(ret = WriteCmd(Mask, FrontEnd, 1, 0x95 , &addrReg, true),"RegCdce "+ QString::number(addrReg,16));
    Message(ret = WriteCmd(Mask, FrontEnd, 2, m_cfg_addr , (u16 *) &conf[WRITECDCE], true),"Write cmd InitCdce");

    ret = ChangCmd(Mask, FrontEnd, true);

    cout << FgColor::yellow << " Command Config  0x" << hex << conf[READCDCE] << dec << FgColor::white <<endl;
    Message(ret = WriteCmd(Mask, FrontEnd, 1, 0x95 , &addrReg, true),"RegCdce "+ QString::number(addrReg,16));
    Message(ret = WriteCmd(Mask, FrontEnd, 2, m_cfg_addr , (u16 *) &conf[READCDCE], true),"Read Cdce");

    ret = ChangCmd(Mask, FrontEnd, true);

    u16 reg[4];
    // Il faut lire deux fois parce que la première lecture n'est pas bonne ==> Probleme ASM
    Message(ret=ReadCmd(Mask, FrontEnd, 3, 0x13, &reg[0],true),"Read Value 0x" + QString::number(reg[0],16));
    Message(ret=ReadCmd(Mask, FrontEnd, 3, 0x13, &reg[0],true),"Read Value 0x" + QString::number(reg[0],16));
    u32 test = reg[1] << 16 | reg[2];
    if (test == asmdt[m_channel].rw.Jitter_Clean[addrReg]) VerifCdce[addrReg] = true;
    else VerifCdce[addrReg] = false;
   }
   if (p_tcp->IsConnected() && !connected) p_tcp->Disconnect();
   return(ret);
}

//=========================================================================
//=========================================================================
int CAsm::ConfigCmd(u32 Mask, u16 FrontEnd, QString f_name, bool DoIt)
{
	u32 nbtry;

	QString title = QString("ASM Config Command");

	if((Mask < 1) || (Mask>0xfff))
	{
		m_str = (QString("Asm Config Command Wrong Asm Number (0x")) + QString::number(Mask,16) + (QString(")"));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
	if(SingleMask(Mask))
	{
		m_str = QString("Asm Config Command Invalid for Multi Channel Mode");
		WriteMsg(m_str,"red");
//		TimerMessage( title, m_str);
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(BAD_ASM_NB);
	}

	if(f_name.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			f_name = QString("%1/ConfigDcs/RegAsm_%2.xml").arg(dirdcs).arg(m_channel);
		} else {
#ifdef Q_OS_OSX
			f_name =  QString("../../../../config/RegAsm_%1.xml").arg(m_channel);
#else
			f_name =  QString("../../ConfigDcs/RegAsm_%1.xml").arg(m_channel);
#endif
		}
	}

	if(m_verbose) {
		cout << "f_name " << f_name.toStdString() << endl;
		cout << CtColor::underscore << CtColor::reverse << FgColor::blue
			 << "Asm ConfigCmd : FrontEnd (0x" << hex << FrontEnd << dec << ")  Channel (" << m_channel << ") : " << f_name.toStdString()
			 << FgColor::white << CtColor::none << endl;
	}
	int ret = ParseXmlReg(m_channel, f_name);
	if (ret == NO_ERROR) { //XML_SUCCESS) {
		DisplayReadWrite(m_channel);
//		cout << "Asm Configuration" << endl;
		ret = p_tcp->Connect();
		if(ret == NO_ERROR) {
			ret = WriteCmd(Mask, FrontEnd, sizeof(asmdt[m_channel].rw) /2, m_address, &(asmdt[m_channel].rw.FrontEnd), true);
//			cout <<  "Config WriteCmd Ret " << ret << endl << endl;
			if(ret == NO_ERROR) {
				if(DoIt) {
//					for(int i=0; i<=ResetFifo240M; i++) {
                    for (int j=0;j<8;j++) ConfigCdce62005(Mask,FrontEnd, j,m_channel,true);
                    for(int i=InitCm1; i<=InitDrs; i++) {   //Ajout calibrartion adc
 //                     if (i != ResetDrs) {
						nbtry = 0;
						do {
							cout << FgColor::yellow << i << " Command Config  0x" << hex << conf[i] << dec << FgColor::white <<endl;
							ret = WriteCmd(Mask, FrontEnd, 2, m_cfg_addr/*ASM_CONF_STROBE_ADDR*/ , (u16 *) &conf[i], true);
							if(ret != NO_ERROR) {
								nbtry++;
								if( nbtry >=3) {
									p_tcp->Disconnect();
									return (ret);
								}
							}
						} while (ret != NO_ERROR);
						nbtry = 0;
						do {
							ret = ChangCmd(Mask, FrontEnd, true);
							if(ret != NO_ERROR) {
								nbtry++;
								if( nbtry >=3) {
									p_tcp->Disconnect();
									return (ret);
								}
							}
						} while (ret != NO_ERROR);
//					}
                  }

				}
				if((ret == NO_ERROR) && (m_verbose))
					cout << FgColor::green << "Command executed" << FgColor::white << endl;
			}
		}
		p_tcp->Disconnect();
	}
	return (ret);

}	// Config



//=========================================================================
//=========================================================================
int CAsm::CalibCmd(u32 Mask, u16 FrontEnd,uint8_t TypeCalib)
{
	qInfo() << "Calibcmd***********************";
//	int Typecal=0;
	int ret = p_tcp->Connect();
	if(ret == NO_ERROR) {
		qInfo() << "Calibcmd*****tcp connected ******************";

			int nbtry = 0;
			do {
                qInfo() << FgColor::yellow << TypeCalib << "Command Config  0x" << hex << conf[TypeCalib] << dec << FgColor::white <<endl;
                ret = WriteCmd(Mask, FrontEnd, 2, m_cfg_addr , (u16 *) &conf[TypeCalib], true);
				if(ret != NO_ERROR) {
				nbtry++;
					if( nbtry >=3) {
						p_tcp->Disconnect();
						return (ret);
					}
				}
			} while (ret != NO_ERROR);
			nbtry = 0;
			do {
				ret = ChangCmd(Mask, FrontEnd, true);
				if(ret != NO_ERROR) {
					nbtry++;
					if( nbtry >=3) {
						p_tcp->Disconnect();
						return (ret);
					}
				}
			} while (ret != NO_ERROR);
	}
	if((ret == NO_ERROR) && (m_verbose))
		cout << FgColor::green << "Command executed" << FgColor::white << endl;
	p_tcp->Disconnect();
    return(ret);
	
}


//====================================================================
//====================================================================
int CAsm::ParseXmlReg(int channel, QString f_name)
{
	QString title = "ASM Parse XML";
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
	if(m_verbose) cout << "ASM ParseXML SetContent :" << ret << endl;

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
			if(child.attribute("TITLE") == "Conf_Strobe") m_cfg_addr = child.attribute("ADDRESS").toUInt(&ret,16);

			if     (child.attribute("TITLE") == "FrontEnd")          asmdt[channel].rw.FrontEnd = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Mode")               asmdt[channel].rw.Mode = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Conf_Strobe") asmdt[channel].rw.Conf_Strobe = child.attribute("VALUE").toUInt(&ret,16);

			else if(child.attribute("TITLE") == "Dac_Cm_0")       asmdt[channel].rw.Dac_Cm[0] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_1")       asmdt[channel].rw.Dac_Cm[1] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_2")       asmdt[channel].rw.Dac_Cm[2] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_3")       asmdt[channel].rw.Dac_Cm[3] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_4")       asmdt[channel].rw.Dac_Cm[4] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_5")       asmdt[channel].rw.Dac_Cm[5] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_6")       asmdt[channel].rw.Dac_Cm[6] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_7")       asmdt[channel].rw.Dac_Cm[7] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_8")       asmdt[channel].rw.Dac_Cm[8] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_9")       asmdt[channel].rw.Dac_Cm[9] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_10")     asmdt[channel].rw.Dac_Cm[10] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_11")     asmdt[channel].rw.Dac_Cm[11] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_12")     asmdt[channel].rw.Dac_Cm[12] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_13")     asmdt[channel].rw.Dac_Cm[13] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_14")     asmdt[channel].rw.Dac_Cm[14] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_15")     asmdt[channel].rw.Dac_Cm[15] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_16")     asmdt[channel].rw.Dac_Cm[16] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_17")     asmdt[channel].rw.Dac_Cm[17] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_18")     asmdt[channel].rw.Dac_Cm[18] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_19")     asmdt[channel].rw.Dac_Cm[19] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_20")     asmdt[channel].rw.Dac_Cm[20] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_21")     asmdt[channel].rw.Dac_Cm[21] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_22")     asmdt[channel].rw.Dac_Cm[22] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Dac_Cm_23")     asmdt[channel].rw.Dac_Cm[23] = child.attribute("VALUE").toUInt(&ret,16);

			else if(child.attribute("TITLE") == "Threshold_low_0")     asmdt[channel].rw.Threshold[0].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_0")    asmdt[channel].rw.Threshold[0].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_1")     asmdt[channel].rw.Threshold[1].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_1")    asmdt[channel].rw.Threshold[1].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_2")     asmdt[channel].rw.Threshold[2].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_2")    asmdt[channel].rw.Threshold[2].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_3")     asmdt[channel].rw.Threshold[3].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_3")    asmdt[channel].rw.Threshold[3].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_4")     asmdt[channel].rw.Threshold[4].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_4")    asmdt[channel].rw.Threshold[4].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_5")     asmdt[channel].rw.Threshold[5].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_5")    asmdt[channel].rw.Threshold[5].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_6")     asmdt[channel].rw.Threshold[6].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_6")    asmdt[channel].rw.Threshold[6].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_7")     asmdt[channel].rw.Threshold[7].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_7")    asmdt[channel].rw.Threshold[7].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_8")     asmdt[channel].rw.Threshold[8].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_8")    asmdt[channel].rw.Threshold[8].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_9")     asmdt[channel].rw.Threshold[9].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_9")    asmdt[channel].rw.Threshold[9].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_10")    asmdt[channel].rw.Threshold[10].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_10")   asmdt[channel].rw.Threshold[10].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_11")    asmdt[channel].rw.Threshold[11].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_11")   asmdt[channel].rw.Threshold[11].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_12")    asmdt[channel].rw.Threshold[12].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_12")   asmdt[channel].rw.Threshold[12].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_13")    asmdt[channel].rw.Threshold[13].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_13")   asmdt[channel].rw.Threshold[13].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_14")    asmdt[channel].rw.Threshold[14].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_14")   asmdt[channel].rw.Threshold[14].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_15")    asmdt[channel].rw.Threshold[15].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_15")   asmdt[channel].rw.Threshold[15].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_16")    asmdt[channel].rw.Threshold[16].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_16")   asmdt[channel].rw.Threshold[16].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_17")    asmdt[channel].rw.Threshold[17].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_17")   asmdt[channel].rw.Threshold[17].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_18")    asmdt[channel].rw.Threshold[18].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_18")   asmdt[channel].rw.Threshold[18].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_19")    asmdt[channel].rw.Threshold[19].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_19")   asmdt[channel].rw.Threshold[19].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_20")    asmdt[channel].rw.Threshold[20].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_20")   asmdt[channel].rw.Threshold[20].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_21")    asmdt[channel].rw.Threshold[21].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_21")   asmdt[channel].rw.Threshold[21].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_22")    asmdt[channel].rw.Threshold[22].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_22")   asmdt[channel].rw.Threshold[22].High = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_low_23")    asmdt[channel].rw.Threshold[23].Low = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Threshold_High_23")   asmdt[channel].rw.Threshold[23].High = child.attribute("VALUE").toUInt(&ret,16);

			else if(child.attribute("TITLE") == "Jitter_Clean_0")     asmdt[channel].rw.Jitter_Clean[0] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_1")     asmdt[channel].rw.Jitter_Clean[1] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_2")     asmdt[channel].rw.Jitter_Clean[2] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_3")     asmdt[channel].rw.Jitter_Clean[3] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_4")     asmdt[channel].rw.Jitter_Clean[4] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_5")     asmdt[channel].rw.Jitter_Clean[5] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_6")     asmdt[channel].rw.Jitter_Clean[6] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_7")     asmdt[channel].rw.Jitter_Clean[7] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Jitter_Clean_8")     asmdt[channel].rw.Jitter_Clean[8] = child.attribute("VALUE").toUInt(&ret,16);

			else if(child.attribute("TITLE") == "DacRofs_0")          asmdt[channel].rw.DacRofs[0] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "DacRofs_1")          asmdt[channel].rw.DacRofs[1] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "DacRofs_2")          asmdt[channel].rw.DacRofs[2] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "DacRofs_3")          asmdt[channel].rw.DacRofs[3] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "DacRofs_4")          asmdt[channel].rw.DacRofs[4] = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "DacRofs_5")          asmdt[channel].rw.DacRofs[5] = child.attribute("VALUE").toUInt(&ret,16);

			else if(child.attribute("TITLE") == "NbSamples")          asmdt[channel].rw.NbSamples = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "ChannelRead")        asmdt[channel].rw.ChannelRead = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Width_Delay_High")   asmdt[channel].rw.Width_Delay = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Filter_Thr_H")       asmdt[channel].rw.Filter_Thr_H = child.attribute("VALUE").toUInt(&ret,16);
			else if(child.attribute("TITLE") == "Filter_Thr_L")       asmdt[channel].rw.Filter_Thr_L = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "Thor_Offset")        asmdt[channel].rw.Blonde_Offset = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "Pattern")            asmdt[channel].rw.Pattern = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "ShiftClockData")     asmdt[channel].rw.ShiftClockData = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "Choice_shift_clock_pllAdc_FPGA")  asmdt[channel].rw.ShiftPllAdc = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "FREQPIED")           asmdt[channel].rw.FreqPied  = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "SPARE2")             asmdt[channel].rw.RegSpare2 = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "SPARE3")             asmdt[channel].rw.RegSpare3 = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "SPARE4")             asmdt[channel].rw.RegSpare4 = child.attribute("VALUE").toUInt(&ret,16);
            else if(child.attribute("TITLE") == "SPARE5")             asmdt[channel].rw.RegSpare5 = child.attribute("VALUE").toUInt(&ret,16);
			child = child.nextSiblingElement();
		}

	} else {
		m_str = QString("Error while Parsing File : ") + f_name + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << m_str.toStdString() << endl;
		return(errno);
	}
	return 0;
} // ParsXmlReg
//====================================================================
//====================================================================
//=========================================================================
//=========================================================================
int CAsm::EraseEpcs(u32 Mask,u16 FrontEnd, bool Confirmation)
{
    u16 Status[2];

	if((Mask < 1) || (Mask>0xfff))
	{
		WriteMsg("Asm Erase EPCS Command Wrong Asm Number (0x" + QString::number(Mask,16) + ")");
		WriteMsg(m_str,"red");
		if(m_verbose) cerr << FgColor::red << "Asm Erase EPCS Command Wrong Asm Number (0x" << hex << Mask << dec << ")" << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
//	Mask = Mask & m_usemask;
	if(m_verbose)
		cout << "Asm ERaseCmd : mask (0x" << hex << Mask << dec
			 << ") Channel (" << m_channel
			 << ") FrontEnd (0x" << hex << FrontEnd << dec << ")" << endl;

	int nb = SingleMask(Mask) +1;

	u16 cmd =(K28_3 | ((FrontEnd & 0x007F) <<8));
	u16 parity = ComputeParity(cmd) << 15;
	cmd |= parity;


	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_EXPORT;						// Export Command
    p_tcp->frm_cmd.nbarg = 3;									// Number of Arguments
	p_tcp->frm_cmd.mask  = Mask;								// Destination Mask
	p_tcp->frm_cmd.arg16[0] = cmd;
    if (Confirmation) {
        p_tcp->frm_cmd.arg16[1] =  CMDERASEEPCSCONF;
        p_tcp->frm_cmd.arg16[2] =  RandomErase;
    }
    else
    {
        p_tcp->frm_cmd.arg16[1] =  CMDERASEEPCS;
        RandomErase = rand() % 65535;
        p_tcp->frm_cmd.arg16[2] =  RandomErase;
        WriteMsg("Asm Key protect (0x" + QString::number(RandomErase,16) + ")");
        RandomErase = cad(RandomErase);

    }
	int ret = Manage(p_tcp->Connect_Exchange(), nb);
	if(ret == NO_ERROR) {
		ret = ReadAck(nb);
	} else {
		WriteMsg("Can't Erase EPCS !", "red");
		if (m_verbose) cerr << FgColor::red << "Can't Erase EPCS !" << FgColor::white << endl;
		return (ret);
	}
	usleep(1000);
    ret = WaitEpcsBusy (Mask,FrontEnd,&Status[0], 10,100000);
    WriteMsg("Erase Completed-> Status : 0x " + QString::number(Status[1],16));
 //   cout << " Status : 0x" << hex << Status << dec << endl;
	return (ret);

}	// EraseEpcs

//=========================================================================
//=========================================================================

int CAsm::StatusEpcs(u32 Mask,u16 FrontEnd, u16* Status, bool Connected)
{
   int ret = ReadCmd(Mask, FrontEnd, 2, ASM_EPCS_STATUS_ADDR, (u16*) Status, Connected);
   if(m_verbose)
     cout << "Epcs Status 0x" << hex << Status[1] << dec << endl;
   return(ret);
}	// StatusEpcs

//=========================================================================
//=========================================================================
int CAsm::WaitEpcsBusy (u32 Mask,u16 FrontEnd,u16* Status, u16 nbtry,u32 TimeWait)
{
	int verb = m_verbose;
    u16 ess[NB_ASM]  = {nbtry,nbtry,nbtry,nbtry,nbtry,nbtry,
                        nbtry,nbtry,nbtry,nbtry,nbtry,nbtry};
	int ret;
	int bar = 0;
    bool BusyEpcs[NB_ASM] = {true,true,true,true,true,true,
                             true,true,true,true,true,true};
    bool EndWait=false;

	m_verbose = 0;
	if(p_bar) {
		p_bar->setValue(bar);
		p_bar->setMaximum(1000);
	}

	ret = p_tcp->Connect();
	if(ret == NO_ERROR) {
		do {
            usleep(TimeWait);
            EndWait = false;
            if(p_bar) p_bar->setValue(bar++);
            if (bar == 1000) bar = 0;
            for (int i=0;i<NB_ASM;++i) {
              if(((Mask >> i) & 0x1) == 0x1 ) {
                u32 Mask1 = (1 << i);
                if (BusyEpcs[i]) {
                    ret= StatusEpcs(Mask1, FrontEnd, Status, true);
                    if(ret != NO_ERROR ) {
                      ess[i] --;
                      if(ess[i] == 0)
                      return (ret);
                    } else {
                        BusyEpcs[i] = (Status[1] & 0x100) >> 8;
                        ess[i] = nbtry;
                    }

                if (m_verbose) cout << ".";
                }
                EndWait |= BusyEpcs[i];
              }
            }
        }	while(EndWait);//Status[1] & 0x100);				// EEprom Busy
	}
    WriteMsg("WaitEpcsBusy Status : 0x " + QString::number(Status[1],16));
	p_tcp->Disconnect();
	m_verbose = verb;
	return (ret);

}	// WaitEpcsBusy

//=========================================================================
//=========================================================================
int CAsm::WritePage(u32 Mask, u16 FrontEnd, std::vector <u8> *Buf)
{
	u32 Addr = 0;
	u32 size = Buf->size();
	u32 max ,i,j;
	u16 crc;
    bool Err[NB_ASM];
	union {
		u16 s;
		u8  c[2];
	}tmp;

	int ret;
	bool prog;
	bool ErrProg;
//	size_t len;
    u16 Nbrequest=0;
    for (int i =0;i< NB_ASM;++i)
       if (((Mask >> i) & 0x1) == 0x1)
           Nbrequest++;

    if(p_bar) {
        p_bar->setValue(0);
        p_bar->setMaximum(size);
    }
	ret = p_tcp->Connect();
	if(ret != 0 ) {
		return(ret);
	}
	while (Addr < size) {
		crc = Addr >> 8;
		max = ((Addr+256) > size) ? (size-Addr) : 256;
//		m_str.append(QString("Write : 0x%1...%2").arg(Addr, 6,16, QChar('0')).arg(size, 6, 16, QChar('0')));
//		WriteMsg(m_str);
		if (m_verbose) cout << m_str.toStdString() << endl;
		prog = false;
		for(u32 k=0; k<max; k++) {
			if(Buf->at(k+Addr) != 0xff) {
				prog = true;
				break;
			}
		}
		if(prog) {
            m_str.append(QString("Write : 0x%1...%2").arg(Addr, 6,16, QChar('0')).arg(size, 6, 16, QChar('0')));
            WriteMsg(m_str);
			u16 cmd = (K30_7 | ((FrontEnd & 0x007F) <<8));
			u16 parity = ComputeParity(cmd)<< 15;
			cmd |= parity;

			p_tcp->ClearBuf();
			p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
			p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
			p_tcp->frm_cmd.cmd   = CMD_EXPORT;					// Export Command
			p_tcp->frm_cmd.nbarg = 3 + (max/2);					// Number of Arguments   
			p_tcp->frm_cmd.mask  = Mask;						// Destination Mask
			p_tcp->frm_cmd.arg16[0] = cmd;
			p_tcp->frm_cmd.arg16[1] = (u16) (Addr >> 8);
			j=2;
			for(i=0; i<max; i+=2) {
				tmp.c[0] = Reverse (Buf->at(i+Addr));
				tmp.c[1] = Reverse (Buf-> at(i+1+Addr));
				p_tcp->frm_cmd.arg16[j] = tmp.s;
//				cout << " buf " << hex << (u16)(Buf->at(i+Addr)) << " " << (u16)(Buf-> at(i+1+Addr)) << "...." << tmp.s << dec << endl;
				crc ^= tmp.s;
				j++;
			}	// for(u32 i=0; i<max; i+=2)
			p_tcp->frm_cmd.arg16[j] = crc;

			ret = p_tcp->Exchange();
            if (ret > 0) {//== NO_ERROR) {
                ret = Manage(ret,Nbrequest);
				if(ret == NO_ERROR) {
                    ret = ReadAckProg(Nbrequest,&Err[0]);
                    for (int k=0;k<Nbrequest;++k) ret |=Err[k];
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
                    p_tcp->Disconnect();
                    ret = p_tcp->Connect();
                    if(ret != 0 ) {
                        return(ret);
                    }
				}
			} else {
				WriteMsg("Client Can't Send to Server " + p_tcp->GetServer(), "red");
				if(m_verbose) cerr << FgColor::red << "Client Can't Send to Server " << p_tcp->GetServer().toStdString() << FgColor::white << endl;
				p_tcp->Disconnect();
				return(ret);
			}
		}	// if(prog)
        if(ErrProg == false) {
            Addr += 256;
            if(p_bar) p_bar->setValue(Addr);
        }
	} // While(Addr < size)
	p_tcp->Disconnect();
	if(m_verbose) cout << FgColor::green << "End of Programming" << FgColor::white << endl;
	return (NO_ERROR);

}	// WritePage

//=========================================================================
//=========================================================================
int CAsm::SendProg(u32 Mask, u16 FrontEnd, QString f_name)
{
	int ret;
    u16 Status[2];
	bool EpcsOk = false;
	std::vector <u8> Memory;
	time_t t1,t2;

	t1=time(NULL);

	if((Mask < 1) || (Mask>0xfff))
	{
		cerr << FgColor::red << "Asm Program Command Wrong Asm Number (0x" << hex << Mask << dec << ")" << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
/*	if(SingleMask(Mask))
	{
		if(m_verbose) cerr << FgColor::red << "Asm Program Command Invalid for Multi Channel Mode" << FgColor::white << endl;
		return(BAD_ASM_NB);
	}
*/	if(f_name.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			f_name = QString("%1/ConfigDcs/asm.rpd").arg(dirdcs);
		} else {
			f_name = "../../ConfigDcs/asm.rpd";
		}
	}
    ret = WaitEpcsBusy (Mask,FrontEnd,&Status[0], 3,1);
	if(ret != NO_ERROR)
		return (ret);

    switch (Status[0] & 0xff) {
        case EPCS1 : cout << FgColor::green << "EPCS1  (0x" << hex << (Status[0] & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
        case EPCS4 : cout << FgColor::green << "EPCS4  (0x" << hex << (Status[0] & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
        case EPCS16: cout << FgColor::green << "EPCS16 (0x" << hex << (Status[0] & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
        case EPCS64: cout << FgColor::green << "EPCS64 (0x" << hex << (Status[0] & 0xff) << dec << ") Detected"  << FgColor::white << endl;EpcsOk=true;break;
        default    : cout << FgColor::red   << "Memory (0x" << hex << (Status[0] & 0xff) << dec << ") not found" << FgColor::white << endl;EpcsOk=false;break;
	}
   if(EpcsOk) {
		ifstream f(f_name.toStdString());
		if(!f.is_open()) {
			cerr << FgColor::red << "Error during loading file : "<< f_name.toStdString() <<endl;
			return (BAD_PROG_NAME);
		}
        ret = EraseEpcs(Mask,FrontEnd,false);
		if(ret != NO_ERROR) {
			f.close();
			return (ret);
		}
		cout << "*************************************************" << endl;
		//for(u16 ii=0; ii<256; ii++)
			//Memory.push_back((u8) ii);
		while(f.good())
			Memory.push_back((u8) f.get());
		f.close();
		ret = WritePage(Mask, FrontEnd, &Memory);
		if(ret != NO_ERROR) {
			cerr << FgColor::red << "Error Programming" << FgColor::white << endl;
			return(ret);
		}
	   cout << "*************************************************" << endl;
	} else {
		cerr << FgColor::red << "Can't Identify EPCS Type !"<< endl;
		return (BAD_EPCS_TYPE);
	}
	t2 = time(NULL);
	cout << FgColor::green << "Programming Duration : " << difftime(t2,t1) << "S" << FgColor::white << endl;
	return (NO_ERROR);

}	// SendProg

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================

