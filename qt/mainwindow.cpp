//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	mainwindow.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================


//=================================================
//=================================================
//=================================================
#include <iostream>
using namespace std;

#include <string>
#include <sstream>

#include <QApplication>
#include <QMessageBox>
#include <QTime>
#include <QtXml>
#include <QCloseEvent>

#include <time.h>


#include "MyDefs.h"
#include "Tools.h"
#include "command_error.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dial_alldaq.h"
#include "dial_about.h"
#include "dial_confirm.h"
#include "dial_amcid.h"
#include "dial_verbose.h"
#include "dial_amcreg.h"
#include "dial_amcdcdce.h"
#include "dial_amcreset.h"
#include "dial_asmdaq.h"
#include "dial_asmreg.h"
#include "dial_asmconfig.h"
#include "dial_asmprog.h"
#include "dial_thordaq.h"
#include "dial_thorreg.h"
#include "dial_thorconfig.h"
#include "dial_thorprog.h"
#include "dial_hvconfig.h"
#include "dial_hvonoff.h"
#include "dial_finder.h"
#include "dial_calib.h"
#include "dial_1asm.h"
#include "version.h"
#include "Version.h"

//=================================================
//=================================================
const char *mois[13] = {"","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
enum  {TABAMC, TABASM, TABTHOR, TABHV, TABMISC};

//=================================================
//=================================================
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)

{
	m_mode = 0;
	m_samples = 0;
	m_niosverbose = 0;
	m_verbose  = VERBOSE;
	for(int i=0; i<NB_CHAN; i++) m_frontend[i] = 0;
	m_usemask = 0;
	m_interv = 5;
	m_auto = Qt::Unchecked;
	m_daqsequence = 0;

	XmlParser();

	amc.SetTcp(&tcp);
	asmm.SetTcp(&tcp);
	thor.SetTcp(&tcp);

	p_amcdt = amc.GetData();
	p_asmdt = asmm.GetData(0);
	p_thordt = thor.GetData();
	p_hvdt = hv.GetData();

	thor.ConfigCmd(BROADCAST_FE, "", false);

//   asmm.SetUse(m_usemask);
//   amc.JtagCmd(m_niosverbose);
	SetVerbose(m_verbose);
	if(m_verbose){
		cout << "Number of 10Gbe Link : " << NB_UDP << endl; 
		cout << "sizeof AMCDATA : " << sizeof(AMCDATA) << endl;
	}
	ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(TABMISC);
//   ui->spinBox_MiscTab->setValue(m_tim);
//	ui->lineEdit_ThorTab_GxStatus->setToolTip("Here is my tool tip");
    AmcTip();
    AsmTip();
    ThorTip();
    MiscTip();
    HvTip();

    ui->actionStart_Thor->setToolTip("blabala");

	p_timer = new QTimer(this);
	connect(p_timer,SIGNAL(timeout()), this, SLOT(TimerDisplay()));
	p_timer->start(1000);
    myProcess = new QProcess(this);
    if ((OnStartMonitor) && (myProcess->state() == QProcess::NotRunning)) {
        QStringList arguments;
        myProcess->start(PathMonitor, arguments);
    }
}   // MainWindow

//=================================================
//=================================================
MainWindow::~MainWindow()
{
    QProcess kill;
    kill.start("kill " + QString::number(myProcess->processId()));
	delete ui;

}   // ~MainWindow

//=================================================
//=================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
//    QString st = QFileInfo(QApplication::applicationFilePath()).fileName();
//    cout << "appl name " << st.toStdString() << endl;
    QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                               QFileInfo(QApplication::applicationFilePath()).fileName(),
                                                               tr("Realy, You are Sure?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox:: Yes,
                                                               QMessageBox::Yes);
    if(resBtn != QMessageBox::Yes)
        event->ignore();
    else
        event->accept();
}

//=================================================
//=================================================
 void MainWindow::SetVerbose (int verbose)
 {
	 amc.SetVerbose(verbose);
	 asmm.SetVerbose(verbose);
	 thor.SetVerbose(verbose);
	 tcp.SetVerbose(verbose);
	 hv.SetVerbose(verbose);
 }
 //-------------------------------------------------
 //-------------------------------------------------
 void MainWindow::XmlParser()
 {
	 QString ip;
	 int port;
	 QString hvip;
	 int hvport;
	 QString tmp;
	 bool ok;

	 QString filename;
	 char * dirdcs = getenv("DIR_DCS");
	 if(dirdcs) {
		filename = QString("%1/ConfigDcs/RegDcs.xml").arg(dirdcs); 
	 } else { 
#ifdef Q_OS_OSX
        filename = ("../../../../config/RegDcs.xml");
#else
		filename = "../../ConfigDcs/RegDcs.xml";
#endif
	}
	 QString title ("MainWindow Parse XML");
	 QString str;

	 cout << filename.toStdString() << endl;
	 QDomDocument xmlDoc;
	 QFile f(filename);
	 if(!f.open(QIODevice::ReadOnly)) {
		 str = QString("Error while loading File : ") + filename + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		 TimerMessage(title, str);
		 if(m_verbose) cerr << str.toStdString() << endl;
		 return;
     }
	 bool ret = xmlDoc.setContent(&f);
	 if(!ret) {
		  f.close();
		  str = QString("Error while read File : ") + filename + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		  TimerMessage(title, str);
		  if(m_verbose) cerr << str.toStdString() << endl;
		  return;
	 }
	 f.close();
	 if(m_verbose>VERBOSE) cout << " setContent : " << ret << endl;

	 QDomElement root = xmlDoc.documentElement();
	 int i = 0;
	 if(!root.isNull()) {
		 QDomElement child = root.firstChildElement("NAME");
		 m_usemask = 0;
		 while(!child.isNull()) {
			 i++;
			 if(m_verbose > VERBOSE) {
				 cout << i << " " << root.tagName().toStdString()
					  << " " << child.tagName().toStdString()
					  << " TITLE : " << child.attribute("TITLE").toStdString()
					  << " ID : "   << child.attribute("ID").toStdString()
					  << " ACTIVED : " << child.attribute("ACTIVED").toStdString()
					  << " COMMENT : " << child.attribute("COMMENT").toStdString() << endl;
			 }
			 if (child.attribute("TITLE") == "DPGA_IP")     ip = child.attribute("ID");
			 if (child.attribute("TITLE") == "DPGA_PORT")   port = child.attribute("ID").toInt();
			 if (child.attribute("TITLE") == "MODE")        m_mode = child.attribute("ID").toUInt();
			 if (child.attribute("TITLE") == "SAMPLES")     m_samples = child.attribute("ID").toUInt();
			 if (child.attribute("TITLE") == "VERBOSE")     m_verbose = child.attribute("ID").toUInt();
			 if (child.attribute("TITLE") == "NIOSVERBOSE") m_niosverbose = child.attribute("ID").toUInt();
             if (child.attribute("TITLE") == "DAQSEQUENCE") {tmp = child.attribute("ID");  m_daqsequence = tmp.toUShort(&ok, 16);}
			 if (child.attribute("TITLE") == "AsmFE_0")     checkid(0, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_1")     checkid(1, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_2")     checkid(2, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_3")     checkid(3, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_4")     checkid(4, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_5")     checkid(5, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_6")     checkid(6, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_7")     checkid(7, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_8")     checkid(8, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_9")     checkid(9, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_10")    checkid(10, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "AsmFE_11")    checkid(11, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 if (child.attribute("TITLE") == "ThorFE_0")    checkid(12, child.attribute("ID"), child.attribute("ACTIVED").toUInt());
			 
			 if (child.attribute("TITLE") == "HV_IP")       hvip = child.attribute("ID");
			 if (child.attribute("TITLE") == "HV_PORT")     hvport = child.attribute("ID").toInt();
             if (child.attribute("TITLE") == "MONITOR")     {PathMonitor = child.attribute("PATH");OnStartMonitor = (bool) child.attribute("START").toInt();}
             if (child.attribute("TITLE") == "DIRECTORY")   PathDir = child.attribute("DIR");
			 child = child.nextSiblingElement() ;
		 }
		 tcp.SetIP(ip,port);
         hv.SetIP(hvip,hvport);
	 } else {
		 str = QString("Error while Parsing File : ") + filename + (QString(" (errno=%1) %2").arg(errno).arg(strerror(errno)));
		 TimerMessage(title, str);
		 if(m_verbose) cerr << str.toStdString() << endl;
	 }
	 if(m_verbose) {
		 cout << "End Xml Parse " << endl;
 //        cout << "IP : " << ip.toStdString() << " port : " << port << endl;
		 cout << "IP : " << tcp.GetServer().toStdString()  << " port : " << tcp.GetPort() << endl;
		 cout << "Mode : " << m_mode << " NbSamples : " << m_samples << endl;
		 cout << "Nios Verbose Level : " << m_niosverbose << endl;
 //        cout << "Hv IP : " << hvip.toStdString() << " port : " << hvport << endl;
         cout << "Hv IP : " << hv.GetServer().toStdString() << " port : " << hv.GetPort() << endl;
         cout << "DaqSequence : 0x" << hex << m_daqsequence << dec << endl;
      }
 }   // XmlParser
 //-------------------------------------------------
 //-------------------------------------------------
 //-------------------------------------------------
 //-------------------------------------------------
 void MainWindow::TimerDisplay(void)
 {
	 static u32 cnt = 0;
	int chan;
	 cnt++;
     QString version = " Version = " + QString(__DCS_VERSION_STRING__) +  QString(GITVER) + "  " + QString(__DATE__) + "  " + QString(__TIME__) ;
	 if(tcp.IsConnected()) {
         ui->statusBar->showMessage( ElapsedTime( cnt) + " ... Connected to Amc" +"\t" + version);
	 } else {
         ui->statusBar->showMessage( ElapsedTime( cnt) + " ... Unconnected" + version);
	 }
	 if((cnt % m_interv) == 0 ) {
		if(m_auto == Qt::Checked) {
			int index = ui->tabWidget->currentIndex();
//         cout<< cnt << " current index " << index << endl;
			switch (index) {
                case TABAMC :        // AmcTab
					if(amc.Dump() == NO_ERROR) AmcDisplay();
					break;
                case TABASM :        // AmsTab
					chan = ui->spinBox_AsmTab_Sel->value();
					if(asmm.Dump(1 << chan) == NO_ERROR) AsmDisplay(chan);
					break;
                case TABTHOR :        // ThorTab
					if(thor.Dump() == NO_ERROR)   ThorDisplay();
					break;
                case TABHV :          // Hv Tab
                    if(hv.Dump() == NO_ERROR) HvDisplay();
                    break;
                case TABMISC :        // MiscTab
					MiscDisplay();
					break;
			}
		 }
	 }
 }

 //-------------------------------------------------
 //-------------------------------------------------
 //-------------------------------------------------
 //-------------------------------------------------

 //=================================================
 //=================================================
 // Menu General
 //=================================================
 //=================================================
 //=================================================
 //=================================================
void MainWindow::on_actionQuit_triggered()
{
  QMessageBox::StandardButton reply;
  reply = QMessageBox::warning(this, "Exit Box", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
  if(reply == QMessageBox::Yes)
	QApplication::quit();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionVerbose_triggered()
{
	Dial_Verbose dl(false, &(m_verbose));
    dl.setModal(true);
	dl.exec();
	SetVerbose(m_verbose);
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionDaq_Run_triggered()
{
    Dial_AllDaq dl (&amc, &asmm, &thor, m_usemask, m_daqsequence);
    dl .setModal(true);
    dl.exec();
}


//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionAbout_triggered()
{
	Dial_About dl;
	dl.setModal(true);
	dl.exec();
//    TimerMessage(10000, "Problem", "BlaBla");
//    QTimer * timer = new QTimer(this);
//    QMessageBox *  msgBox = new QMessageBox();
//    msgBox->setWindowTitle("AutoClose Meessage Box");
//    msgBox->setText("Blal bala");
//    timer->singleShot(10000, msgBox, SLOT(hide()));
//    msgBox->exec();
//    msgBox->close();
//    delete msgBox;
//    delete timer;
//    return ;
//    char str[80];
//    sprintf(str, "Client Socket could not be created (Errno=%d) %s", errno, strerror(errno));
//    TimerMessage(5000,"Problem",str);
//    QString strg;
//    strg = QString("Client Socket could not be created (Errno=%1) %2").arg(errno).arg(strerror(errno)) ;
//    TimerMessage("Problem",strg);

//    int ret;
//    AbDialog ab;
//    ab.setModal(true);
//    ret = ab.exec();
//    std::cout << " return : " << ret << endl;
//    QMessageBox::about(this,"About Box", " Bob & All\n LPC 2016");
 }

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionReload_Config_triggered()
{
	XmlParser();
	amc.JtagCmd(m_niosverbose);
}

//=================================================
//=================================================
// TABS
//=================================================
//=================================================
void MainWindow::on_spinBox_MiscTab_editingFinished()
{
	m_interv= ui->spinBox_MiscTab->value();
	p_timer->stop();
	p_timer->start(1000);
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_spinBox_MiscTab_valueChanged(int arg1)
{
	m_interv = arg1;
	p_timer->stop();
	p_timer->start(1000);
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_checkBox_MisTab_clicked()
{
	m_auto = ui->checkBox_MisTab->checkState();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::AmcDisplay(void)
{
	u32 u = QsOrdorn(amc.GetPrjId());
	char *c = (char*) &u;
	ui->lineEdit_AmcTab_PrjId->setText(QString::fromLocal8Bit(c,4));
	ui->lineEdit_AmcTab_Firm->setText(QString("%1.%2").arg((amc.GetFirmRev() >> 16) & 0xffff).arg(amc.GetFirmRev() & 0xffff,2, 10, QChar('0')));
	u = QsOrdorn(amc.GetHardRev());
	c = (char*) &u;
	ui->lineEdit_AmcTab_Hard->setText(QString::fromLocal8Bit(c,4));
	ui->lineEdit_AmcTab_Sn->setText(QString::number(amc.GetSN()));
	ui->lineEdit_AmcTab_SysId->setText(QString::number(amc.GetSysId()));

	struct tm *tmm;
	time_t tt = p_amcdt->sysstamp;
	tmm = gmtime(&tt);
	ui->lineEdit_AmcTab_SysStamp->setText(QString(mois[tmm->tm_mon +1]) + QString(" %1 %2").arg(tmm->tm_mday).arg(tmm->tm_year +1900)
										  + QString(" %1:%2:%3").arg(tmm->tm_hour, 2,10).arg(tmm->tm_min, 1,10,QChar('0')).arg(tmm->tm_sec, 1,10,QChar('0')));    
 //   ui->lineEdit_AmcTab_SysStamp->setText(QString::number(amc.GetSysStamp()));
	ui->lineEdit_AmcTab_Btime->setText(amc.GetBTime());
	ui->lineEdit_AmcTab_BDate->setText(amc.GetBDate());
	ui->lineEdit_AmcTab_Mask->setText(QString("0x%1").arg(p_amcdt->UseMask,4, 16, QChar('0')));
	ui->lineEdit_AmcTab_Cnfg->setText(QString("0x%1").arg(p_amcdt->cnfg.global, 8 , 16, QChar('0')));
	ui->lineEdit_AmcTab_Stat->setText(QString("0x%1").arg(p_amcdt->status.global, 8 , 16, QChar('0')));
	ui->lineEdit_AmcTab_Signal->setText(QString("0x%1").arg(p_amcdt->signal, 8 , 16, QChar('0')));
	ui->lineEdit_AmcTab_NiosVerbose->setText(QString::number(p_amcdt->jtaguart_verbose));

	QLineEdit * cd[NB_CDCE] [NB_CDCE_REG]= {
		{ ui->lineEdit_AmcTab_CDCE00, ui->lineEdit_AmcTab_CDCE01, ui->lineEdit_AmcTab_CDCE02, ui->lineEdit_AmcTab_CDCE03, ui->lineEdit_AmcTab_CDCE04, ui->lineEdit_AmcTab_CDCE05, ui->lineEdit_AmcTab_CDCE06, ui->lineEdit_AmcTab_CDCE07, ui->lineEdit_AmcTab_CDCE08 },
		{ ui->lineEdit_AmcTab_CDCE10, ui->lineEdit_AmcTab_CDCE11, ui->lineEdit_AmcTab_CDCE12, ui->lineEdit_AmcTab_CDCE13, ui->lineEdit_AmcTab_CDCE14, ui->lineEdit_AmcTab_CDCE15, ui->lineEdit_AmcTab_CDCE16, ui->lineEdit_AmcTab_CDCE17, ui->lineEdit_AmcTab_CDCE18 },
		{ ui->lineEdit_AmcTab_CDCE20, ui->lineEdit_AmcTab_CDCE21, ui->lineEdit_AmcTab_CDCE22, ui->lineEdit_AmcTab_CDCE23, ui->lineEdit_AmcTab_CDCE24, ui->lineEdit_AmcTab_CDCE25, ui->lineEdit_AmcTab_CDCE26, ui->lineEdit_AmcTab_CDCE27, ui->lineEdit_AmcTab_CDCE28 }
	};
	for(int i=0 ; i<NB_CDCE; i++) {
		for(int j=0; j<NB_CDCE_REG; j++) {
			cd[i][j]->setText(QString("0x%1").arg(p_amcdt->cdce.Chip[i].reg[j], 8 , 16, QChar('0')));
		}
	}
	ui->lineEdit_AmcTab_CDCECnfg->setText(QString("0x%1").arg(p_amcdt->cdce.cnfg, 8 , 16, QChar('0')));
	ui->lineEdit_AmcTab_CDCEStat->setText(QString("0x%1").arg(p_amcdt->cdce.status, 8 , 16, QChar('0')));
	
	ui->lineEdit_AmcTab_SrcMac->setText(QString("%1:%2:%3:%4:%5:%6")
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[5],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[4],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[3],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[2],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[1],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Src_Mac_Addr.m[0],2,16,QChar('0')));
	ui->lineEdit_AmcTab_DestMac->setText(QString("%1:%2:%3:%4:%5:%6")
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[5],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[4],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[3],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[2],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[1],2,16,QChar('0'))
                                       .arg(p_amcdt->udp[0].Dst_Mac_Addr.m[0],2,16,QChar('0')));
 
	QString str; 
    str = QString::number((p_amcdt->udp[0].Src_Ip_Addr >> 24) & 0xff ) + "." + QString::number((p_amcdt->udp[0].Src_Ip_Addr >> 16) & 0xff ) + "."
        + QString::number((p_amcdt->udp[0].Src_Ip_Addr >>  8) & 0xff ) + "." + QString::number((p_amcdt->udp[0].Src_Ip_Addr      ) & 0xff );
	ui->lineEdit_AmcTab_SrcIp->setText(str);
    str = QString::number((p_amcdt->udp[0].Dst_Ip_Addr >> 24) & 0xff ) + "." + QString::number((p_amcdt->udp[0].Dst_Ip_Addr >> 16) & 0xff ) + "."
        + QString::number((p_amcdt->udp[0].Dst_Ip_Addr >>  8) & 0xff ) + "." + QString::number((p_amcdt->udp[0].Dst_Ip_Addr      ) & 0xff );
	ui->lineEdit_AmcTab_DestIp->setText(str);
    ui->lineEdit_AmcTab_SrcPort->setText(QString::number(p_amcdt->udp[0].Port.Src));
    ui->lineEdit_AmcTab_DestPort->setText(QString::number(p_amcdt->udp[0].Port.Dst));

    if(NB_UDP == 2) {
        ui->lineEdit_AmcTab_SrcMac_2->setText(QString("%1:%2:%3:%4:%5:%6")
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[5],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[4],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[3],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[2],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[1],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Src_Mac_Addr.m[0],2,16,QChar('0')));
        ui->lineEdit_AmcTab_DestMac_3->setText(QString("%1:%2:%3:%4:%5:%6")
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[5],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[4],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[3],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[2],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[1],2,16,QChar('0'))
                                           .arg(p_amcdt->udp[1].Dst_Mac_Addr.m[0],2,16,QChar('0')));

        str = QString::number((p_amcdt->udp[1].Src_Ip_Addr >> 24) & 0xff ) + "." + QString::number((p_amcdt->udp[1].Src_Ip_Addr >> 16) & 0xff ) + "."
            + QString::number((p_amcdt->udp[1].Src_Ip_Addr >>  8) & 0xff ) + "." + QString::number((p_amcdt->udp[1].Src_Ip_Addr      ) & 0xff );
        ui->lineEdit_AmcTab_SrcIp_2->setText(str);
        str = QString::number((p_amcdt->udp[1].Dst_Ip_Addr >> 24) & 0xff ) + "." + QString::number((p_amcdt->udp[1].Dst_Ip_Addr >> 16) & 0xff ) + "."
            + QString::number((p_amcdt->udp[1].Dst_Ip_Addr >>  8) & 0xff ) + "." + QString::number((p_amcdt->udp[1].Dst_Ip_Addr      ) & 0xff );
        ui->lineEdit_AmcTab_DestIp_2->setText(str);
        ui->lineEdit_AmcTab_SrcPort_2->setText(QString::number(p_amcdt->udp[1].Port.Src));
        ui->lineEdit_AmcTab_DestPort_2->setText(QString::number(p_amcdt->udp[1].Port.Dst));

    }


	ui->lineEdit_AmcTab_PtrigCnt->setText(QString::number(p_amcdt->PrTrigCount));
	ui->lineEdit_AmcTab_TrigCnt->setText(QString::number(p_amcdt->TrigCount));

    ui->lineEdit_AmcTab_FrmCount_1->setText(QString::number(p_amcdt->FrmCount[0]));
    ui->lineEdit_AmcTab_FrmCount_2->setText(QString::number(p_amcdt->FrmCount[1]));
    ui->lineEdit_AmcTab_FrmCount_3->setText(QString::number(p_amcdt->FrmCount[2]));
    ui->lineEdit_AmcTab_FrmCount_4->setText(QString::number(p_amcdt->FrmCount[3]));
    ui->lineEdit_AmcTab_FrmCount_5->setText(QString::number(p_amcdt->FrmCount[4]));
    ui->lineEdit_AmcTab_FrmCount_6->setText(QString::number(p_amcdt->FrmCount[5]));
    ui->lineEdit_AmcTab_FrmCount_7->setText(QString::number(p_amcdt->FrmCount[6]));
    ui->lineEdit_AmcTab_FrmCount_8->setText(QString::number(p_amcdt->FrmCount[7]));
    ui->lineEdit_AmcTab_FrmCount_9->setText(QString::number(p_amcdt->FrmCount[8]));
    ui->lineEdit_AmcTab_FrmCount_10->setText(QString::number(p_amcdt->FrmCount[9]));
    ui->lineEdit_AmcTab_FrmCount_11->setText(QString::number(p_amcdt->FrmCount[10]));
    ui->lineEdit_AmcTab_FrmCount_12->setText(QString::number(p_amcdt->FrmCount[11]));

	ui->lineEdit_AmcTab_LTemp->setText(QString::number(((double)p_amcdt->local_temperature * TEMP_LSB),'f',2) + " °C");
	ui->lineEdit_AmcTab_FTemp->setText(QString::number(((double)p_amcdt->fpga_temperature * TEMP_LSB),'f', 2) + " °C");

	switch(p_amcdt->cnfg.mode) {
		case 0  : str = "0_Raw"; break;
		case 1  : str = "1_Normal"; break;
		default : str = QString::number(p_amcdt->cnfg.mode) + "_Unknow";
	}
	ui->lineEdit_AmcTab_CnfgMode->setText(str);
	ui->lineEdit_AmcTab_CnfgSamples->setText(QString::number(p_amcdt->cnfg.nbsamples));
//	str = (p_amcdt->cnfg.daq) ? "On" : "Off" ;
	if(p_amcdt->cnfg.daq) {
		if(p_amcdt->status.DaqPause) {
			str = "Pause";
		} else {
			str = "On";
		}
	} else {
		str = "Off";
	}
	ui->lineEdit_AmcTab_CnfgDaq->setText(str);
	str.clear();
	if(p_amcdt->cnfg.leds & 0x4) str.append("Red ");
	if(p_amcdt->cnfg.leds & 0x2) str.append("Green ");
	if(p_amcdt->cnfg.leds & 0x1) str.append("Blue");
	ui->lineEdit_AmcTab_CnfgLeds->setText(str);
//    ui->lineEdit_AmcTab_CnfgLeds->setText(QString::number(p_amcdt->cnfg.leds));

	ui->lineEdit_AmcTab_StatUpd->setText("0x" + QString::number(p_amcdt->status.UpdStat,16));

	str = (p_amcdt->status.AsmReconfig) ? ("In Progress " + QString::number(p_amcdt->status.AsmReconfig)) : "Done";
	ui->lineEdit_AmcTab_StatAsmCnfg->setText(str);
	str = (p_amcdt->status.AsmPllLock) ? "Lock" : "Unlock";
	ui->lineEdit_AmcTab_StatAsmPll->setText(str);

//	cout << " AsmSynchred : 0x" << hex << p_amcdt->status.AsmSynchred << dec << endl;
	Qt::CheckState state;
	state = (p_amcdt->status.AsmSynchred & 0x001)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_1->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x002)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_2->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x004)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_3->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x008)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_4->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x010)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_5->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x020)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_6->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x040)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_7->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x080)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_8->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x100)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_9->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x200)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_10->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x400)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_11->setCheckState(state);
	state = (p_amcdt->status.AsmSynchred & 0x800)  ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AcTab_StatAsmSyn_12->setCheckState(state);
	state = (p_amcdt->status.DaqPause)             ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_AmcTab_Pause->setCheckState(state);
	

	str = (p_amcdt->status.ThorReconfig) ? "In use" : "Done" ;
	ui->lineEdit_AmcTab_StatThorCnfg->setText(str);
	str = (p_amcdt->status.ThorPllLock) ? "Lock" : "Unlock";
	ui->lineEdit_AsmTab_StatThorPll->setText(str);
	str = (p_amcdt->status.ThorSynchred) ? "Synchro" : "Not Synchro";
	ui->lineEdit_AmcTab_StatThorSyn->setText(str);
	
    str = QString("<b>Global Config</b> 0x%1").arg(p_amcdt->cnfg.global, 8, 16, QChar('0'));
    str.append("<ul>");
	switch(p_amcdt->cnfg.mode) {
        case 0  : str.append("<li>Mode : <font color= 'green'>0_Raw</font></li>"); break;
        case 1  : str.append("<li>Mode : <font color= 'green'>1_Normal</font></li>"); break;
        default : str.append(QString("<li>Mode : <font color= 'red'>%1_Unknow</font></li>").arg(p_amcdt->cnfg.mode));
	}
    str.append(QString("<li>Nb of Samples : %1</li>").arg(p_amcdt->cnfg.nbsamples));
    if(p_amcdt->cnfg.daq) str.append("<li>DaQ : <font color= 'green'>Run</font></li>"); else str.append("<li>DaQ : <font color= 'red'>Stop</font></li>");
    str.append("<li>Leds : ");
    if(p_amcdt->cnfg.leds == 0x0) str.append("Off</li>");
	else {
        if(p_amcdt->cnfg.leds & 0x4) str.append("<font color= 'green'>Red</font> </li>");
        if(p_amcdt->cnfg.leds & 0x2) str.append("<font color= 'green'>Green</font> </li>");
        if(p_amcdt->cnfg.leds & 0x1) str.append("<font color= 'green'>Blue</font></li>");
	}
    str.append("</ul>");
	ui->lineEdit_AmcTab_Cnfg->setToolTip(str);

    str = QString("<b>Global Status</b> 0x%1").arg(p_amcdt->status.global,8, 16, QChar('0'));
    str.append("<ul>");
    str.append(QString("<li>Upd Status : 0x%1").arg(p_amcdt->status.UpdStat, 2 , 16 , QChar('0')));
    if(p_amcdt->status.UpdStat & 0x01) str.append("<li>UDP Port : <font color= 'red'>Bad</font></li>");        else str.append("<li>UDP Port : <font color= 'green'>Ok</color></font></li>");
    if(p_amcdt->status.UpdStat & 0x02) str.append("<li>IP Addr : <font color= 'red'>Bad</font></li>");         else str.append("<li>IP Addr : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x01) str.append("<li>Udp Protocol : <font color= 'red'>Bad</font></li>");    else str.append("<li>IP Protocol : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x01) str.append("<li>IP Version : <font color= 'red'>Bad</font></li>");      else str.append("<li>IP Version : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x01) str.append("<li>Mac Type : <font color= 'red'>Bad</font></li>");        else str.append("<li>Mac Type : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x01) str.append("<li>Mac Addr : <font color= 'red'>Bad</font></li>");        else str.append("<li>Mac Addr : <font color= 'green'>Ok</font></li>");
	
    if(p_amcdt->status.AsmReconfig) str.append("<li>Asm Reconfig : <font color= 'red'>Busy</font></li>");         else str.append("<li>Asm Reconfig : <font color= 'green'>Done</font></li>");
    if(p_amcdt->status.AsmPllLock) str.append("<li>Asm Pll : <font color= 'green'>Locked</font></li>");          else str.append("<li>Asm Pll : <font color= 'red'>Unlocked</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x001) str.append("<li>Asm  0 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  0 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x002) str.append("<li>Asm  1 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  1 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x004) str.append("<li>Asm  2 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  2 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x008) str.append("<li>Asm  3 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  3 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x010) str.append("<li>Asm  4 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  4 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x020) str.append("<li>Asm  5 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  5 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x040) str.append("<li>Asm  6 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  6 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x080) str.append("<li>Asm  7 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  7 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x100) str.append("<li>Asm  8 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  8 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x200) str.append("<li>Asm  9 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm  9 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x400) str.append("<li>Asm 10 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm 10 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.AsmSynchred & 0x800) str.append("<li>Asm 11 : <font color= 'green'>Synchro</font></li>"); else str.append("<li>Asm 11 : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.ThorReconfig) str.append("<li>Thor Reconfig : <font color= 'red'>Busy</font></li>");       else str.append("<li>Thor Reconfig : <font color= 'green'>Done</font></li>");
    if(p_amcdt->status.ThorPllLock)  str.append("<li>Thor Pll : <font color= 'green'>Locked</font></li>");       else str.append("<li>Thor Pll : <font color= 'red'>Unlocked</font></li>");
    if(p_amcdt->status.ThorSynchred) str.append("<li>Thor0 : <font color= 'green'>Synchro</font></li>");         else str.append("<li>Thor : <font color= 'red'>Not Synchro</font></li>");
    if(p_amcdt->status.DaqPause)     str.append("<li>Daq Pause : <font color= 'red'>on</font></li>");            else str.append("<li>Daq Pause : <font color= 'green'>off</font></li>");
    str.append("</ul>");
	ui->lineEdit_AmcTab_Stat->setToolTip(str);
	
    str = QString("<b>Upd Status</b>0x%1").arg(p_amcdt->status.UpdStat, 4 , 16 , QChar('0'));
    str.append("<ul>");
    if(p_amcdt->status.UpdStat & 0x001) str.append("<li>Link 0 UDP Port : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 0 UDP Port : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x002) str.append("<li>Link 0 IP Addr : <font color= 'red'>Bad</font></li>");         else str.append("<li>Link 0 IP Addr : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x004) str.append("<li>Link 0 Udp Protocol : <font color= 'red'>Bad</font></li>");    else str.append("<li>Link 0 IP Protocol : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x008) str.append("<li>Link 0 IP Version : <font color= 'red'>Bad</font></li>");      else str.append("<li>Link 0 IP Version : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x010) str.append("<li>Link 0 Mac Type : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 0 Mac Type : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x020) str.append("<li>Link 0 Mac Addr : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 0 Mac Addr : <font color= 'green'>Ok</font></li>");
    str.append("</ul> <ul>");
    if(p_amcdt->status.UpdStat & 0x040) str.append("<li>Link 1 UDP Port : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 1 UDP Port : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x080) str.append("<li>Link 1 IP Addr : <font color= 'red'>Bad</font></li>");         else str.append("<li>Link 1 IP Addr : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x100) str.append("<li>Link 1 Udp Protocol : <font color= 'red'>Bad</font></li>");    else str.append("<li>Link 1 IP Protocol : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x200) str.append("<li>Link 1 IP Version : <font color= 'red'>Bad</font></li>");      else str.append("<li>Link 1 IP Version : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x400) str.append("<li>Link 1 Mac Type : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 1 Mac Type : <font color= 'green'>Ok</font></li>");
    if(p_amcdt->status.UpdStat & 0x800) str.append("<li>Link 1 Mac Addr : <font color= 'red'>Bad</font></li>");        else str.append("<li>Link 1 Mac Addr : <font color= 'green'>Ok</font></li>");
 
    str.append("</ul>");
    ui->lineEdit_AmcTab_StatUpd->setToolTip(str);
	
    str = QString("<b>Signal Status</b> 0x%1").arg(p_amcdt->signal, 8, 16 , QChar('0'));
    str.append("<ul>");
    if((p_amcdt->signal & (0x3 << 0)) == (0x3 << 0)) str.append("<li>Asm  0 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  0 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 << 2)) == (0x3 << 2)) str.append("<li>Asm  1 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  1 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 << 4)) == (0x3 << 4)) str.append("<li>Asm  2 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  2 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 << 6)) == (0x3 << 6)) str.append("<li>Asm  3 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  3 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 << 8)) == (0x3 << 8)) str.append("<li>Asm  4 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  4 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<10)) == (0x3 <<10)) str.append("<li>Asm  5 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  5 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<12)) == (0x3 <<12)) str.append("<li>Asm  6 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  6 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<14)) == (0x3 <<14)) str.append("<li>Asm  7 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  7 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<16)) == (0x3 <<16)) str.append("<li>Asm  8 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  8 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<18)) == (0x3 <<18)) str.append("<li>Asm  9 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm  9 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<20)) == (0x3 <<20)) str.append("<li>Asm 10 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm 10 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<22)) == (0x3 <<22)) str.append("<li>Asm 11 Rx Status : <font color= 'green'>Synchro</font></li>");  else  str.append("<li>Asm 11 Rx Status : <font color= 'red'>Not Synchro</font></li>");
    if((p_amcdt->signal & (0x3 <<24)) == (0x3 <<24)) str.append("<li>Thor Rx Status : <font color= 'green'>Synchro</font></li>");    else  str.append("<li>Thor Rx Status : <font color= 'red'>Not Synchro</font></li>");
	if((p_amcdt->signal & (0x3F<<26))) str.append("\nUnknown Upper Part Status");   
    str.append("</ul>");
	ui->lineEdit_AmcTab_Signal->setToolTip(str);

}   // AmcDisplaySysStamp));

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::AsmDisplay(u32 Channel)
{
 //   p_asmdt = asmm.GetData(ui->spinBox_AsmTab_Sel->value());
	p_asmdt = asmm.GetData(Channel);
 //   cout << "Channel " <<  Channel << endl;

	u32  u = QsOrdorn(p_asmdt->rd.BoardId);
	char *c = (char*) &(u);
	ui->lineEdit_AsmTab_Id->setText(QString::fromLocal8Bit(c,4));
	ui->lineEdit_AsmTab_Sn->setText(QString::number(p_asmdt->rd.BoardSN)); 
	ui->lineEdit_AsmTab_FirmRev->setText(QString("%1.%2").arg(p_asmdt->rd.FwRev_Maj).arg(p_asmdt->rd.FwRev_Min,2, 10, QChar('0')));
	ui->lineEdit_AsmTab_HardRev->setText(QString("%1.%2").arg(p_asmdt->rd.HwRev_Maj).arg(p_asmdt->rd.HwRev_Min,2, 10, QChar('0')));
	ui->lineEdit_AsmTab_BTime->setText(QString("%1:%2:%3").arg((p_asmdt->rd.BuildTime >> 16) & 0xffff, 2, 16).arg((p_asmdt->rd.BuildTime >> 8) & 0xff, 2, 16, QChar('0')).arg(p_asmdt->rd.BuildTime & 0xff, 2, 16, QChar('0')));
	
	if(  (((p_asmdt->rd.BuildDate >> 24) & 0xff) > 0) && (((p_asmdt->rd.BuildDate >> 24) & 0xff) <=12))
		ui->lineEdit_AsmTab_BDate->setText(QString(mois[((p_asmdt->rd.BuildDate >> 24) & 0xff)]) + QString(" %1 %2").arg((p_asmdt->rd.BuildDate >> 16) & 0xff, 2, 16, QChar('0')).arg(p_asmdt->rd.BuildDate & 0xffff, 4, 16, QChar('0')));
	else
		ui->lineEdit_AsmTab_BDate->setText(QString("%1/%2/%3").arg((p_asmdt->rd.BuildDate >> 24) & 0xff, 2, 16).arg((p_asmdt->rd.BuildDate >> 16) & 0xff, 2, 16, QChar('0')).arg(p_asmdt->rd.BuildDate & 0xffff, 4, 16, QChar('0')));
	ui->lineEdit_AsmTab_AddrConf->setText(QString("0x%1").arg(p_asmdt->rd.AddrConf, 2, 16, QChar('0')));
	ui->lineEdit_AsmTab_StatGx->setText(QString("0x%1").arg(p_asmdt->rd.Stat_GXB, 8, 16, QChar('0')));
	u64 cnt;
	
	QString str;
	
	cnt = ((u64)p_asmdt->rd.Cpt_Single_Msb << 32) + ((u64)p_asmdt->rd.Cpt_Single_Mid << 16) + (u64)p_asmdt->rd.Cpt_Single_Lsb;       ui->lineEdit_AsmTab_Cnt0->setText(QString("0x%1").arg(cnt,4, 16, QChar('0')));
    str =  QString("<b>Status Epcs </b> 0x%1").arg(cnt,4, 16, QChar('0'));
	str += QString("<Li>Epcs Sid  0x%1</li>").arg((p_asmdt->rd.Cpt_Single_Msb & 0xff),2, 16, QChar('0'));
	str += QString("<li>Epcs Rid  0x%1</li>").arg(((p_asmdt->rd.Cpt_Single_Msb & 0xff00) >> 8),2, 16, QChar('0'));
	str += QString("<Li>Epcs Data Ok  0x%1</li>").arg(((p_asmdt->rd.Cpt_Single_Mid & 0x8000) << 16),2, 16, QChar('0'));
	
	ui->lineEdit_AsmTab_Cnt0->setToolTip(str);
	
	
	cnt = ((u64)p_asmdt->rd.Cpt_Sgl_Veto_Msb << 32) + ((u64)p_asmdt->rd.Cpt_Sgl_Veto_Mid << 16) + (u64)p_asmdt->rd.Cpt_Sgl_Veto_Lsb; ui->lineEdit_AsmTab_Cnt1->setText(QString::number(cnt));
	cnt = ((u64)p_asmdt->rd.Cpt_Clk64_Msb << 32) + ((u64)p_asmdt->rd.Cpt_Clk64_Mid << 16) + (u64)p_asmdt->rd.Cpt_Clk64_Lsb;          ui->lineEdit_AsmTab_Cnt2->setText(QString::number(cnt));
	cnt = ((u64)p_asmdt->rd.Cpt_64Inib_Msb << 32) + ((u64)p_asmdt->rd.Cpt_64Inib_Mid << 16) + (u64)p_asmdt->rd.Cpt_64Inib_Lsb;       ui->lineEdit_AsmTab_Cnt3->setText(QString::number(cnt));
	cnt = ((u64)p_asmdt->rd.Cpt_Trigin_Msb << 32) + ((u64)p_asmdt->rd.Cpt_Trigin_Mid << 16) + (u64)p_asmdt->rd.Cpt_Trigin_Lsb;       ui->lineEdit_AsmTab_Cnt4->setText(QString::number(cnt));
	ui->lineEdit_AsmTab_Cnt5->setText("Nav");
	ui->lineEdit_AsmTab_Cnt6->setText("Nav");
	ui->lineEdit_AsmTab_Cnt7->setText("Nav");
	

    asmm.decodeStatus(NULL,ui->lineEdit_AsmTab_PllLock,m_usemask,Channel,false);
/*	ui->lineEdit_AsmTab_PllLock->setText(QString("0x%1").arg(p_asmdt->rd.Pll_Lock,4, 16, QChar('0')));
	str = QString("<b>Pll Locked</b> 0x%1").arg(p_asmdt->rd.Pll_Lock,4, 16, QChar('0'));
   str.append("<ul>");
   if(p_asmdt->rd.Pll_Lock & (1 << 0)) str.append("<li>Pll Drs 0			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 0        : <font color='red'>not Locked</font></li>");
   if(p_asmdt->rd.Pll_Lock & (1 << 1)) str.append("<li>Pll Drs 1			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 1        : <font color='red'>not Locked</font></li>");
   if(p_asmdt->rd.Pll_Lock & (1 << 2)) str.append("<li>Pll Drs 2			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 2        : <font color='red'>not Locked</font></li>");
   if(p_asmdt->rd.Pll_Lock & (1 << 3)) str.append("<li>Pll Cdce62005		: <font color='green'>Locked</li>");else str.append("<li>Pll Cdce62005    : <font color='red'>not Locked</font></li>");
   
   if(p_asmdt->rd.Pll_Lock & (1 << 4)) str.append("<li>Pll 40Mhz to		: <font color='green'>Locked</li>");else str.append("<li>Pll 40Mhz to     : <font color='red'>not Locked</font></li>");
   if(p_asmdt->rd.Pll_Lock & (1 << 5)) str.append("<li>Pll Config (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Config (FPGA): <font color='red'>not Locked</font></li>");
   if(p_asmdt->rd.Pll_Lock & (1 << 6)) str.append("<li>Pll Sample (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Sample (FPGA): <font color='red'>not Locked</font></li>");
	
	ui->lineEdit_AsmTab_PllLock->setToolTip(str);
*/
    Qt::CheckState state = (((m_usemask >> Channel) & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
	ui->checkBox_AsmTab_Actived->setCheckState(state);
	ui->lineEdit_AsmTab_FrontEnd->setText(QString("0x%1").arg(p_asmdt->rw.FrontEnd, 2, 16, QChar('0')));

	switch(p_asmdt->rw.Mode) {
		case 0  : str = "0_Raw"; break;
		case 1  : str = "1_Normal"; break;
		default : str = QString::number(p_asmdt->rw.Mode) + "_Unknow";
	}
	ui->lineEdit_AsmTab_Mode->setText(str);
	ui->lineEdit_AsmTab_ConfStrobe->setText(QString("0x%1").arg(p_asmdt->rw.Conf_Strobe, 8, 16, QChar('0')));

	QLineEdit *dc[ASM_NB_CHAN] = {
		ui->lineEdit_AsmTab_DacCm0,  ui->lineEdit_AsmTab_DacCm1,  ui->lineEdit_AsmTab_DacCm2,  ui->lineEdit_AsmTab_DacCm3,  ui->lineEdit_AsmTab_DacCm4,  ui->lineEdit_AsmTab_DacCm5,
		ui->lineEdit_AsmTab_DacCm6,  ui->lineEdit_AsmTab_DacCm7,  ui->lineEdit_AsmTab_DacCm8,  ui->lineEdit_AsmTab_DacCm9,  ui->lineEdit_AsmTab_DacCm10, ui->lineEdit_AsmTab_DacCm11,
		ui->lineEdit_AsmTab_DacCm12, ui->lineEdit_AsmTab_DacCm13, ui->lineEdit_AsmTab_DacCm14, ui->lineEdit_AsmTab_DacCm15, ui->lineEdit_AsmTab_DacCm16, ui->lineEdit_AsmTab_DacCm17,
		ui->lineEdit_AsmTab_DacCm18, ui->lineEdit_AsmTab_DacCm19, ui->lineEdit_AsmTab_DacCm20, ui->lineEdit_AsmTab_DacCm21, ui->lineEdit_AsmTab_DacCm22, ui->lineEdit_AsmTab_DacCm23};
	 QLineEdit *th[ASM_NB_CHAN] ={
		ui->lineEdit_AsmTab_ThHigh0,  ui->lineEdit_AsmTab_ThHigh1,  ui->lineEdit_AsmTab_ThHigh2,  ui->lineEdit_AsmTab_ThHigh3,  ui->lineEdit_AsmTab_ThHigh4,  ui->lineEdit_AsmTab_ThHigh5,
		ui->lineEdit_AsmTab_ThHigh6,  ui->lineEdit_AsmTab_ThHigh7,  ui->lineEdit_AsmTab_ThHigh8,  ui->lineEdit_AsmTab_ThHigh9,  ui->lineEdit_AsmTab_ThHigh10, ui->lineEdit_AsmTab_ThHigh11,
		ui->lineEdit_AsmTab_ThHigh12, ui->lineEdit_AsmTab_ThHigh13, ui->lineEdit_AsmTab_ThHigh14, ui->lineEdit_AsmTab_ThHigh15, ui->lineEdit_AsmTab_ThHigh16, ui->lineEdit_AsmTab_ThHigh17,
		ui->lineEdit_AsmTab_ThHigh18, ui->lineEdit_AsmTab_ThHigh19, ui->lineEdit_AsmTab_ThHigh20, ui->lineEdit_AsmTab_ThHigh21, ui->lineEdit_AsmTab_ThHigh22, ui->lineEdit_AsmTab_ThHigh23 };
	QLineEdit *tl[ASM_NB_CHAN] ={
		ui->lineEdit_AsmTab_ThLow0,  ui->lineEdit_AsmTab_ThLow1,  ui->lineEdit_AsmTab_ThLow2,  ui->lineEdit_AsmTab_ThLow3,  ui->lineEdit_AsmTab_ThLow4,  ui->lineEdit_AsmTab_ThLow5,
		ui->lineEdit_AsmTab_ThLow6,  ui->lineEdit_AsmTab_ThLow7,  ui->lineEdit_AsmTab_ThLow8,  ui->lineEdit_AsmTab_ThLow9,  ui->lineEdit_AsmTab_ThLow10, ui->lineEdit_AsmTab_ThLow11,
		ui->lineEdit_AsmTab_ThLow12, ui->lineEdit_AsmTab_ThLow13, ui->lineEdit_AsmTab_ThLow14, ui->lineEdit_AsmTab_ThLow15, ui->lineEdit_AsmTab_ThLow16, ui->lineEdit_AsmTab_ThLow17,
		ui->lineEdit_AsmTab_ThLow18, ui->lineEdit_AsmTab_ThLow19, ui->lineEdit_AsmTab_ThLow20, ui->lineEdit_AsmTab_ThLow21, ui->lineEdit_AsmTab_ThLow22, ui->lineEdit_AsmTab_ThLow23 };
	for(int i=0; i<ASM_NB_CHAN; i++) {
		dc[i]->setText(QString("0x%1").arg(p_asmdt->rw.Dac_Cm[i], 4, 16, QChar('0')));
		th[i]->setText(QString("0x%1").arg(p_asmdt->rw.Threshold[i].High, 4, 16, QChar('0')));
		tl[i]->setText(QString("0x%1").arg(p_asmdt->rw.Threshold[i].Low, 4, 16, QChar('0')));
	}
	QLineEdit *cd[NB_CDCE_REG] = {
		ui->lineEdit_AsmTab_CDCE0, ui->lineEdit_AsmTab_CDCE1, ui->lineEdit_AsmTab_CDCE2, ui->lineEdit_AsmTab_CDCE3, ui->lineEdit_AsmTab_CDCE4,
		ui->lineEdit_AsmTab_CDCE5, ui->lineEdit_AsmTab_CDCE6, ui->lineEdit_AsmTab_CDCE7, ui->lineEdit_AsmTab_CDCE8 };
	for(int i=0; i<NB_CDCE_REG; i++)
		cd[i]->setText(QString("0x%1").arg(p_asmdt->rw.Jitter_Clean[i], 8, 16, QChar('0')));
	QLineEdit *dr[ASM_NB_ROFS] = {
		ui->lineEdit_AsmTab_DacRofs0, ui->lineEdit_AsmTab_DacRofs1, ui->lineEdit_AsmTab_DacRofs2, ui->lineEdit_AsmTab_DacRofs3,ui->lineEdit_AsmTab_DacRofs4, ui->lineEdit_AsmTab_DacRofs5 };
	for(int i=0; i<ASM_NB_ROFS; i++)
		dr[i]->setText(QString("0x%1").arg(p_asmdt->rw.DacRofs[i], 4, 16, QChar('0')));
	ui->lineEdit_AsmTab_Samples->setText(QString::number(p_asmdt->rw.NbSamples));
	ui->lineEdit_AsmTab_ChanRead->setText(QString::number(p_asmdt->rw.ChannelRead));
	ui->lineEdit_AsmTab_Width->setText(QString::number(p_asmdt->rw.Width_Delay >> 8));
	ui->lineEdit_AsmTab_Delay->setText(QString::number(p_asmdt->rw.Width_Delay & 0xff));
	ui->lineEdit_AsmTab_Filter_Thr_H->setText(QString("0x%1").arg(p_asmdt->rw.Filter_Thr_H, 8, 16, QChar('0')));
	ui->lineEdit_AsmTab_Filter_Thr_L->setText(QString("0X%1").arg(p_asmdt->rw.Filter_Thr_L,  8, 16, QChar('0')));
	ui->lineEdit_AsmTab_Offset->setText(QString("0x%1").arg(p_asmdt->rw.Blonde_Offset, 4, 16, QChar('0')));
	
    
    ui->lineEdit_AsmTab_Pattern->setText(QString("0x%1").arg(p_asmdt->rw.Pattern, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_ShtClkSelect->setText(QString("0x%1").arg(p_asmdt->rw.ShiftClockData, 8, 16, QChar('0')));
    ui->lineEdit_AsmTab_PllAdc->setText(QString("0x%1").arg(p_asmdt->rw.ShiftPllAdc, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_FreqPied->setText(QString("0x%1").arg(p_asmdt->rw.FreqPied, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_Spare2->setText(QString("0x%1").arg(p_asmdt->rw.RegSpare2, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_Spare3->setText(QString("0x%1").arg(p_asmdt->rw.RegSpare3, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_Spare4->setText(QString("0x%1").arg(p_asmdt->rw.RegSpare4, 4, 16, QChar('0')));
    ui->lineEdit_AsmTab_Spare5->setText(QString("0x%1").arg(p_asmdt->rw.RegSpare5, 4, 16, QChar('0')));
    
    str = QString("<b>Global Status</b> 0x%1").arg(p_asmdt->rd.Stat_GXB,8, 16, QChar('0'));
    str.append("<ul>");
    if(p_asmdt->rd.Stat_GXB & (1 <<  0)) str.append("<li>Reconfiguration : <font color='red'>Busy</font></li>");   else str.append("<li>Reconfiguration : <font color='green'>Done</font></li>");
    str.append(QString("<li>Rx Bit Slip : 0x%1</li>").arg((p_asmdt->rd.Stat_GXB & 0x3E) >> 1, 2 , 16 ,QChar('0')));
    if(p_asmdt->rd.Stat_GXB & (1 <<  6)) str.append("<li>Transnmit : <font color='red'>Fault</font></li>");        else str.append("<li>Transmit : <font color='green'>Ok</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 <<  7)) str.append("<li>Receive : <font color='red'>Loss</font></li>");           else str.append("<li>Receive : <font color='green'>Ok</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 <<  8)) str.append("<li>Module : <font color='red'>Absent</font></li>");          else str.append("<li>Module : <font color='green'>Present</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 <<  9)) str.append("<li>Synchred : <font color='green'>Yes</font></li>");           else str.append("<li>Synnchred : <font color='red'>No</font></li>");
    str.append(QString("<li>nRx Bit Slip : 0x%1</li>").arg((p_asmdt->rd.Stat_GXB & (0x1F << 10)) >> 10, 2 , 16 ,QChar('0')));
    if(p_asmdt->rd.Stat_GXB & (1 << 15)) str.append("<li>Control 0 : <font color='green'>Detected</font></li>");     else str.append("<li>Control 0 : <font color='red'>Not</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 16)) str.append("<li>Control 1 : <font color='green'>Detected</font></li>");     else str.append("<li>Control 1 : <font color='red'>Not</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 17)) str.append("<li>Rx Frequency : <font color='green'>Locked</font></li>");    else str.append("<li>RXx Freqency : <font color='red'>Unlocked</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 18)) str.append("<li>Synchro 0 : <font color='green'>Detected</font></li>");     else str.append("<li>Synchro 0 : <font color='red'>Not</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 19)) str.append("<li>Synchro 1 : <font color='green'>Detected</font></li>");     else str.append("<li>Synchro 1 : <font color='red'>Not</font></li>");
    str.append(QString("<li>Rx Running Disp : 0x%1</li>").arg((p_asmdt->rd.Stat_GXB & (0x3 << 20)) >> 20, 2 , 16 ,QChar('0')));
    str.append(QString("<li>Rx Disp Error : 0x%1</li>").arg((p_asmdt->rd.Stat_GXB & (0x3 << 22)) >> 22, 2 , 16 ,QChar('0')));
    if(p_asmdt->rd.Stat_GXB & (1 << 24)) str.append("<li>Channel Read : <font color='green'>ON/<li>");        else str.append("<li>Channel Read: <font color='red'>Off</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 25)) str.append("<li>PLL Gx 0 : <font color='green'>Locked</font></li>");           else str.append("<li>PLL Gx 0 : <font color='red'>Unlocked</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 26)) str.append("<li>PLL Gx 1 : <font color='green'>Locked</font></li>");           else str.append("<li>PLL Gx 1 : <font color='red'>Unlocked</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 27)) str.append("<li>Reconfiguration : <font color='red'>Busy</font></li>");   else str.append("<li>Reconfiguration : <font color='green'>Done</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 28)) str.append("<li>Calibration Adc Mezza 0 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 29)) str.append("<li>Calibration Adc Mezza 1 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 30)) str.append("<li>Calibration Adc Mezza 2 : <font color='green'>Done</font></li>");   else str.append("<li>Calibration Adc Mezza 0 : <font color='red'>No</font></li>");
    if(p_asmdt->rd.Stat_GXB & (1 << 31)) str.append("<li>Daq Status : <font color='green'>On</font></li>");        else str.append("<li>Daq Status : <font color='red'>Off</font></li>");
    if(p_asmdt->rd.Stat_GXB & 0x70000000) str.append("<li>Error : <font color='green'>Unknown</font></li>");      // else str.append("<li>Error : <font color='red'>No Error</font></li>");
    str.append("</ul>");
    ui->lineEdit_AsmTab_StatGx->setToolTip(str);

}   // AsmDisplay

//-------------------------------------------------
//-------------------------------------------------
QString MainWindow::ThorMask(int Asm, u8 Mask)
{
	QString str;
	switch(Mask) {
		case 0 :  str = QString("Asm %1 : 000").arg(Asm, 2, 10, QChar(' '));  break;
		case 1 :  str = QString("Asm %1 : 001").arg(Asm, 2, 10, QChar(' '));  break;
		case 2 :  str = QString("Asm %1 : 010").arg(Asm, 2, 10, QChar(' '));  break;
		case 3 :  str = QString("Asm %1 : 011").arg(Asm, 2, 10, QChar(' '));  break;
		case 4 :  str = QString("Asm %1 : 100").arg(Asm, 2, 10, QChar(' '));  break;
		case 5 :  str = QString("Asm %1 : 101").arg(Asm, 2, 10, QChar(' '));  break;
		case 6 :  str = QString("Asm %1 : 110").arg(Asm, 2, 10, QChar(' '));  break;
		case 7 :  str = QString("Asm %1 : 111").arg(Asm, 2, 10, QChar(' '));  break;
		default : str = QString("Asm %1 : xxx").arg(Asm, 2, 10, QChar(' '));  break;
	}
	return (str);
}
//--------------------------
//--------------------------
void MainWindow::ThorDisplay(void)
{
	u32 u = QsOrdorn(p_thordt->rd.BoardID);
	char *c = (char*) &(u);
	ui->lineEdit_ThorTab_BoardId->setText(QString::fromLocal8Bit(c,4));
	ui->lineEdit_ThorTab_Sn->setText(QString::number(p_thordt->rd.BoardSN));
	ui->lineEdit_ThorTab_FirmRev->setText(QString("%1.%2").arg(p_thordt->rd.FwRev_Maj).arg(p_thordt->rd.FwRev_Min,2, 10, QChar('0')));
	ui->lineEdit_ThorTab_HardRev->setText(QString("%1.%2").arg(p_thordt->rd.HwRev_Maj).arg(p_thordt->rd.HwRev_Min,2, 10, QChar('0')));
	ui->lineEdit_ThorTab_BTime->setText(QString("%1:%2:%3").arg((p_thordt->rd.BuildTime >> 16) & 0xffff, 2, 16).arg((p_thordt->rd.BuildTime >> 8) & 0xff, 2, 16, QChar('0')).arg(p_thordt->rd.BuildTime & 0xff, 2, 16, QChar('0')));
	if(  (((p_thordt->rd.BuildDate >> 24) & 0xff) > 0) && (((p_thordt->rd.BuildDate >> 24) & 0xff) <=12))
		ui->lineEdit_ThorTab_BDate->setText(QString(mois[((p_thordt->rd.BuildDate >> 24) & 0xff)]) + QString(" %1 %2").arg((p_thordt->rd.BuildDate >> 16) & 0xff, 2, 16, QChar('0')).arg(p_thordt->rd.BuildDate & 0xffff, 4, 16, QChar('0')));
	else
		ui->lineEdit_ThorTab_BDate->setText(QString("%1/%2/%3").arg((p_thordt->rd.BuildDate >> 24) & 0xff, 2, 16).arg((p_thordt->rd.BuildDate >> 16) & 0xff, 2, 16, QChar('0')).arg(p_thordt->rd.BuildDate & 0xffff, 4, 16, QChar('0')));
	
	ui->lineEdit_ThorTab_AddrConf->setText(QString("0x%1").arg(p_thordt->rd.Addr_Conf, 2, 16, QChar('0')));
	ui->lineEdit_ThorTab_GxStatus->setText(QString("0x%1").arg(p_thordt->rd.GXB_Status,  4, 16, QChar('0')));
	
//	ui->lineEdit_ThorTab_TrigStamp->setText(QString("0x%1").arg(p_thordt->rd.Trig_Stamp, 8, 16, QChar('0')));
	ui->lineEdit_ThorTab_TrigStamp->setText(QString("0x%1%2%3")
								 .arg(p_thordt->rd.Trig_Stamp[0], 4, 16, QChar('0'))
								 .arg(p_thordt->rd.Trig_Stamp[1], 4, 16, QChar('0'))
								 .arg(p_thordt->rd.Trig_Stamp[2], 4, 16, QChar('0')));

	ui->lineEdit_ThorTab_Used->setText(QString::number(p_thordt->rd.PreTrig_FifoUsed));
	ui->lineEdit_ThorTab_Status->setText(QString("0x%1").arg(p_thordt->rd.Global_Status,  4, 16, QChar('0')));
	ui->lineEdit_ThorTab_TT->setText(QString("0x%1").arg(p_thordt->rd.Trigger_TT, 2, 16, QChar('0')));
	ui->lineEdit_ThorTab_TrigPat->setText(QString("0x%1%2%3")
								 .arg(p_thordt->rd.Trig_Pattern[0], 4, 16, QChar('0')) 
								 .arg(p_thordt->rd.Trig_Pattern[1], 4, 16, QChar('0')) 
								 .arg(p_thordt->rd.Trig_Pattern[2], 4, 16, QChar('0'))); 
	
	Qt::CheckState state = (((m_usemask >> THOR_CHAN) & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
	ui->checkBox_ThorTab_Actived->setCheckState(state);

	ui->lineEdit_ThorTab_FrontEnd->setText(QString("0x%1").arg(p_thordt->rw.FrontEnd, 2, 16, QChar('0')));
	QString str;
	switch (p_thordt->rw.Trigger_Type) {
		case 0 : str = "0_DG_And" ; break;
		case 1 : str = "1_Coinc_2mez"; break;
		case 2 : str = "2_Coinc_1to3Boards"; break;
		case 3 : str = "3_Pedestal"; break;
		case 4 : str = "4_DG_Or"; break;
		case 5 :
		case 6 :
		case 7 : str = QString::number(p_thordt->rw.Trigger_Type) + "_Unknow"; break;
		default: str = QString::number(p_thordt->rw.Trigger_Type) + "_Error"; break;
	}
	ui->lineEdit_ThorTab_TrigType->setText(str);
	ui->lineEdit_ThorTab_ConfStrobe->setText(QString("0x%1").arg(p_thordt->rw.Conf_Strobe, 4, 16, QChar('0')));
	QLineEdit *cd[NB_CDCE_REG] = {
		ui->lineEdit_ThorTab_CDCE0, ui->lineEdit_ThorTab_CDCE1, ui->lineEdit_ThorTab_CDCE2, ui->lineEdit_ThorTab_CDCE3, ui->lineEdit_ThorTab_CDCE4, ui->lineEdit_ThorTab_CDCE5, ui->lineEdit_ThorTab_CDCE6, ui->lineEdit_ThorTab_CDCE7, ui->lineEdit_ThorTab_CDCE8};
	for(int i=0; i<NB_CDCE_REG; i++)
		cd[i]->setText(QString("0x%1").arg(p_thordt->rw.Jitter_Clean[i],  8, 16, QChar('0')));
	ui->lineEdit_ThorTab_PTrigMask->setText(QString("0x%1").arg(p_thordt->rw.PreTrig_Mask, 12, 16, QChar('0')));
	ui->lineEdit_ThorTab_PTrigFreq->setText(QString::number(p_thordt->rw.PreTrig_PedFreq));
	ui->lineEdit_ThorTab_TrigShape->setText(QString::number(p_thordt->rw.Trig_Shape));
	ui->lineEdit_ThorTab_TrigDelay->setText(QString::number(p_thordt->rw.Trig_Retardment));

	state = (p_thordt->rd.Global_Status & 0x0001) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_0->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0002) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_1->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0004) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_2->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0008) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_3->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0010) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_4->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0020) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_5->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0040) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_6->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0080) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_7->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0100) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_9->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0x0200) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_10->setCheckState(state);
	state = (p_thordt->rd.Global_Status & 0xfC00) ? (Qt::Checked) : (Qt::Unchecked); ui->checkBox_ThorTab_St_8->setCheckState(state);


    str = QString("<b>Global Status</b> 0x%1").arg(p_thordt->rd.Global_Status,4, 16, QChar('0'));
    str.append("<ul>");
    if(p_thordt->rd.Global_Status & 0x0001) str.append(QString("<li> C Pll : <font color='green'>Locked</font></li>"));     else str.append(QString("<li> C Pll : <font color='red'>Unlocked</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0002) str.append(QString("<li>Thor Pll : <font color='green'>Locked</font></li>"));   else str.append(QString("<li>Thor Pll : <font color='red'>Unlocked</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0004) str.append(QString("<li>Syst Pll : <font color='green'>Locked</font></li>"));   else str.append(QString("<li>Syst Pll : <font color='red'>Unlocked</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0008) str.append(QString("<li>TDC  Pll : <font color='green'>Locked</font></li>"));   else str.append(QString("<li>TDC  Pll : <font color='red'>Unlocked</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0010) str.append(QString("<li>Rx Synchro : <font color='green'>Yes</font></li>"));    else str.append(QString("<li>Rx Synchro : <font color='red'>No</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0020) str.append(QString("<li>Dcs Fifo Full : <font color='red'>Yes</font></li>"));   else str.append(QString("<li>Dcs Fifo Full : <font color='green'>No</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0040) str.append(QString("<li>Rx Fifo Full : <font color='red'>Yes</font></li>"));    else str.append(QString("<li>Rx Fifo Full : <font color='green'>No</font></li>"));
    if(p_thordt->rd.Global_Status & 0x0080) str.append(QString("<li>PTrig Fifo Full : <font color='red'>Yes</font></li>")); else str.append(QString("<li>PTrig Fifo Full : <font color='green'>No</font></li>")) ;
    if(p_thordt->rd.Global_Status & 0x0100) str.append(QString("<li>Daq Run : <font color='green'>On</font></li>"));        else str.append(QString("<li>Daq Run : <font color='red'>Off</font></li>")) ;
    if(p_thordt->rd.Global_Status & 0x0200) str.append(QString("<li>Daq Pause : <font color='red'>On</font></li>"));        else str.append(QString("<li>Daq Pause : <font color='green'>Off</font></li>")) ;
    if(p_thordt->rd.Global_Status & 0xfC00) str.append(QString("<li>Reseved : <font color='red'>On</font></li>"));          else str.append(QString("<li>Reseved : <font color='green'>Off</font></li>"));
    str.append("</ul>");
	 ui->lineEdit_ThorTab_Status->setToolTip(str);
	
    str = QString("<b>Gx Status</b> 0x%1").arg(p_thordt->rd.GXB_Status, 4, 16, QChar('0'));
    str.append("<ul>");
    if(p_thordt->rd.GXB_Status & (1 <<  0)) str.append("<li>Reconfiguration : <font color='red'>Busy</font></li>"); else str.append("<li>Reconfiguration : <font color='green'>Done</font></li>");
    if(p_thordt->rd.GXB_Status & (1 <<  1)) str.append("<li>Transnmit : <font color='red'>Fault</font></li>");      else str.append("<li>Transmit : <font color='green'>Ok</font></li>");
    if(p_thordt->rd.GXB_Status & (1 <<  2)) str.append("<li>Receive : <font color='red'>Loss</font></li>");         else str.append("<li>Receive : <font color='green'>Ok</font></li>");
    if(p_thordt->rd.GXB_Status & (1 <<  3)) str.append("<li>Module : <font color='red'>Absent</font></li>");        else str.append("<li>Module : <font color='green'>Present</font></li>");
    str.append(QString("<li>Rx Bit Slip : 0x%1</li>").arg((p_thordt->rd.GXB_Status & (0x1F << 4)) >> 4, 2 , 16 ,QChar('0')));
    if(p_thordt->rd.GXB_Status & (1 <<  9)) str.append("<li>Rx Frequency : <font color='green'>Locked</font></li>");  else str.append("<li>RXx Freqency : <font color='red'>Unlocked</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 10)) str.append("<li>PLL 0 : <font color='green'>Locked</font></li>");         else str.append("<li>PLL 0 : <font color='red'>Unlocked</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 11)) str.append("<li>PLL 1 : <font color='green'>Locked</font></li>");         else str.append("<li>PLL 1 : <font color='red'>Unlocked</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 12)) str.append("<li>Sender : <font color='red'>Busy</font></li>");          else str.append("<li>Sender: <font color='green'>Free</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 13)) str.append("<li>Rx Error 0 : <font color='red'>Detected</font></li>");  else str.append("<li>Rx Error 0 : <font color='green'>No Error</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 14)) str.append("<li>Rx Error 1 : <font color='red'>Detected</font></li>");  else str.append("<li>Rx Error 1 : <font color='green'>No Error</font></li>");
    if(p_thordt->rd.GXB_Status & (1 << 15)) str.append("<li>Error : <font color='red'>Unknown</font></li>");       // else str.append("<li>Error : <font color='green'>No Error</font></li>");
    str.append("</ul>");
    ui->lineEdit_ThorTab_GxStatus->setToolTip(str);
    
    str = QString("<b>Thor Pre Trigger Mask</b> 0x%1").arg(p_thordt->rw.PreTrig_Mask, 12, 16, QChar('0')); 
    str.append("<ul>");
    for(int i=0; i<NB_ASM; i++) {
       str.append("<li>");
       str.append(ThorMask(i, (u8) ((p_thordt->rw.PreTrig_Mask >> (i*3)) & 0x7)));
       str.append("</li>");
    }
    str.append("</ul>");
    ui->lineEdit_ThorTab_PTrigMask->setToolTip(str);

}   // ThorDisplay

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::HvDisplay(void)
{

QLineEdit *hv[NB_HVMOD][NB_HVCHAN_MOD] = {
    { ui->lineEdit_Hv_00, ui->lineEdit_Hv_01, ui->lineEdit_Hv_02, ui->lineEdit_Hv_03, ui->lineEdit_Hv_04, ui->lineEdit_Hv_05,ui->lineEdit_Hv_06, ui->lineEdit_Hv_07,
      ui->lineEdit_Hv_08, ui->lineEdit_Hv_09, ui->lineEdit_Hv_10, ui->lineEdit_Hv_11, ui->lineEdit_Hv_12, ui->lineEdit_Hv_13,ui->lineEdit_Hv_14, ui->lineEdit_Hv_15},
    { ui->lineEdit_Hv_16, ui->lineEdit_Hv_17, ui->lineEdit_Hv_18, ui->lineEdit_Hv_19, ui->lineEdit_Hv_20, ui->lineEdit_Hv_21,ui->lineEdit_Hv_22, ui->lineEdit_Hv_23,
      ui->lineEdit_Hv_24, ui->lineEdit_Hv_25, ui->lineEdit_Hv_26, ui->lineEdit_Hv_27, ui->lineEdit_Hv_28, ui->lineEdit_Hv_29,ui->lineEdit_Hv_30, ui->lineEdit_Hv_31},
    { ui->lineEdit_Hv_32, ui->lineEdit_Hv_33, ui->lineEdit_Hv_34, ui->lineEdit_Hv_35, ui->lineEdit_Hv_36, ui->lineEdit_Hv_37,ui->lineEdit_Hv_38, ui->lineEdit_Hv_39,
      ui->lineEdit_Hv_40, ui->lineEdit_Hv_41, ui->lineEdit_Hv_42, ui->lineEdit_Hv_43, ui->lineEdit_Hv_44, ui->lineEdit_Hv_45,ui->lineEdit_Hv_46, ui->lineEdit_Hv_47},
    { ui->lineEdit_Hv_48, ui->lineEdit_Hv_49, ui->lineEdit_Hv_50, ui->lineEdit_Hv_51, ui->lineEdit_Hv_52, ui->lineEdit_Hv_53,ui->lineEdit_Hv_54, ui->lineEdit_Hv_55,
      ui->lineEdit_Hv_56, ui->lineEdit_Hv_57, ui->lineEdit_Hv_58, ui->lineEdit_Hv_59,  ui->lineEdit_Hv_60, ui->lineEdit_Hv_61,ui->lineEdit_Hv_62, ui->lineEdit_Hv_63}
};
QCheckBox *pos[NB_HVMOD]= { ui->checkBox_HvPos_0, ui->checkBox_HvPos_1, ui->checkBox_HvPos_2, ui->checkBox_HvPos_3 };
QCheckBox *on[NB_HVMOD] = { ui->checkBox_HvOn_0, ui->checkBox_HvOn_1, ui->checkBox_HvOn_2, ui->checkBox_HvOn_3};
QCheckBox *act[NB_HVMOD]= { ui->checkBox_HvAct_0, ui->checkBox_HvAct_1, ui->checkBox_HvAct_2, ui->checkBox_HvAct_3};

	if(ui->radioButton_Hv_Order->isChecked()) {
		for(int mod=0; mod<NB_HVMOD; mod++) {
			for(int chan=0; chan<NB_HVCHAN_MOD; chan++) {
                hv[mod][chan]->setText(QString::number((double)p_hvdt->hvmod[mod].hvchan[chan].order / 10.0, 'f', 1));
                hv[mod][chan]->setToolTip(QString("High Voltage Order Module %1 Channel %2").arg(mod).arg(chan));
            }
		}
	} else {
			for(int mod=0; mod<NB_HVMOD; mod++) {
			for(int chan=0; chan<NB_HVCHAN_MOD; chan++) {
                hv[mod][chan]->setText(QString::number((double)p_hvdt->hvmod[mod].hvchan[chan].val / 10.0, 'f', 1));
                hv[mod][chan]->setToolTip(QString("High Voltage Order Module %1 Channel %2").arg(mod).arg(chan));
			} 
		}
	} 
    for(int mod=0; mod<NB_HVMOD; mod++) {
        pos[mod]->setChecked(p_hvdt->hvmod[mod].hvstat.positive);
        on[mod]->setChecked(p_hvdt->hvmod[mod].hvstat.positive);
        act[mod]->setChecked(p_hvdt->hvmod[mod].hvstat.actived);

        if(p_hvdt->hvmod[mod].hvstat.positive) {
            pos[mod]->setToolTip("High Voltage  : Positive");
        } else {
            pos[mod]->setToolTip("High Voltage  : Negative");
        }
        if(p_hvdt->hvmod[mod].hvstat.on) {
            on[mod]->setToolTip("High Voltage  : On");
        } else {
            on[mod]->setToolTip("High Voltage  : Off");
        }
        if(p_hvdt->hvmod[mod].hvstat.actived) {
            act[mod]->setToolTip("Module : Actived");
        } else {
            act[mod]->setToolTip("Module : Not Actived");
        }

    }
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::checkid(int chan, QString sid, u32 use)
{
	bool ok;
	u16 id = sid.toUShort(&ok, 16);
	m_frontend[chan] = id;
	m_usemask |= (use & 0x1) << chan;
 //   cout << "sid :"<< sid.toStdString() << " id:" << id << " chan:" << chan << " use:" << use << " usemask:" << m_usemask << endl;
}   // checkid

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_tabWidget_currentChanged(int index)
{
 //   cout << "current tab " << index << endl;
	switch (index) {
	case TABAMC :        // AmcTab
		AmcDisplay();
		break;
	case TABASM :        // AmsTab
		AsmDisplay(ui->spinBox_AsmTab_Sel->value());
		break;
	case TABTHOR :        // ThorTab
		ThorDisplay();
		break;
	case TABHV :          // Hv Tab
		HvDisplay();
		break;
	case TABMISC :        // MiscTab
		MiscDisplay();
		break;
	}
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_pushButton_MiscTab_clicked()
{
	MiscDisplay();
}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::MiscDisplay(void)
{
	Qt::CheckState state;
	QLineEdit * id[NB_CHAN] ={ ui->lineEdit_AsmId_MiscTab_0, ui->lineEdit_AsmId_MiscTab_1,ui->lineEdit_AsmId_MiscTab_2, ui->lineEdit_AsmId_MiscTab_3,
							   ui->lineEdit_AsmId_MiscTab_4, ui->lineEdit_AsmId_MiscTab_5,ui->lineEdit_AsmId_MiscTab_6, ui->lineEdit_AsmId_MiscTab_7,
							   ui->lineEdit_AsmId_MiscTab_8, ui->lineEdit_AsmId_MiscTab_9,ui->lineEdit_AsmId_MiscTab_10,ui->lineEdit_AsmId_MiscTab_11,
							   ui->lineEdit_ThorId_MiscTab};
	QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm_MiscTab_0, ui->checkBox_Asm_MiscTab_1, ui->checkBox_Asm_MiscTab_2, ui->checkBox_Asm_MiscTab_3,
							   ui->checkBox_Asm_MiscTab_4, ui->checkBox_Asm_MiscTab_5, ui->checkBox_Asm_MiscTab_6, ui->checkBox_Asm_MiscTab_7,
							   ui->checkBox_Asm_MiscTab_8, ui->checkBox_Asm_MiscTab_9, ui->checkBox_Asm_MiscTab_10, ui->checkBox_Asm_MiscTab_11,
							   ui->checkBox_Thor_MiscTab};
	ui->lineEdit_Ip_MiscTab->setText(tcp.GetServer());
	ui->lineEdit_Port_MiscTab->setText(QString::number(tcp.GetPort()));
	QString str;
	switch(m_mode) {
		case 0  : str = "0_Raw"; break;
		case 1  : str = "1_Normal"; break;
		default : str = QString::number(m_mode) + "_Unknow";
	}
	ui->lineEdit_Mode_MiscTab->setText(str);
	ui->lineEdit_Samples_MiscTab->setText(QString::number(m_samples));
	ui->lineEdit_Verbose_MiscTab->setText(QString::number(m_verbose));
	ui->lineEdit_NiosVerbose_MiscTab->setText(QString::number(m_niosverbose));
	for(int i=0; i<NB_CHAN; i++) {
 //       cout << i << " frontend :" << m_frontend[i] << endl;
		id[i]->setText(QString("0x%1").arg(m_frontend[i], 2, 16, QChar('0')));
		state = (((m_usemask >> i) & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
		ck[i]->setCheckState(state);
	}
	ui->checkBox_MisTab->setCheckState(m_auto);
	ui->spinBox_MiscTab->setValue(m_interv);
	ui->lineEdit_Ip_MiscTab_3->setText(hv.GetServer());
	ui->lineEdit_Port_MiscTab_3->setText(QString::number(hv.GetPort()));
}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_spinBox_AsmTab_Sel_valueChanged(const QString &arg1)
{
	AsmDisplay(arg1.toInt());
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_spinBox_AsmTab_Sel_editingFinished()
{
	AsmDisplay(ui->spinBox_AsmTab_Sel->value());
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_pushButton_AmcTab_Do_clicked()
{
	if(amc.Dump() == NO_ERROR)
		AmcDisplay();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_pushButton_AsmTab_Do_clicked()
{
	int chan = ui->spinBox_AsmTab_Sel->value();
//	cout << "Asm to display " << chan << endl;
	u32 mask = 1 << chan;

	if(m_usemask & mask) {
		if(asmm.Dump(mask) == NO_ERROR)
			AsmDisplay(chan);
	} else {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::warning(this, "Not Actived Asm", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
		if(reply == QMessageBox::Yes) {
			if(asmm.Dump(mask) == NO_ERROR)
				AsmDisplay(chan);
		}
	}
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on__ThorTab_Do_clicked()
{
	if(thor.Dump() == NO_ERROR)
		ThorDisplay();
}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_pushButton_Hv_Refresh_clicked()
{
    if(hv.Dump() == NO_ERROR)
        HvDisplay();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_radioButton_Hv_Order_clicked()
{
    HvDisplay();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_radioButton_Hv_Val_clicked()
{
    HvDisplay();
}
//-------------------------------------------------
//-------------------------------------------------

//=================================================
//=================================================
//=================================================
// Menu AMC
//=================================================
//=================================================
void MainWindow::on_actionIdentification_triggered()
{
	dial_amcid dl(&amc);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionReboot_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Reboot Nios", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//      amc.Reboot();
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, BROADCAST_FE, 4);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionReset_triggered()
{
	Dial_AmcReset dl(&amc, m_usemask);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionNios_Verbose_triggered()
{
	Dial_Verbose dl(true, &(p_amcdt->jtaguart_verbose));
	dl.setModal(true);
	dl.exec();
	amc.JtagCmd(p_amcdt->jtaguart_verbose);
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionStart_Amc_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Amc Daq Start", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        amc.RunCmd(1,0);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, BROADCAST_FE, 2);
	dl.setModal(true);
	dl.exec();

}
//------------------------------------------------
void MainWindow::on_actionStop_Amc_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Amc Daq Stop", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        amc.RunCmd(0,0);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, BROADCAST_FE, 3);
	dl.setModal(true);
	dl.exec();

}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionRead_Write_Registers_triggered()
{
	Dial_AmcReg dl(&amc);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionCDCE_Registers_triggered()
{
	Dial_AmcDCDCE dl(&amc);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionConfig_Amc_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Amc Config", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        amc.WriteRegister(1,m_usemask);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, BROADCAST_FE, 1);
	dl.setModal(true);
	dl.exec();

}

//=================================================
//=================================================
// Menu ASM
//=================================================
//=================================================
void MainWindow::on_actionStart_Asm_triggered()
{
	Dial_AsmDaq dl(&asmm,true, m_usemask);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionStop_Asm_triggered()
{
    Dial_AsmDaq dl(&asmm,false, m_usemask);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionRead_Write_Register_triggered()
{
	Dial_AsmReg dl(&asmm, m_usemask , &m_frontend[0]);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionUpdate_Change_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Asm Change", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        asmm.ChangCmd(m_usemask,BROADCAST_FE);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, BROADCAST_FE, 11);
	dl.setModal(true);
	dl.exec();

}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionConfig_Asm_triggered()
{
   Dial_AsmConfig dl(&asmm, m_usemask, &m_frontend[0]);
   dl.setModal(true);
   dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionReset_Fpga_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Asm Reset FPGA", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        asmm.ResetCmd(m_usemask,BROADCAST_FE);
    Dial_Confirm dl(&amc, &asmm, &thor, 0xfff, BROADCAST_FE, 10); //m_usemask
	dl.setModal(true);
	dl.exec();

}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionProgrammation_triggered()
{
	Dial_AsmProg dl(&asmm, m_usemask, &m_frontend[0]);
	dl.setModal(true);
	dl.exec();
}

//=================================================
//=================================================
// Menu Thor
//=================================================
//=================================================
void MainWindow::on_actionStart_Thor_triggered()
{
	Dial_ThorDaq dl(&thor,true, m_usemask, m_frontend[THOR_CHAN]);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionStop_Thor_triggered()
{
	Dial_ThorDaq dl(&thor,false, m_usemask, m_frontend[THOR_CHAN]);
	dl.setModal(true);
	dl.exec();
}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionRead_Write_Register_2_triggered()
{
	Dial_ThorReg dl(&thor, m_usemask , m_frontend[THOR_CHAN]);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionUpdate_Change_2_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Thor Change", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        thor.ChangCmd(m_frontend[THOR_CHAN]);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, m_frontend[THOR_CHAN], 21);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionReset_Fpga_2_triggered()
{
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(this, "Asm Reset FPGA", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
//    if(reply == QMessageBox::Yes)
//        thor.ResetCmd(m_frontend[THOR_CHAN]);
	Dial_Confirm dl(&amc, &asmm, &thor, m_usemask, m_frontend[THOR_CHAN], 20);
	dl.setModal(true);
	dl.exec();

}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionConfig_Thor_triggered()
{
	Dial_ThorConfig dl(&thor, m_frontend[THOR_CHAN]);
	dl.setModal(true);
	dl.exec();
}
//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionProgammation_2_triggered()
{
	Dial_ThorProg dl(&thor, m_frontend[THOR_CHAN]);
	dl.setModal(true);
	dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------

//=================================================
//=================================================
//=================================================
//=================================================
//=================================================
void MainWindow::AmcTip(void)
{
    ui->lineEdit_AmcTab_PrjId->setToolTip("FPGA Project Indentifier");
    ui->lineEdit_AmcTab_Firm->setToolTip("FPGA Firmware Revision");
    ui->lineEdit_AmcTab_Hard->setToolTip("Board Hardware Revision");
    ui->lineEdit_AmcTab_Sn->setToolTip("FPGA Board Serial Number");
    ui->lineEdit_AmcTab_SysId->setToolTip("Nios System Identifier");
    ui->lineEdit_AmcTab_SysStamp->setToolTip("Nios Time of the Last Compilation");
    ui->lineEdit_AmcTab_Btime->setToolTip("FPGA Time of the last Compilation");
    ui->lineEdit_AmcTab_BDate->setToolTip("FPGA Date of the last Compilation");
    ui->lineEdit_AmcTab_Mask->setToolTip("In use Module\nBits 0..11 : Asm\n Bit 12 : Thor");
    ui->lineEdit_AmcTab_Cnfg->setToolTip("Global Configuration Register");
    ui->lineEdit_AmcTab_Stat->setToolTip("Global Status Register");
    ui->lineEdit_AmcTab_Signal->setToolTip("Gx Block Synchro");
    ui->lineEdit_AmcTab_NiosVerbose->setToolTip("Nios Verbose Level on JtagUart");
    ui->lineEdit_AmcTab_CnfgMode->setToolTip("Amc Configuration DAQ Type");
    ui->lineEdit_AmcTab_CnfgSamples->setToolTip("Amc Configuration Number of Samples");
    ui->lineEdit_AmcTab_CnfgDaq->setToolTip("Amc DAQ Status");
    ui->lineEdit_AmcTab_CnfgLeds->setToolTip("<b>Nios Leds Status</b>");
    ui->lineEdit_AmcTab_StatAsmCnfg->setToolTip("Asm Link Gx Block Reconfiguration");
    ui->lineEdit_AmcTab_StatAsmPll->setToolTip("Asm Link Gx Block Pll Status");
    ui->lineEdit_AmcTab_StatThorCnfg->setToolTip("Thor Link Gx Block Reconfiguration");
    ui->lineEdit_AsmTab_StatThorPll->setToolTip("Thor Link Gx Block Pll status");
    ui->lineEdit_AmcTab_StatThorSyn->setToolTip("Thor Link Synchronization");
    ui->lineEdit_AmcTab_StatUpd->setToolTip("10Gbe UDP Links Incomming Status ");

    QCheckBox *ck[NB_ASM]=
        { ui->checkBox_AcTab_StatAsmSyn_1, ui->checkBox_AcTab_StatAsmSyn_2, ui->checkBox_AcTab_StatAsmSyn_3, ui->checkBox_AcTab_StatAsmSyn_4,
          ui->checkBox_AcTab_StatAsmSyn_5, ui->checkBox_AcTab_StatAsmSyn_6, ui->checkBox_AcTab_StatAsmSyn_7, ui->checkBox_AcTab_StatAsmSyn_8,
          ui->checkBox_AcTab_StatAsmSyn_9, ui->checkBox_AcTab_StatAsmSyn_10, ui->checkBox_AcTab_StatAsmSyn_11, ui->checkBox_AcTab_StatAsmSyn_12};
    for( int i=0; i<NB_ASM; i++)
        ck[i]->setToolTip(QString("Asm %1 Link Synchronization").arg(i));
    QLineEdit * cd[NB_CDCE] [NB_CDCE_REG]= {
        { ui->lineEdit_AmcTab_CDCE00, ui->lineEdit_AmcTab_CDCE01, ui->lineEdit_AmcTab_CDCE02, ui->lineEdit_AmcTab_CDCE03, ui->lineEdit_AmcTab_CDCE04, ui->lineEdit_AmcTab_CDCE05, ui->lineEdit_AmcTab_CDCE06, ui->lineEdit_AmcTab_CDCE07, ui->lineEdit_AmcTab_CDCE08 },
        { ui->lineEdit_AmcTab_CDCE10, ui->lineEdit_AmcTab_CDCE11, ui->lineEdit_AmcTab_CDCE12, ui->lineEdit_AmcTab_CDCE13, ui->lineEdit_AmcTab_CDCE14, ui->lineEdit_AmcTab_CDCE15, ui->lineEdit_AmcTab_CDCE16, ui->lineEdit_AmcTab_CDCE17, ui->lineEdit_AmcTab_CDCE18 },
        { ui->lineEdit_AmcTab_CDCE20, ui->lineEdit_AmcTab_CDCE21, ui->lineEdit_AmcTab_CDCE22, ui->lineEdit_AmcTab_CDCE23, ui->lineEdit_AmcTab_CDCE24, ui->lineEdit_AmcTab_CDCE25, ui->lineEdit_AmcTab_CDCE26, ui->lineEdit_AmcTab_CDCE27, ui->lineEdit_AmcTab_CDCE28 }
    };
    for(int i=0 ; i<NB_CDCE; i++) {
        for(int j=0; j<NB_CDCE_REG; j++) {
            cd[i][j]->setToolTip(QString("Jitter Clearner Chip %1 Register %2 Value").arg(i).arg(j));
        }
    }
    ui->checkBox_AmcTab_Pause->setToolTip("Daq Pause Status");
    ui->lineEdit_AmcTab_CDCECnfg->setToolTip("Jitter Cleaner Configuration");
    ui->lineEdit_AmcTab_CDCEStat->setToolTip("Jitter Cleaner Status");

    ui->lineEdit_AmcTab_SrcMac->setToolTip("10Gbe Link 0 MAC Source Address");
    ui->lineEdit_AmcTab_SrcIp->setToolTip("10Gbe Link 0 IP Source Address");
    ui->lineEdit_AmcTab_SrcPort->setToolTip("10Gbe Link 0  Ip Source Port");
    ui->lineEdit_AmcTab_DestMac->setToolTip("10Gbe Link 0 MAC Destination Address");
    ui->lineEdit_AmcTab_DestIp->setToolTip("10Gbe Link 0 IP Destination Address");
    ui->lineEdit_AmcTab_DestPort->setToolTip("10Gbe Link 0 Ip Destination Port");

    ui->lineEdit_AmcTab_SrcMac_2->setToolTip("10Gbe Link 1 MAC Source Address");
    ui->lineEdit_AmcTab_SrcIp_2->setToolTip("10Gbe Link 1 IP Source Address");
    ui->lineEdit_AmcTab_SrcPort_2->setToolTip("10Gbe Link 1 Ip Source Port");
    ui->lineEdit_AmcTab_DestMac_3->setToolTip("10Gbe Link 1 MAC Destination Address");
    ui->lineEdit_AmcTab_DestIp_2->setToolTip("10Gbe Link 1 IP Destination Address");
    ui->lineEdit_AmcTab_DestPort_2->setToolTip("10Gbe Link 1 Ip Destination Port");

    ui->lineEdit_AmcTab_PtrigCnt->setToolTip("PreTrigger Counter");
    ui->lineEdit_AmcTab_TrigCnt->setToolTip("Trigger Counter");
    ui->lineEdit_AmcTab_LTemp->setToolTip("Amc Board Temperature");
    ui->lineEdit_AmcTab_FTemp->setToolTip("Amc FPGA Temperature");

    ui->pushButton_AmcTab_Do->setToolTip("Dump Data from Amc Tab\nRefresh Display");
 
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::AsmTip(void)
{
    ui->lineEdit_AsmTab_Id->setToolTip("Ams Board Identifier");
    ui->lineEdit_AsmTab_Sn->setToolTip("Asm Board Serial Number");
    ui->lineEdit_AsmTab_FirmRev->setToolTip("Asm Fpga Firmware Revision");
    ui->lineEdit_AsmTab_HardRev->setToolTip("Asm Board Hardware Rivision");
    ui->lineEdit_AsmTab_BTime->setToolTip("Asm FPGA Last Compilation Time");
    ui->lineEdit_AsmTab_BDate->setToolTip("Asm FPGA Last Compilation Date");
    ui->lineEdit_AsmTab_AddrConf->setToolTip("Asm Configuration REgister Address");
    ui->lineEdit_AsmTab_StatGx->setToolTip("Asm Status Register");
    ui->lineEdit_AsmTab_Cnt0->setToolTip("Asm Counter 0");
    ui->lineEdit_AsmTab_Cnt1->setToolTip("Asm Counter 1");
    ui->lineEdit_AsmTab_Cnt2->setToolTip("Asm Counter 2");
    ui->lineEdit_AsmTab_Cnt3->setToolTip("Asm Counter 3");
    ui->lineEdit_AsmTab_Cnt4->setToolTip("Asm Counter 4");
    ui->lineEdit_AsmTab_Cnt5->setToolTip("Asm Counter 5");
    ui->lineEdit_AsmTab_Cnt6->setToolTip("Asm Counter 6");
    ui->lineEdit_AsmTab_Cnt7->setToolTip("Asm Counter 7");
    ui->lineEdit_AsmTab_PllLock->setToolTip("Asm Pll Status");

    ui->checkBox_AsmTab_Actived->setToolTip("Selected Asm Actived status");
    ui->spinBox_AsmTab_Sel->setToolTip("Select Asm Board 0..11");
    ui->pushButton_AsmTab_Do->setToolTip("Dump Data from the Select Asm Tab\nRefresh Display");

    ui->lineEdit_AsmTab_FrontEnd->setToolTip("Asm Front End Number");
    ui->lineEdit_AsmTab_Mode->setToolTip("Asm Daq Mode");
    ui->lineEdit_AsmTab_ConfStrobe->setToolTip("Asm Configuration Register");

    QLineEdit *dc[ASM_NB_CHAN] = {
        ui->lineEdit_AsmTab_DacCm0,  ui->lineEdit_AsmTab_DacCm1,  ui->lineEdit_AsmTab_DacCm2,  ui->lineEdit_AsmTab_DacCm3,  ui->lineEdit_AsmTab_DacCm4,  ui->lineEdit_AsmTab_DacCm5,
        ui->lineEdit_AsmTab_DacCm6,  ui->lineEdit_AsmTab_DacCm7,  ui->lineEdit_AsmTab_DacCm8,  ui->lineEdit_AsmTab_DacCm9,  ui->lineEdit_AsmTab_DacCm10, ui->lineEdit_AsmTab_DacCm11,
        ui->lineEdit_AsmTab_DacCm12, ui->lineEdit_AsmTab_DacCm13, ui->lineEdit_AsmTab_DacCm14, ui->lineEdit_AsmTab_DacCm15, ui->lineEdit_AsmTab_DacCm16, ui->lineEdit_AsmTab_DacCm17,
        ui->lineEdit_AsmTab_DacCm18, ui->lineEdit_AsmTab_DacCm19, ui->lineEdit_AsmTab_DacCm20, ui->lineEdit_AsmTab_DacCm21, ui->lineEdit_AsmTab_DacCm22, ui->lineEdit_AsmTab_DacCm23};
     QLineEdit *th[ASM_NB_CHAN] ={
        ui->lineEdit_AsmTab_ThHigh0,  ui->lineEdit_AsmTab_ThHigh1,  ui->lineEdit_AsmTab_ThHigh2,  ui->lineEdit_AsmTab_ThHigh3,  ui->lineEdit_AsmTab_ThHigh4,  ui->lineEdit_AsmTab_ThHigh5,
        ui->lineEdit_AsmTab_ThHigh6,  ui->lineEdit_AsmTab_ThHigh7,  ui->lineEdit_AsmTab_ThHigh8,  ui->lineEdit_AsmTab_ThHigh9,  ui->lineEdit_AsmTab_ThHigh10, ui->lineEdit_AsmTab_ThHigh11,
        ui->lineEdit_AsmTab_ThHigh12, ui->lineEdit_AsmTab_ThHigh13, ui->lineEdit_AsmTab_ThHigh14, ui->lineEdit_AsmTab_ThHigh15, ui->lineEdit_AsmTab_ThHigh16, ui->lineEdit_AsmTab_ThHigh17,
        ui->lineEdit_AsmTab_ThHigh18, ui->lineEdit_AsmTab_ThHigh19, ui->lineEdit_AsmTab_ThHigh20, ui->lineEdit_AsmTab_ThHigh21, ui->lineEdit_AsmTab_ThHigh22, ui->lineEdit_AsmTab_ThHigh23 };
    QLineEdit *tl[ASM_NB_CHAN] ={
        ui->lineEdit_AsmTab_ThLow0,  ui->lineEdit_AsmTab_ThLow1,  ui->lineEdit_AsmTab_ThLow2,  ui->lineEdit_AsmTab_ThLow3,  ui->lineEdit_AsmTab_ThLow4,  ui->lineEdit_AsmTab_ThLow5,
        ui->lineEdit_AsmTab_ThLow6,  ui->lineEdit_AsmTab_ThLow7,  ui->lineEdit_AsmTab_ThLow8,  ui->lineEdit_AsmTab_ThLow9,  ui->lineEdit_AsmTab_ThLow10, ui->lineEdit_AsmTab_ThLow11,
        ui->lineEdit_AsmTab_ThLow12, ui->lineEdit_AsmTab_ThLow13, ui->lineEdit_AsmTab_ThLow14, ui->lineEdit_AsmTab_ThLow15, ui->lineEdit_AsmTab_ThLow16, ui->lineEdit_AsmTab_ThLow17,
        ui->lineEdit_AsmTab_ThLow18, ui->lineEdit_AsmTab_ThLow19, ui->lineEdit_AsmTab_ThLow20, ui->lineEdit_AsmTab_ThLow21, ui->lineEdit_AsmTab_ThLow22, ui->lineEdit_AsmTab_ThLow23 };

    for(int i=0; i<ASM_NB_CHAN; i++) {
        dc[i]->setToolTip(QString("Asm Channel %1 Dac CM Value").arg(i));
        th[i]->setToolTip(QString("Asm Channel %1 Threshold.High").arg(i));
        tl[i]->setToolTip(QString("Asm Channel %1 Threshold.Low").arg(i));
    }
    QLineEdit *cd[NB_CDCE_REG] = {
        ui->lineEdit_AsmTab_CDCE0, ui->lineEdit_AsmTab_CDCE1, ui->lineEdit_AsmTab_CDCE2, ui->lineEdit_AsmTab_CDCE3, ui->lineEdit_AsmTab_CDCE4,
        ui->lineEdit_AsmTab_CDCE5, ui->lineEdit_AsmTab_CDCE6, ui->lineEdit_AsmTab_CDCE7, ui->lineEdit_AsmTab_CDCE8 };
    for(int i=0; i<NB_CDCE_REG; i++)
        cd[i]->setToolTip(QString("Jitterv Cleaner Chip Register %1 Value").arg(i));
    QLineEdit *dr[ASM_NB_ROFS] = {
        ui->lineEdit_AsmTab_DacRofs0, ui->lineEdit_AsmTab_DacRofs1, ui->lineEdit_AsmTab_DacRofs2, ui->lineEdit_AsmTab_DacRofs3,ui->lineEdit_AsmTab_DacRofs4, ui->lineEdit_AsmTab_DacRofs5 };
    for(int i=0; i<ASM_NB_ROFS; i++)
        dr[i]->setToolTip(QString("Asm Dac Rofs %1 Value").arg(i));
    ui->lineEdit_AsmTab_Samples->setToolTip("Asm Number of Samples");
    ui->lineEdit_AsmTab_ChanRead->setToolTip("Asm Channel Read");
    ui->lineEdit_AsmTab_Width->setToolTip("Asm Width Register");
    ui->lineEdit_AsmTab_Delay->setToolTip("Asm Delay Register");
    ui->lineEdit_AsmTab_Filter_Thr_H->setToolTip("Asm Filter Threshold High");
    ui->lineEdit_AsmTab_Filter_Thr_L->setToolTip("Asm Filter Threshold Low");
    ui->lineEdit_AsmTab_Offset->setToolTip("Asm Thor Offset");
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::ThorTip(void)
{
    ui->lineEdit_ThorTab_BoardId->setToolTip("Thor Board Indentifier");
    ui->lineEdit_ThorTab_Sn->setToolTip("Thor Board Serial Number");
    ui->lineEdit_ThorTab_FirmRev->setToolTip("Thor FPGA Firmware Revision");
    ui->lineEdit_ThorTab_HardRev->setToolTip("Thor Board HardWare Revision");
    ui->lineEdit_ThorTab_BTime->setToolTip("Thor Fpga Last Compilation Time");
    ui->lineEdit_ThorTab_BDate->setToolTip("Thor Fpga Last Compilation Date");

    ui->lineEdit_ThorTab_AddrConf->setToolTip("Thor Configuration Register Address");
    ui->lineEdit_ThorTab_GxStatus->setToolTip("Thor Gx link Block Status");
    ui->lineEdit_ThorTab_TrigStamp->setToolTip("Thor Trigger Stamp");
    ui->lineEdit_ThorTab_Used->setToolTip("Thor PreTrigger FifoUsed");
    ui->lineEdit_ThorTab_Status->setToolTip("Thor Global Status");
    ui->lineEdit_ThorTab_TT->setToolTip("Thor Last Event Trigger Type");
    ui->lineEdit_ThorTab_TrigPat->setToolTip("Thor Trigger Pattern");

    ui->checkBox_ThorTab_Actived->setToolTip("Thor Actived Status");
    ui->_ThorTab_Do->setToolTip("Dump Data from the Thor Tab\nRefresh Display");

    ui->lineEdit_ThorTab_FrontEnd->setToolTip("Thor Front End Number");
    ui->lineEdit_ThorTab_TrigType->setToolTip("Thor Trigger Type");
    ui->lineEdit_ThorTab_ConfStrobe->setToolTip("Thor Configuration Strobe Register");
    QLineEdit *cd[NB_CDCE_REG] = {
        ui->lineEdit_ThorTab_CDCE0, ui->lineEdit_ThorTab_CDCE1, ui->lineEdit_ThorTab_CDCE2, ui->lineEdit_ThorTab_CDCE3, ui->lineEdit_ThorTab_CDCE4, ui->lineEdit_ThorTab_CDCE5, ui->lineEdit_ThorTab_CDCE6, ui->lineEdit_ThorTab_CDCE7, ui->lineEdit_ThorTab_CDCE8};
    for(int i=0; i<NB_CDCE_REG; i++)
        cd[i]->setToolTip(QString("Thor Jitter Cleaner Register %1 value").arg(i));
    ui->lineEdit_ThorTab_PTrigMask->setToolTip("Thor Pre Trigger Mask");
    ui->lineEdit_ThorTab_PTrigFreq->setToolTip("Thor PreTrigger Pedestal Frequency");
    ui->lineEdit_ThorTab_TrigShape->setToolTip("Thor Trigger Shape Duration");
    ui->lineEdit_ThorTab_TrigDelay->setToolTip("Thor Trigger Delay Duration");

    ui->checkBox_ThorTab_St_0->setToolTip("Thor C Pll Lock Status");
    ui->checkBox_ThorTab_St_1->setToolTip("Thor Main Pll Status");
    ui->checkBox_ThorTab_St_2->setToolTip("Thor System Pll Status");
    ui->checkBox_ThorTab_St_3->setToolTip("Thor TDC Pll Status");
    ui->checkBox_ThorTab_St_4->setToolTip("Thor Rx Synchro Status");
    ui->checkBox_ThorTab_St_5->setToolTip("Thor DCSFifo Status");
    ui->checkBox_ThorTab_St_6->setToolTip("Thor Rx Fifo Status");
    ui->checkBox_ThorTab_St_7->setToolTip("Thor Pretrigger Fifo Status");
    ui->checkBox_ThorTab_St_8->setToolTip("Thor Reserved Status");
    ui->checkBox_ThorTab_St_9->setToolTip("Thor Daq Run Status");
    ui->checkBox_ThorTab_St_10->setToolTip("Thor Daq Pause Status");
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::MiscTip(void)
{
    QLineEdit * id[NB_CHAN] ={ ui->lineEdit_AsmId_MiscTab_0, ui->lineEdit_AsmId_MiscTab_1,ui->lineEdit_AsmId_MiscTab_2, ui->lineEdit_AsmId_MiscTab_3,
                               ui->lineEdit_AsmId_MiscTab_4, ui->lineEdit_AsmId_MiscTab_5,ui->lineEdit_AsmId_MiscTab_6, ui->lineEdit_AsmId_MiscTab_7,
                               ui->lineEdit_AsmId_MiscTab_8, ui->lineEdit_AsmId_MiscTab_9,ui->lineEdit_AsmId_MiscTab_10,ui->lineEdit_AsmId_MiscTab_11,
                               ui->lineEdit_ThorId_MiscTab};
    QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm_MiscTab_0, ui->checkBox_Asm_MiscTab_1, ui->checkBox_Asm_MiscTab_2, ui->checkBox_Asm_MiscTab_3,
                               ui->checkBox_Asm_MiscTab_4, ui->checkBox_Asm_MiscTab_5, ui->checkBox_Asm_MiscTab_6, ui->checkBox_Asm_MiscTab_7,
                               ui->checkBox_Asm_MiscTab_8, ui->checkBox_Asm_MiscTab_9, ui->checkBox_Asm_MiscTab_10, ui->checkBox_Asm_MiscTab_11,
                               ui->checkBox_Thor_MiscTab};
    ui->lineEdit_Ip_MiscTab->setToolTip("IP Target Address");
    ui->lineEdit_Port_MiscTab->setToolTip("IP Target Port");
    ui->lineEdit_Mode_MiscTab->setToolTip("Daq Mode");
    ui->lineEdit_Samples_MiscTab->setToolTip("Daq Number of Samples");
    ui->lineEdit_Verbose_MiscTab->setToolTip("Main Programm Verbose Level");
    ui->lineEdit_NiosVerbose_MiscTab->setToolTip("Nios Verbose Level");
    for(int i=0; i<NB_CHAN; i++) {
        id[i]->setToolTip(QString("Board %1 Frontend").arg(i));
        ck[i]->setToolTip(QString("Board %1 Actived Status").arg(i));
    }
    ui->checkBox_MisTab->setToolTip("Auto Display Status");
    ui->spinBox_MiscTab->setToolTip("Auto Display Inreval");
    ui->pushButton_MiscTab->setToolTip("Dump Data from the Miscellaneous Tab\nRefresh Display");
	 ui->lineEdit_Ip_MiscTab_3->setToolTip("High Voltage Server IP Address");
	 ui->lineEdit_Port_MiscTab_3->setToolTip("High Voltage Server Port");

}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::HvTip(void)
{
    ui->radioButton_Hv_Order->setToolTip("Hv Orders Display Select");
    ui->radioButton_Hv_Val->setToolTip("Hv Values Display Select");
    ui->pushButton_Hv_Refresh->setToolTip("Refresh Action");
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionLoad_From_file_triggered()
{
    Dial_HvConfig  dl(&hv);
    dl.setModal(true);
    dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionHV_On_Off_triggered()
{
    Dial_HvONOff dl(&hv);
    dl.setModal(true);
    dl.exec();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionStop_Server_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Stop Hv Server", "Are you sure ?", QMessageBox::Yes | QMessageBox::No );
    if(reply == QMessageBox::Yes)
        hv.ServerStop();
}

//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionFinder_triggered()
{
    Dial_finder dl(&asmm, &thor, & m_frontend[0]);
    dl.setModal(true);
    dl.exec();
    on_pushButton_AsmTab_Do_clicked();
}


//-------------------------------------------------
//-------------------------------------------------
void MainWindow::on_actionHelp_triggered()
{
	QString file;
	char * dirdcs = getenv("DIR_DCS");
#ifdef Q_OS_OSX
	 if(dirdcs) {
		file = QString("/Applications/Firefox.app/Contents/MacOs/firefox %1/QtDpgaDcs/help/index.html").arg(dirdcs); 
	 } else {
		file = QString("/Applications/Firefox.app/Contents/MacOs/firefox ../../../../QtDpgaDcs/help/index.html");
	 }
#else
	 if(dirdcs) {
		file = QString("/usr/bin/firefox %1/QtDpgaDcs/help/index.html").arg(dirdcs); 
	 } else { 
		file = QString("/usr/bin/firefox ../help/index.html");
	 }
#endif

	cout << file.toStdString() << endl;
	QProcess process;
	process.startDetached(file);    // concurent prog
//		QProcess::execute(file);     // exculsive prog

}

//=================================================
//=================================================

void MainWindow::on_SelectCalib_clicked()
{
    Dial_calib dl(&amc, &asmm, &thor,ui->spinBox_AsmTab_Sel->value(), & m_frontend[0],m_usemask);
    dl.setModal(false);
    dl.exec();
    on_pushButton_AsmTab_Do_clicked();
}


void MainWindow::on_pushButton_clicked()
{
    dial_1Asm dl(&asmm,&hv,ui->spinBox_AsmTab_Sel->value(), & m_frontend[0],PathDir);
    dl.setModal(true);
    dl.exec();
 //   on_pushButton_AsmTab_Do_clicked();

}

void MainWindow::on_actionMonitoring_triggered()
{
    QStringList arguments;
    if (myProcess->state() == QProcess::NotRunning) {
        myProcess->start(PathMonitor, arguments);
        qDebug() << myProcess->program() << "State <= " << myProcess->state();
    }
}
