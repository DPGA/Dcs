//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	HvClient.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
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
#include "command_error.h"


#include "HvClient.h"

//=========================================================================
//=========================================================================
CHvClient::CHvClient(int verbose)
{
	m_verbose = verbose;
	if(m_verbose >= VERBOSE)
		cout << "Create CHvClient Class" << endl;
	memset((void*)&hvdt, 0, sizeof(HVDATA));
	
	
	if(m_verbose > VERBOSE) {
		cout << "sizeof(HVDATA) : " << sizeof(HVDATA) << endl;
		cout << "sizeof(double) : " << sizeof(double) << endl;
		cout << "sizeof(bool)   : " << sizeof(bool) << endl;
		cout << "sizeof(u16)    : " << sizeof(u16) << endl;
	}
}   // Constructor

//=========================================================================
CHvClient::~CHvClient()
{
	if(m_verbose >= VERBOSE)
		cout << "Delete CThor Class" << endl;

}	// Destructor

//=========================================================================
//=========================================================================
//=========================================================================
void CHvClient::WriteMsg(QString str)
{
	if(p_mbox) {
		p_mbox->append(str);
	}
}   // WriteMsg

//=========================================================================
//=========================================================================
void CHvClient::WriteMsg(QString str, QColor color)
{
	if(p_mbox) {
		QColor sav = p_mbox->textColor();
		p_mbox->setTextColor(color);
		p_mbox->append(str);
		p_mbox->setTextColor(sav);
	}
}   // WriteMsg

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
int CHvClient::Dump(bool Connected)
{
	if(m_verbose) {
		cout << "sizeof(HVDATA) " << (sizeof(HVDATA))  << endl;
		cout << CtColor::underscore << CtColor::reverse << FgColor::magenta
			  << "HV Dump .... Connected " << Connected << FgColor::white << CtColor::none << endl;
	}
	tcp.ClearBuf();
	tcp.frm_cmd.indic = (u16) (random() & 0xffff);		// Frame Index
	tcp.frm_cmd.stat  = PRJID & 0xffff;					// Command Status
	tcp.frm_cmd.cmd   = CMD_HV_DUMP;						// Command Dump
	tcp.frm_cmd.nbarg = 0;										// Number of Arguments
	tcp.frm_cmd.mask  = 0;										// Destination Mask
	
	int ret = (Connected) ? tcp.Exchange() : tcp.Connect_Exchange();
	ret = Manage (ret , sizeof(HVDATA) >> 1);
	if(m_verbose > VERBOSE) {
		 cout << "Indic 0x" << hex << tcp.frm_rep.indic << endl;
		 cout << "Stat  0x" << tcp.frm_rep.stat << endl;
		 cout << "Cmd   0x" << tcp.frm_rep.cmd << endl;
		 cout << "NbArg 0x" << tcp.frm_rep.nbarg << endl;
		 cout << "Mask  0x" << tcp.frm_rep.mask << endl;
		 int max = (tcp.frm_rep.nbarg <=10) ? tcp.frm_rep.nbarg : 10;
		 for(int i=0; i<max; i++)
			 cout << "Arg " << i << " 0x" << tcp.frm_rep.arg16[i] << endl;
		 cout << dec << endl;
	}

	if(ret == NO_ERROR) {
		memcpy(&hvdt, &tcp.frm_rep.arg16[0], sizeof(HVDATA));
	}
	return (ret);
}	// Dump

//=========================================================================
//=========================================================================
int CHvClient::SwitchHV   (u32 mask, bool onoff , bool Connected)
{
	 cout << "Switch " << mask << " " << onoff << " Connected " << Connected << endl;
	 tcp.ClearBuf();
	 tcp.frm_cmd.indic = (u16) (random() & 0xffff);		// Frame Index
	 tcp.frm_cmd.stat  = PRJID & 0xffff;					// Command Status
	 tcp.frm_cmd.cmd   = CMD_HV_SWITCH;						// Command Switch High Voltage 
	 tcp.frm_cmd.nbarg = 1;										// Number of Arguments
	 tcp.frm_cmd.mask  = mask;									// Destination Mask
	 tcp.frm_cmd.arg16[0] = onoff;							// Set Arguments
	 int ret = (Connected) ? tcp.Exchange() : tcp.Connect_Exchange();
	 ret = Manage(ret, 0);
	 return ret;
}	// SwitchHV

//=========================================================================
//=========================================================================
int CHvClient::ParseXmlOrder(QString f_name)
{
	QString title ;
	QDomDocument xmlDoc;
	double dd;
	bool ok;
	QFile f(f_name);
	cout << __FUNCTION__ << __LINE__ << " f_name :" << f_name.toStdString() << endl;
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
	cout << "verbose " << m_verbose << endl;
	if(m_verbose) cout << "HV Order ParseXML SetContent :" << ret << endl;
	int i = 0;
	QDomElement root = xmlDoc.documentElement();
	if(!root.isNull()) {
		QDomElement child = root.firstChildElement("NAME");
		cout << __FUNCTION__ << __LINE__ << endl;
		while(!child.isNull()) { 
		  i++;
		 if(m_verbose > VERBOSE) {

			  cout << i << " " << root.tagName().toStdString()
				   << " " << child.tagName().toStdString()
				   << " TITLE :" << child.attribute("TITLE").toStdString()
				   << " VALUE :" << child.attribute("VALUE").toStdString()
				   << " COMMENT :" << child.attribute("COMMENT").toStdString() << endl;
		  }

			for(int mod=0; mod<NB_HVMOD; mod++) {
				for(int chan=0; chan<NB_HVCHAN_MOD; chan++) {
					int j = (mod * NB_HVCHAN_MOD) + chan;
					title = QString("HV_%1").arg(j);
					if(child.attribute("TITLE") == title) {
						dd = child.attribute("VALUE").toDouble(&ok);
						hvdt.hvmod[mod].hvchan[chan].order = (short)(dd * 10.0);
					}
				}
			} 
		  child = child.nextSiblingElement();
	   }
	} else {
		m_str = QString("Error while Parsing File : ") + f_name + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		WriteMsg(m_str,"red");
//		TimerMessage(title, m_str);
		if(m_verbose) cerr << m_str.toStdString() << endl;
		return(errno);
	}
	return (NO_ERROR);
}	// ParseXmlOrder

//=========================================================================
//=========================================================================
int CHvClient::LoadOrder(int mod, bool Connected)
{
	 cout << " Load Order....Connected " << Connected  << endl;
	 tcp.ClearBuf();
	 tcp.frm_cmd.indic = (u16) (random() & 0xffff);		// Frame Index
	 tcp.frm_cmd.stat  = PRJID & 0xffff;					// Command Status
	 tcp.frm_cmd.cmd   = CMD_HV_ORDER;						// Command Load High Voltage Orders
	 tcp.frm_cmd.nbarg = NB_HVCHAN_MOD;						// Number of Arguments
	 tcp.frm_cmd.mask  = 1 << mod;							// Destination Mask
	 for (int i=0; i<NB_HVCHAN_MOD; i++)
		tcp.frm_cmd.arg16[i] = hvdt.hvmod[mod].hvchan[i].order;
	 int ret = (Connected) ? tcp.Exchange() : tcp.Connect_Exchange();
	 ret = Manage(ret, 0);
	 return (ret);
}	// LoadOrder

//=========================================================================
//=========================================================================
int  CHvClient::ServerStop (bool Connected)
{
	 cout << " Stop Server...Connected " << Connected << endl;
	 tcp.ClearBuf();
	 tcp.frm_cmd.indic = (u16) (random() & 0xffff);		// Frame Index
	 tcp.frm_cmd.stat  = PRJID & 0xffff;					// Command Status
	 tcp.frm_cmd.cmd   = CMD_HV_STOP;						// Command Stop Hv Server
	 tcp.frm_cmd.nbarg = 0;										// Number of Arguments
	 tcp.frm_cmd.mask  = 0;										// Destination Mask
	 int ret = (Connected) ? tcp.Exchange() : tcp.Connect_Exchange();
	 ret = Manage (ret, 0);
	 return (ret);
}	// ServerStop

//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
int CHvClient::Manage(int man, int nbarg) 
{
	QString err;
	if(man != -1) {
		if(tcp.frm_cmd.indic == tcp.frm_rep.indic) {
			if(m_verbose) {
				cout << "Reply->stat : " << tcp.frm_rep.stat << endl;
				cout << "Reply->NbArg: " << tcp.frm_rep.nbarg << endl;
			}
			if(tcp.frm_rep.stat == NO_ERROR ) {
				if(tcp.frm_rep.nbarg == nbarg) {
					return (NO_ERROR);
//				} else if (tcp.frm_rep.nbarg == 1) {
//					return (ReadAck());
				} else {
					m_str = QString("Bad Nb Arg : Expected (%1) Received (%2)").arg(nbarg).arg(tcp.frm_rep.nbarg);
					WriteMsg(m_str, "red");
					if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
					return (BAD_NBARG);
				}
			} else {
				switch(tcp.frm_rep.stat) {
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
				m_str = QString("Bad Reply : Stat (%1)...").arg(tcp.frm_rep.stat) + err;
				WriteMsg(m_str, "red");
				if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
				return (BAD_STATUS);
			}
		} else {
			m_str = QString("Bad Reply : Expected Indic (%1) -> Received (%2)").arg(tcp.frm_cmd.indic).arg(tcp.frm_rep.indic);
			WriteMsg(m_str, "red");
			if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
			return (BAD_INDIC);
		}
	} else {
		m_str = QString("Client can't send Command to Server ") + tcp.GetServer() + QString(" (errno=%1) : %2").arg(errno).arg(strerror(errno));
		WriteMsg(m_str, "red");
		if(m_verbose) cerr << FgColor::red << m_str.toStdString() << FgColor::white  << endl;
		return (BAD_CONNECTION);
	}
}	// Manage

//=========================================================================
//=========================================================================
//int CHvClient:: ReadAck(void)
//{
	//int ret  = NO_ERROR;
	//return (ret);
//}
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================
//=========================================================================


