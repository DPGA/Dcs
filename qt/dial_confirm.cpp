//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_confirm.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "dial_confirm.h"
#include "ui_dial_confirm.h"

//===============================================
//===============================================
Dial_Confirm::Dial_Confirm(CAmc *amc, CAsm *asmm, CThor *thor, u32 mask, u16 frontend, u16 cmd, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_Confirm)
{
    p_amc = amc;
    p_asm = asmm;
    p_thor = thor;
    m_mask = mask;
    m_frontend = frontend;
    m_cmd = cmd;
    ui->setupUi(this);
    p_amc->SetMsgBox(ui->textEdit_Msg);
    p_asm->SetMsgBox(ui->textEdit_Msg);
    p_thor->SetMsgBox(ui->textEdit_Msg);
    QString str,bp;

    switch(m_cmd) {
        case 1  : str = "Amc Configuration"; bp = "Configuration"; break;
        case 2  : str = "Amc Daq Start";     bp = "DAQ Start";     break;
        case 3  : str = "Amc Daq Stop";      bp = "DAQ Stop";      break;
        case 4  : str = "Amc Nios Reboot";   bp = "NIOS Reboot";   break;

        case 10 : str = "Asm Reset FPGA";    bp = "FPGA Reset" ;   break;
        case 11 : str = "Amc Change";        bp = "Update";        break;

        case 20 : str = "Thor Reset FPGA";   bp = "FPGA Reset";    break;
        case 21 : str = "Thor Change";       bp = "Update";        break;

        default : str = "Unknown Question";
    }
    this->setWindowTitle(str);
    ui->pushButton_Yes->setText(bp);
    ui->pushButton_Yes->setToolTip(str + " Action");
    ui->pushButton_No->setToolTip("Close This window");
    ui->textEdit_Msg->setToolTip("Message Box");
}

//===============================================
//===============================================
Dial_Confirm::~Dial_Confirm()
{
    p_amc->SetMsgBox(NULL);
    p_asm->SetMsgBox(NULL);
    p_thor->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_Confirm::on_pushButton_Yes_clicked()
{
    int ret;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();

    switch(m_cmd) {
        case 1 : ret = p_amc->WriteRegister(1,m_mask); break;
        case 2 : ret = p_amc->RunCmd(1,0); break;
        case 3 : ret = p_amc->RunCmd(0,0); break;
        case 4 : ret = NO_ERROR; p_amc->Reboot(); break;

        case 10 : ret = p_asm->ResetCmd(m_mask, m_frontend); break;
        case 11 : ret = p_asm->ChangCmd(m_mask, m_frontend); break;

        case 20 : ret = p_thor->ResetCmd(m_frontend); break;
        case 21 : ret = p_thor->ChangCmd(m_frontend); break;

        default : ret = UNKNOW_CMD;
    }

    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation Failed");
    }

    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_Confirm::on_pushButton_No_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
//===============================================
