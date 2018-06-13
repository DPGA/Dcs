//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Tcp.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//================================================
//================================================
#include <iostream>
using namespace std;

#include <string>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Tcp.h"
#include "Tools.h"
//===============================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
CTCP::CTCP(int verbose)
{
	if (verbose > VERBOSE)
		cout << "Create DPgaTCP Class" << endl;
	m_sock = -1;
	m_verbose = verbose;
	ClearBuf();
	frm_cmd.indic = (u16) (random() & 0xffff);
	frm_cmd.stat  = 0xAA55;
	m_server = "192.168.1.51";
	m_port = 5001;
	m_connect = false;
}   // CTCP

//================================================
//================================================
CTCP::~CTCP ()
{
	if(m_verbose > VERBOSE)
		cout << "Delete DpgaDcs Class" << endl;
}   // ~CTCP

//================================================
//================================================
//================================================
void CTCP::ClearBuf(void)
{
	memset((void*) &frm_cmd, 0, sizeof(TCP_msg));
	memset((void*) &frm_rep, 0, sizeof(TCP_msg));
}   // ClearBuf

//================================================
//================================================
//================================================
void CTCP::Disconnect(void)
{
//    cout << "Disconnect" << endl;
	close (m_sock);
    m_connect = false;
}   // Disconnect

//================================================
//================================================
int CTCP::Connect(void)
{
 //   cout << "Connect" << endl;
	struct sockaddr_in saddr;
	m_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(m_sock < 0) {
		m_connect = false;
		m_str = QString("Client Socket could not be created (errno=%1) %2").arg(errno).arg(strerror(errno));
		TimerMessage("TCP Problem",m_str);
		if(m_verbose)
			cerr << endl << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return(errno);
	}
	memset((void*) &saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr((char*) m_server.c_str());
	saddr.sin_port = htons(m_port);
	if(connect(m_sock, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
		m_connect = false;
		m_str= QString("Client Socket Failed to Connect to Server port %1 (errno=%2) %3").arg(m_port).arg(errno).arg(strerror(errno));
		TimerMessage("TCP Problem",m_str);
		if(m_verbose)
			cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		return (errno);
	}
	m_connect = true;
	if(m_verbose > VERBOSE)
		cout << "Client Socket Connected to server " << m_server << endl;
	return (0);
}   // Connect

//================================================
//================================================
int CTCP::Send_Frame(char* frame, const size_t length)
{
	std::string st;
	if(m_verbose > VERBOSE) {
		cout << "Send (" << length << "): ";
		for (int i=0; i <(int)length; i+=2) {
			cout << CFormat("%02X", (unsigned char) frame[i+1]);
			cout << CFormat("%02X ",(unsigned char) frame[i]);
		}
		cout << endl;
	}
	size_t ret = send(m_sock, frame, length, 0);
	if((int)ret < 0) return (-1);
	if(ret != length) return (-2);
	return (0);
}   // Send_Frame

//================================================
//================================================
int CTCP::WaitRequest(char *buffer, const size_t length)
{
#define TCP_TMO (10000)
	ssize_t nrecv = 0;
	u32 tmo = TCP_TMO;
	char tmp;
	size_t n = 0;

	if(m_verbose > VERBOSE) {
		cout << "Wainting for Request on Socket...";
		fflush(stdout);
	}
	do {
		nrecv = recv(m_sock, (void*) &tmp, 1, MSG_DONTWAIT);
//      cout << "nrecv : " << nrecv << endl;
		if(nrecv < 0) {
			if(errno == EAGAIN) {
				if( n == 0) {
					usleep(1000);
					tmo--;
					if(tmo) continue;
					m_str = QString("Connection TIMEOUT ! (%1 )").arg(tmo);
					TimerMessage("TCP Problem", m_str);
					if(m_verbose)
						cerr << endl << FgColor::red << m_str.toStdString() << FgColor::white << endl;
					return(-1);
				} else {
					break;
				}
			} else {
				m_str = QString("Error while Receiving Request (errno=%1) %s %2").arg(errno).arg(strerror(errno));
				TimerMessage("TCP Problem", m_str);
				if(m_verbose)
					 cerr << endl << FgColor::red << m_str.toStdString() << FgColor::white << endl;
				return(-2);
			}
		} else {
			buffer[n] = tmp;
			n++;
		}
	} while (n < length);
	if(m_verbose > VERBOSE) {
		cout << endl << "Recv (" << length << "): ";
		for(int i=0; i<(int)length; i+=2) {
			cout << CFormat("%02X", (unsigned char) buffer[i+1]);
			cout << CFormat("%02X ", (unsigned char) buffer[i]);
		}
		cout << endl;
	}
	return ((int) n);
}   // WaitRequest

//================================================
//================================================
int CTCP::Exchange(bool returnflag)
{
 //   cout << "Exchange " << returnflag << endl;
    int ret;
	if(m_sock != -1) {
        if (!IsConnected()) ret=Connect();
		size_t len = (frm_cmd.nbarg * sizeof(u16)) + 12;
        ret = Send_Frame((char*) frm_cmd.buf8, len);
		if(ret == 0) {
			if(m_verbose > VERBOSE) {
				cout << " Send Command : " << frm_cmd.cmd << endl;
			}
			if (returnflag) {
				ret = WaitRequest((char*) frm_rep.buf8, TCP_BUF8_LEN);
			}
		} else {
			m_str = QString("Client can't Send Command to Server ") + GetServer() + QString(" (errno=%1) %2").arg(errno).arg(strerror(errno));
//            sprintf(m_str, "Client can't Send Command to Server %s (errno=%d) %s", m_server.c_str(), errno, strerror(errno));
			TimerMessage("TCP Problem", m_str);
			if(m_verbose)
				cerr << FgColor::red << m_str.toStdString() << FgColor::white << endl;
		}
		return (ret);
	}
	return (-1);
}   // Exchange

//================================================
//================================================
int CTCP::Connect_Exchange(bool returnflag)
{
    int ret =0;
//    cout << "Connect_Exchange " << returnflag << endl;
    if (!IsConnected()) ret= Connect();
    if (ret == 0) {
//        cout << "frm_cmd.indic " << frm_cmd.indic << " (0x" << hex << frm_cmd.indic << dec << ")" << endl;
            ret = Exchange(returnflag);
            Disconnect();
    }
	return (ret);
}   // Connect_Exchange

//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
//================================================
