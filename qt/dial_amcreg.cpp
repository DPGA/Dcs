//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcreg.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <iostream>
using namespace std;


#include "dial_amcreg.h"
#include "ui_dial_amcreg.h"
#include "command_error.h"

//==============================================
Dial_AmcReg::Dial_AmcReg(CAmc * amc, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dial_AmcReg)
{
	p_amc = amc;
	m_addr = 0;
	m_rval = 0;
	m_wval = 0;
	ui->setupUi(this);
	p_amc->SetMsgBox(ui->textEdit_Msg);
	for(int i=0; i<NB_AUTHORIZATION; i++)
		ui->listWidget->addItem(reglist[i].nam);
	ui->lineEdit_AmcReg_Addr->setReadOnly(true);
    ui->listWidget->setCurrentRow(0);
	display();
	ui->checkBox_AmcReg->setToolTip("Hexadecimal or Decimal Display");
	ui->lineEdit_AmcReg_Addr->setToolTip("Register Address");
	ui->lineEdit_AmcReg_Read->setToolTip("Read Value at");
	ui->lineEdit_AmcReg_Write->setToolTip("Value to write to the Address");
	ui->listWidget->setToolTip("Authorized Register Select");
	ui->textEdit_Msg->setToolTip("Message Box");
	ui->pushButton_AmcReg_Read ->setToolTip("Read Action");
	ui->pushButton_AmcReg_Write->setToolTip("Write Action");
	ui->pushButton_AmcReg_Cancel->setToolTip("Close this window"); 
}

//==============================================
Dial_AmcReg::~Dial_AmcReg()
{
	p_amc->SetMsgBox(NULL);
	delete ui;
}

//==============================================
void Dial_AmcReg::on_checkBox_AmcReg_clicked()
{
	display();
}

//==============================================
void Dial_AmcReg::on_pushButton_AmcReg_Read_clicked()
{
	bool ok;
	QColor c = ui->textEdit_Msg->textColor();
	QString str = ui->lineEdit_AmcReg_Addr->text();
	m_addr = str.toLong(&ok, m_base);
	ui->textEdit_Msg->clear();
	ui->textEdit_Msg->append(QString("Amc Read Register 0x%1").arg(m_addr, 8, 16, QChar('0')));
	if(p_amc->ReadRegister(m_addr, &m_rval) == NO_ERROR) {
		ui->textEdit_Msg->setTextColor("green");
		ui->textEdit_Msg->append("Operation Successfull");
	} else {
		ui->textEdit_Msg->setTextColor("red");
		ui->textEdit_Msg->append("Operation Failed");
	}
	ui->textEdit_Msg->setTextColor(c);
	display();
}

//==============================================
void Dial_AmcReg::on_pushButton_AmcReg_Write_clicked()
{
    int row = ui->listWidget->currentRow();
    if(reglist[row].nam == tr("Others")) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this,
                                                                   tr("Dangerous"),
                                                                   tr("Realy, You are Sure?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox:: Yes,
                                                                   QMessageBox::Yes);
        if(resBtn != QMessageBox::Yes)
            return;
    }
	bool ok;
	QColor c = ui->textEdit_Msg->textColor();
	QString str = ui->lineEdit_AmcReg_Addr->text();
	m_addr = str.toLong(&ok, m_base);
	str = ui->lineEdit_AmcReg_Write->text();
	m_wval = str.toLong(&ok, m_base);
	ui->textEdit_Msg->clear();
	ui->textEdit_Msg->append(QString("Amc Write Register 0x%1 with 0x%2").arg(m_addr,  8, 16, QChar('0')).arg(m_wval, 8, 16, QChar('0')));
	if(p_amc->WriteRegister(m_addr, m_wval)== NO_ERROR) {
		ui->textEdit_Msg->setTextColor("green");
		ui->textEdit_Msg->append("Operation Successfull");
	} else {
		ui->textEdit_Msg->setTextColor("red");
		ui->textEdit_Msg->append("Operation Failed");
	}
	ui->textEdit_Msg->setTextColor(c);
}

//==============================================
void Dial_AmcReg::on_pushButton_AmcReg_Cancel_clicked()
{
	this->close();
}
//==============================================
//==============================================
void Dial_AmcReg::display(void)
{
	int row = ui->listWidget->currentRow();
    cout << "row " << row << " -> 0x" << hex << reglist[row].addr << dec << endl;
    if(reglist[row].nam == tr("Others")) {
        ui->lineEdit_AmcReg_Addr->setReadOnly(false);
    } else {
        ui->lineEdit_AmcReg_Addr->setReadOnly(true);
        m_addr = reglist[row].addr;
    }
	m_base = (ui->checkBox_AmcReg->isChecked()) ? 16 :10;
	ui->lineEdit_AmcReg_Addr->setText(QString::number(m_addr,m_base));
	ui->lineEdit_AmcReg_Read->setText(QString::number(m_rval,m_base));
	ui->lineEdit_AmcReg_Write->setText(QString::number(m_wval,m_base));

	ui->pushButton_AmcReg_Write->setEnabled(reglist[row].wr);
}

//==============================================
//==============================================
//==============================================
//==============================================

void Dial_AmcReg::on_listWidget_itemSelectionChanged()
{
 //  int row = ui->listWidget->currentRow();
 //  cout << "row " << row << " -> 0x" << hex << reglist[row].addr << dec << endl;
 //  m_addr = reglist[row].addr;
	display();
}
//==============================================
//==============================================
//==============================================
//==============================================
