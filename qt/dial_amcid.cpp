//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dialamcid.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//================================================
//================================================
//================================================
#include "dial_amcid.h"
#include "ui_dial_amcid.h"

#include "command_error.h"

//================================================
dial_amcid::dial_amcid(CAmc *amc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dial_amcid)
{
    p_amc = amc;
    ui->setupUi(this);
    p_amc->SetMsgBox(ui->textEdit_Msg);
    
    ui->lineEdit_AmcId_PrjId->setToolTip("Project Identifier");
    ui->lineEdit_AmcId_FirmRev->setToolTip("Amc FPGA Firmware Revision");
    ui->lineEdit_AmcId_HardRev->setToolTip("Amc Board Hardware Revision");
    ui->lineEdit_AmcId_Sn->setToolTip("Amc Board Serial Number");
    ui->lineEdit_AmcId_SysId->setToolTip("Nios System Identifier");
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->pushButton_AmcId_Do->setToolTip("Refresh Display Action");
    ui->pushButton_AmcId_Quit->setToolTip("Close this window");
    
}

//================================================
dial_amcid::~dial_amcid()
{
    p_amc->SetMsgBox(NULL);
    delete ui;
}

//================================================
//================================================
void dial_amcid::on_pushButton_AmcId_Do_clicked()
{
	QString str[5] = {"Nav","Nav","Nav","Nav","Nav"} ;
	QColor c = ui->textEdit_Msg->textColor();
	ui->textEdit_Msg->clear();
	ui->textEdit_Msg->append(" Amc Get Indentity");
	if(p_amc->GetIdentity() == NO_ERROR)
	{
		u32 u  = QsOrdorn(p_amc->GetPrjId());
		char *c= (char*) &u;
		str[0] = QString::fromLocal8Bit(c,4);
		str[1] = QString("%1.%2").arg((p_amc->GetFirmRev() >> 16) & 0xffff).arg(p_amc->GetFirmRev() & 0xffff,2, 10, QChar('0'));
		u      = QsOrdorn(p_amc->GetHardRev());
		c      = (char*) &u;
		str[2] = QString::fromLocal8Bit(c,4);
		str[3] = QString::number(p_amc->GetSN());
		str[4] = QString::number(p_amc->GetSysId());
		ui->textEdit_Msg->setTextColor("green");
		ui->textEdit_Msg->append("Operation Successfull");
	} else {
		ui->textEdit_Msg->setTextColor("red");
		ui->textEdit_Msg->append("Operation Failed");
	}
	ui->textEdit_Msg->setTextColor(c);
	ui->lineEdit_AmcId_PrjId->setText(str[0]);
	ui->lineEdit_AmcId_FirmRev->setText(str[1]);
	ui->lineEdit_AmcId_HardRev->setText(str[2]);
	ui->lineEdit_AmcId_Sn->setText(str[3]);
	ui->lineEdit_AmcId_SysId->setText(str[4]);
}

//================================================
//================================================
void dial_amcid::on_pushButton_AmcId_Quit_clicked()
{
    this->close();
}

//================================================
//================================================
