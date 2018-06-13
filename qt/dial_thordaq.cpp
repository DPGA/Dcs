//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thordaq.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "dial_thordaq.h"
#include "ui_dial_thordaq.h"

#include "command_error.h"

//===============================================
//===============================================
Dial_ThorDaq::Dial_ThorDaq(CThor *thor, bool flag, u32 use, u16 frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_ThorDaq)
{
    p_thor = thor;
    m_flag = flag;
    m_frontend = frontend;
    m_state = (use << THOR_CHAN) ? (Qt::Checked) : (Qt::Unchecked);
    ui->setupUi(this);
    p_thor->SetMsgBox(ui->textEdit_Msg);
    if(m_flag) {
        this->setWindowTitle("Thor Daq Start");
        ui->pushButton_Do->setText("Daq On");
        ui->pushButton_Do->setToolTip("Start DAQ Action");
    } else {
        this->setWindowTitle("Thor Daq Stop");
        ui->pushButton_Do->setText("Daq Off");
        ui->pushButton_Do->setToolTip("Stop DAQ Action");
    }
    ui->checkBox_Actived->setCheckState(m_state);
    ui->checkBox_Actived->setToolTip("Thor Avtived Status");
    ui->pushButton_Close->setToolTip("Close this window");
    ui->textEdit_Msg->setToolTip("Message Box");
}

//===============================================
//===============================================
Dial_ThorDaq::~Dial_ThorDaq()
{
    p_thor->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_ThorDaq::on_pushButton_Do_clicked()
{
    int ret;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    if(m_flag) {
        ui->textEdit_Msg->append("Thor Run Command Start");
        ret = p_thor->RunCmd(m_frontend, 1);
    } else {
        ui->textEdit_Msg->append("Thor Run Command Stop");
        ret = p_thor->RunCmd(m_frontend, 0);
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
void Dial_ThorDaq::on_pushButton_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
