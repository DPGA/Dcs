//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Amc.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//================================================
//================================================
#include <iostream>
using namespace std;

#include "Amc.h"
#include "command_error.h"

//================================================
//================================================
//================================================
//================================================
CAmc::CAmc(int verbose)
{
	m_verbose = verbose;
	p_tcp = NULL;
	p_mbox = NULL;
	if(m_verbose >= VERBOSE)
		cout << "Create CAmc Class" << endl;
	memset((void*)&amcdt, 0, sizeof(AMCDATA));
}	// Constructor
//================================================
//================================================
CAmc::~CAmc()
{
	if(m_verbose >= VERBOSE)
		cout << "Delete CAmc Class" << endl;
}	// Destructor

//====================================================================
//====================================================================
//====================================================================
//====================================================================
void CAmc::WriteMsg(QString str)
{
	if(p_mbox) {
		p_mbox->append(str);
	}
}   // WriteMsg
//====================================================================
void CAmc::WriteMsg(QString str, QColor color)
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
int CAmc::Manage (int man, int nbarg)
{
	QString err;
	if(man !=-1) {
		if(p_tcp->frm_cmd.indic == p_tcp->frm_rep.indic) {
			if(m_verbose) {
				cout << "Reply->Stat : " << p_tcp->frm_rep.stat << endl;
				cout << "Reply->NbArg: " << p_tcp->frm_rep.nbarg << endl;
			}
			if(p_tcp->frm_rep.stat == 0) {
				if (p_tcp->frm_rep.nbarg == nbarg) {
					return (NO_ERROR);
				} else {
					m_str = QString("Bad Nb Arg :  Expected(%1) received (%2)").arg(nbarg).arg(p_tcp->frm_rep.nbarg);
					WriteMsg(m_str,"red");
//					TimerMessage("Amc Manage", m_str);
					if(m_verbose)
						cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
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
				m_str = (QString("Bad Reply : Stat (%1)...%2").arg(p_tcp->frm_rep.stat)) + err;
				WriteMsg(m_str, "red");
//				TimerMessage("Amc Manage", m_str);
				if(m_verbose)
					cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
				 return (BAD_STATUS);
			}
		} else {
			m_str = QString("Bad Reply : Expected Indic %1-> Received %2").arg(p_tcp-> frm_cmd.indic).arg(p_tcp->frm_rep.indic);
			WriteMsg(m_str, "red");
//			TimerMessage("Amc Manage", m_str);
			if(m_verbose)
				cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (BAD_INDIC);
		}
	} else {
		m_str = QString("Client can't send command to server ") + p_tcp->GetServer() + (QString(" (errno=%1) :%2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str, "red");
//		TimerMessage("Amc Manage", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_CONNECTION);
	}
}	// Manage

//================================================
//================================================
int CAmc::Dump (void)
{
	WriteMsg("Amc Dump","cyan");
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Dump" << FgColor::white << CtColor::none << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_DUMP;							// Command Number
	p_tcp->frm_cmd.nbarg = 0;									// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;									// Destination Mask

//	cout << "sizeof(AMCDATA) : " << sizeof(AMCDATA) << endl;
	int ret = Manage(p_tcp->Connect_Exchange(), sizeof(AMCDATA)/2);
	if (ret == NO_ERROR) {
		memcpy((void*) &amcdt, (void*)&(p_tcp->frm_rep.arg32[0]), sizeof(AMCDATA));
	}
	return (ret);

}	// Dump

//================================================
//================================================
void CAmc::Display(void)
{
}	// Display

//================================================
//================================================
void CAmc::DisplayId(void)
{
	if(m_verbose) {
		cout << "Prj Identifier    : " << CFormat("%c", (amcdt.prjid >> 24) & 0xff)
			 << CFormat("%c", (amcdt.prjid >> 16) & 0xff)
			 << CFormat("%c", (amcdt.prjid >> 8) & 0xff)
			 << CFormat("%c", (amcdt.prjid ) & 0xff)
			 << "\t(0x" << hex << amcdt.prjid << ")"<< dec << endl;
		cout << "Amc Firmware Rev  : " << CFormat("%2d", (amcdt.firmrev >> 16) & 0xffff)
			 << CFormat(".%02d",amcdt.firmrev & 0xffff)
			 << "\t(0x" << hex << amcdt.firmrev << ")" << dec << endl;
		cout << "Amc Hardware Rev  : " << CFormat("%c", (amcdt.hardrev >> 24) & 0xff)
			 << CFormat("%c", (amcdt.hardrev >> 16) & 0xff)
			 << CFormat("%c", (amcdt.hardrev >> 8) & 0xff)
			 << CFormat("%c", (amcdt.hardrev ) & 0xff)
			 << "\t(0x" << hex << amcdt.hardrev << ")"<< dec << endl;
		cout << "Amc Serial Number : " << CFormat("%4d", amcdt.sn) << "\t(0x" << hex << amcdt.sn << ")" << dec << endl;
		cout << "System Identifier : " << CFormat("%4d", amcdt.sysid) << "\t(0x" << hex << amcdt.sysid << ")" << dec << endl;
	}
}	// DisplayId

//================================================
//================================================
void CAmc::Reboot (void)
{
	WriteMsg("Amc Nios Reboot", "Cyan");
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc NIOS Reboot" << FgColor::white << CtColor::none << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_NIOS_REBOOT;				// Command Number
	p_tcp->frm_cmd.nbarg = 0;							// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;							// Destination Mask
	p_tcp->Connect_Exchange(false);

}	// Reboot

//================================================
//================================================
int CAmc::GetIdentity (void)
{
	WriteMsg("Amc Get Nios Identity", "cyan");
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Get NIOS Id" << FgColor::white << CtColor::none << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_NIOS_GET_ID;				// Command Number
	p_tcp->frm_cmd.nbarg = 0;									// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;									// Destination Mask

	int ret = Manage(p_tcp->Connect_Exchange(),ARGIN32BITS(5)); //10);
	if(ret == NO_ERROR) {
		amcdt.prjid   = p_tcp->frm_rep.arg32[0];
		amcdt.firmrev = p_tcp->frm_rep.arg32[1];
		amcdt.hardrev = p_tcp->frm_rep.arg32[2];
		amcdt.sn      = p_tcp->frm_rep.arg32[3];
		amcdt.sysid   = p_tcp->frm_rep.arg32[4];
//        cout << endl;
		DisplayId();
//        cout << endl;
	}
	return (ret);

}	// GetId

//================================================
//================================================
int CAmc::ReadRegister	(u32 regaddr, u32* regval)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Read NIOS" << FgColor::white << CtColor::none << endl;

	if(regaddr == (u32)-1)  {
		m_str = QString("Bad Argument (%1, %2)").arg(regaddr).arg(*regval);
		WriteMsg(m_str,"red");
//		TimerMessage("Amc Read Register", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_PARM);
	}
	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_READ;					// Command Number
	p_tcp->frm_cmd.nbarg = ARGIN32BITS(1);				// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;       					// Destination Mask
	p_tcp->frm_cmd.arg32[0] = regaddr;

	int ret = Manage(p_tcp->Connect_Exchange(), ARGIN32BITS(2));
	if(ret == NO_ERROR) {
		if(m_verbose)
			cout << CFormat("Amc Reg (0x%08X) : ", p_tcp->frm_rep.arg32[0])
				 << p_tcp->frm_rep.arg32[1]
				 << CFormat(" (0x%08x)", p_tcp->frm_rep.arg32[1]) << endl;
		*regval = p_tcp->frm_rep.arg32[1];
	}
	return (ret);

}	// ReadRegister

//================================================
//================================================
int CAmc::WriteRegister (u32 regaddr, u32 regval)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Write NIOS" << FgColor::white << CtColor::none << endl;
	if((regaddr == (u32)-1) || (regval == (u32)-1)) {
		m_str = QString("Bad Argument (%1, %2)").arg(regaddr).arg(regval);
		WriteMsg(m_str,"red");
//		TimerMessage("Amc Write Register", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_PARM);
	}
	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
	p_tcp->frm_cmd.cmd   = CMD_WRITE;						// Command Number
	p_tcp->frm_cmd.nbarg = ARGIN32BITS(2);					// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;       							// Destination Mask
	p_tcp->frm_cmd.arg32[0] = regaddr;
	p_tcp->frm_cmd.arg32[1] = regval;

	int ret = Manage(p_tcp->Connect_Exchange(), ARGIN32BITS(2));
	if(ret == NO_ERROR) {
		if(m_verbose)
			cout << CFormat("Amc Reg (0x%08X) : ", p_tcp->frm_rep.arg32[0])
				 << p_tcp->frm_rep.arg32[1]
				 << CFormat(" (0x%08x)", p_tcp->frm_rep.arg32[1]) << endl;
		regval = p_tcp->frm_rep.arg32[1];
	}
	return (ret);

}	// WriteRegister

//================================================
//================================================
int CAmc::WriteCDCE (u32 chip,  u32 val , QString xml)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Write CDCE" << FgColor::white << CtColor::none << endl;

	u32 reg = val & 0xF;

	if(chip < NB_CDCE)
	{
		if(val != (u32) -1)
		{					// One Chip
			if(reg <= NB_CDCE_REG )
			{
				u32 tmp = amcdt.cdce.Chip[chip].reg[reg];
				amcdt.cdce.Chip[chip].reg[reg] = val;
				int ret = WriteManageCDCE (chip, reg);
				if(ret != 0)
				{
					amcdt.cdce.Chip[chip].reg[reg] = tmp;
				}
				return (ret);
			} else {
				m_str = QString("Bad Register Argument (%1)").arg(reg);
				WriteMsg(m_str,"red");
//				TimerMessage("Amc Write CDCE",m_str);
				cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
				return (BAD_REGISTER);
			}
		} else if(xml.isEmpty() == false) {
			m_str = QString("Write from the file ") + xml + QString("\nNot yet implemented !");
			WriteMsg(m_str,"red");
//			TimerMessage("Amc Write DCCE", m_str);
			if(m_verbose)
				cout << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (NO_ERROR);
		} else {
			m_str = (QString("Bad Argument (%1, %2, ").arg(val).arg(xml.isEmpty())) + xml + QString(")");
			WriteMsg(m_str,"red");
//			TimerMessage("Amc Write DCCE", m_str);
			if(m_verbose)
				cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (BAD_PARM);
		}
	} else {
		m_str = QString("Bad Chip Argument (%1)").arg(chip);
		WriteMsg(m_str,"red");
//		TimerMessage("Amc Write CDCE", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_CHIP);
	}
}	// WriteCDCE

//================================================
//================================================
int CAmc::WriteManageCDCE (u32 chip,  u32 reg)
{
	int ret;

	if(chip<=NB_CDCE){
		p_tcp->ClearBuf();
		p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
		p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
		p_tcp->frm_cmd.cmd   = CMD_NIOS_CDCE_SPI_WRITE;		// Command Number
		p_tcp->frm_cmd.mask  = 0;       					// Destination Mask

		if(reg == 0 ) {
			if(m_verbose) cout << "All Channel of Chip " << chip << endl;
			p_tcp->frm_cmd.nbarg = ARGIN32BITS(9);			// Number of Arguments
			p_tcp->frm_cmd.arg32[0] = chip;
			for(int i=0; i< NB_CDCE_REG; i++)
				p_tcp->frm_cmd.arg32[i+1] = amcdt.cdce.Chip[chip].reg[i];
			ret = Manage(p_tcp->Connect_Exchange(), ARGIN32BITS(2));
			return (ret);
		} else if (reg <= NB_CDCE_REG) {
			if(m_verbose) cout << "Channel " << reg << " of chip " << chip << endl;
			p_tcp->frm_cmd.nbarg = ARGIN32BITS(2);			// Number of Arguments
			p_tcp->frm_cmd.arg32[0] = chip;
			p_tcp->frm_cmd.arg32[1] = amcdt.cdce.Chip[chip].reg[reg];
			ret = Manage(p_tcp->Connect_Exchange(), ARGIN32BITS(2));
			return (ret);
		} else {
			return (BAD_REGISTER);
		}
	}else {
		return (BAD_CHIP);
	}
}	// WriteManageCDCE

//================================================
//================================================
int  CAmc::ReadCDCE (u32 chip, QString xml)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Read CDCE" << FgColor::white << CtColor::none << endl;

	if(chip < NB_CDCE){
		if(xml.isEmpty() == false)
		{
			m_str = QString("Read from the file ") + xml + QString("\nNot yet implemented !");
			WriteMsg(m_str,"red");
//			TimerMessage("Amc Read DCCE", m_str);
			if(m_verbose)
				cout << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (NO_ERROR);
		} else {
			if(m_verbose) cout << "Read Single CDCE " << chip <<  endl;
			int ret = ReadManageCDCE (chip);
			if((ret == 0) && (m_verbose)) {
				for (int i=0; i<NB_CDCE_REG; i++)
					cout << "CDCE (Chip :" << chip << ") (Reg : " << i << CFormat( ") : 0x%08x",amcdt.cdce.Chip[chip].reg[i]) << endl;
				cout << endl;
			}
			return (ret);
		}
	} else {
		m_str = QString("Bad Chip Argument (%1)").arg(chip);
		WriteMsg(m_str,"red");
//		TimerMessage("Amc Read CDCE", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_CHIP);
	}

	return 0;

}	// ReadCDCE

//================================================
//================================================
int CAmc::ReadManageCDCE (u32 chip)
{
	if(chip < NB_CDCE )
	{
		p_tcp->ClearBuf();
		p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
		p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
		p_tcp->frm_cmd.cmd   = CMD_NIOS_CDCE_SPI_READ;		// Command Number
		p_tcp->frm_cmd.nbarg = ARGIN32BITS(2);				// Number of Arguments
		p_tcp->frm_cmd.mask  = 0;							// Destination Mask
		p_tcp->frm_cmd.arg32[0] = chip;						// Chip Number
		p_tcp->frm_cmd.arg32[1] = 0;						// All Register
		int len = NB_CDCE_REG + 2;
		int ret = Manage(p_tcp->Connect_Exchange(),ARGIN32BITS(len));
		if(ret == 0 )
		{
			for(int i=0; i< NB_CDCE_REG; i++)
				amcdt.cdce.Chip[chip].reg[i] = p_tcp->frm_rep.arg32[i+2];
		}
		return (ret);
	} else {
		return (BAD_CHIP);
	}
}	// ReadManageCDCE

//================================================
//================================================
int  CAmc::SaveCDCE	(u32 chip)
{
	if(m_verbose)
		cout << CtColor::underscore << CtColor::reverse << FgColor::cyan
			 << "Amc Save CDCE" << FgColor::white << CtColor::none << endl;

	if((chip > 0 ) && (chip <= NB_CDCE ))
	{
		p_tcp->ClearBuf();
		p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
		p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;					// Command Status
		p_tcp->frm_cmd.cmd   = CMD_NIOS_CDCE_SPI_SAVE;		// Command Number
		p_tcp->frm_cmd.nbarg = ARGIN32BITS(1);					// Number of Arguments
		p_tcp->frm_cmd.mask  = 0;									// Destination Mask
		p_tcp->frm_cmd.arg32[0] = chip;

		int ret = Manage(p_tcp->Connect_Exchange(), 0);
		if(ret == 0)
		{
			if(m_verbose) cout << "CDCE (chip " << chip << ") Configuration Saved" << endl;
		}
		return (ret);
	} else {
		m_str = QString("Bad Chip Argument (%1)").arg(chip);
		WriteMsg(m_str,"red");
//		TimerMessage("Amc Save CDCE", m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (BAD_CHIP);
	}
}	// SaveCDCE

//================================================
//================================================
int CAmc::ResetCmd(u32 select)
{
	p_mbox->append("CAmc::ResetCmd");
	if(m_verbose)
		cout << "ResetCmd" << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_RESET;					// Export Command
	p_tcp->frm_cmd.nbarg = ARGIN32BITS(1);				// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;							// Destination Mask
	p_tcp->frm_cmd.arg32[0] =  select;
	return(p_tcp->Connect_Exchange(false));

}	// ResetCmd

//================================================
//================================================
int CAmc::RunCmd(u32 Run, u32 Export)
{
	const QString s[2] = {"Stop", "Start"};
	m_str = "Amc Run Cmd (" + QString::number(Run) + ")";
	if(p_mbox) p_mbox->append(m_str);
	if(m_verbose)
		cout << m_str.toStdString() << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16)(random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_RUN;						// Export Command
	p_tcp->frm_cmd.nbarg = ARGIN32BITS(2);				// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;							// Destination Mask
	p_tcp->frm_cmd.arg32[0] = Run;
	p_tcp->frm_cmd.arg32[1] = Export;

	int ret = Manage(p_tcp->Connect_Exchange(), ARGIN32BITS(1));
	if(ret == NO_ERROR) {
		if(p_tcp->frm_rep.arg32[0] == 0) {
			if(Export) {
				if(m_verbose) cout << FgColor::green << "Amc Run All Channels : " << s[Run].toStdString() << FgColor::white << endl;
			} else {
				if(m_verbose) cout << FgColor::green << "Amc Run Only : " << s[Run].toStdString()  << FgColor::white << endl;
			}
		} else {
			m_str = QString("Amc Run Problem on Channels (0x") + QString::number(p_tcp->frm_rep.arg32[0],16) + QString(") : ") + s[Run];
			WriteMsg(m_str,"red");
//			TimerMessage("Amc Run Command", m_str);
			if(m_verbose)
				cout << FgColor::red << m_str.toStdString() <<FgColor::white << endl;
		 }
	}
	return (ret);

}	// ResetCmd

//================================================
//================================================
int CAmc::JtagCmd(u32 Level)
{
	if(m_verbose)
		cout << "Amc Jtag Cmd (" << Level << ")" << endl;

	p_tcp->ClearBuf();
	p_tcp->frm_cmd.indic = (u16) (random() & 0xFFFF);	// Frame Index
	p_tcp->frm_cmd.stat  = PRJID & 0xFFFF;				// Command Status
	p_tcp->frm_cmd.cmd   = CMD_NIOS_JTAGUART;			// Export Command
	p_tcp->frm_cmd.nbarg = ARGIN32BITS(1);				// Number of Arguments
	p_tcp->frm_cmd.mask  = 0;							// Destination Mask
	p_tcp->frm_cmd.arg32[0] = Level;
	return(Manage(p_tcp->Connect_Exchange(), 0));

}	// JtagCmd

//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================


