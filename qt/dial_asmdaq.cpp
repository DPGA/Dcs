//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmdaq.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "dial_asmdaq.h"
#include "ui_dial_asmdaq.h"

#include "Asm.h"
#include "command_error.h"

//===============================================
//===============================================
Dial_AsmDaq::Dial_AsmDaq(CAsm * asmm, bool flag, u32 use, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AsmDaq)
{
    p_asm = asmm;
    m_flag = flag;
    m_use = use;
    m_mask = use;
    usage(use);
    ui->setupUi(this);
    p_asm->SetMsgBox(ui->textEdit_Msg);
    if(flag) {
        this->setWindowTitle("Asm DaQ Start");
        ui->pushButton_Do->setText("Daq On");
        ui->pushButton_Do->setToolTip("Start DAQ Action");
    } else {
        this->setWindowTitle("Asm Daq Stop");
        ui->pushButton_Do->setText("Daq Off");
        ui->pushButton_Do->setToolTip("Stop DAQ Action");
    }
    display();
    
    QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm0, ui->checkBox_Asm1, ui->checkBox_Asm2, ui->checkBox_Asm3, ui->checkBox_Asm4, ui->checkBox_Asm5,
                               ui->checkBox_Asm6, ui->checkBox_Asm7, ui->checkBox_Asm8, ui->checkBox_Asm9, ui->checkBox_Asm10, ui->checkBox_Asm11 };
    for(int i=0; i<NB_ASM; i++)  ck[i]->setToolTip(QString("Select Asm %1").arg(i));
    ui->checkBox_All->setToolTip("Select all Asm");
    ui->checkBox_Clear->setToolTip("Unselect all Asm");
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->pushButton_Refresh->setToolTip("Refresh Selection to Default Action");
    ui->pushButton_Close->setToolTip("Close this window");
}

//===============================================
//===============================================
Dial_AsmDaq::~Dial_AsmDaq()
{
    p_asm->SetMsgBox(NULL);
    delete ui;
}


//===============================================
//===============================================
void Dial_AsmDaq::on_checkBox_All_clicked()
{
    usage(0xffff);
    display();
}
//===============================================
//===============================================
void Dial_AsmDaq::on_checkBox_Asm0_clicked()
{
    state[0]= ui->checkBox_Asm0->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm1_clicked()
{
    state[1]= ui->checkBox_Asm1->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm2_clicked()
{
    state[2]= ui->checkBox_Asm2->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm3_clicked()
{
    state[3]= ui->checkBox_Asm3->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm4_clicked()
{
    state[4]= ui->checkBox_Asm4->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm5_clicked()
{
    state[5]= ui->checkBox_Asm5->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm6_clicked()
{
    state[6]= ui->checkBox_Asm6->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm7_clicked()
{
    state[7]= ui->checkBox_Asm7->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm8_clicked()
{
    state[8]= ui->checkBox_Asm8->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm9_clicked()
{
    state[9]= ui->checkBox_Asm9->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm10_clicked()
{
    state[10]= ui->checkBox_Asm10->checkState();
    display();
}

//===============================================
void Dial_AsmDaq::on_checkBox_Asm11_clicked()
{
    state[11]= ui->checkBox_Asm11->checkState();
    display();
}

//===============================================
//===============================================
void Dial_AsmDaq::on_checkBox_Clear_clicked()
{
    usage(0);
    display();
}

//===============================================
//===============================================
//===============================================
void Dial_AsmDaq::on_pushButton_Do_clicked()
{
    int ret;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    if(m_flag) {
        ui->textEdit_Msg->append(QString("Asm Run Command Start with mask 0x%1 front End 0x%2").arg(m_mask, 4, 16, QChar('0')).arg(BROADCAST_FE, 2, 16, QChar('0')));
        ret = p_asm->RunCmd(m_mask,BROADCAST_FE, 1);
    } else {
        ui->textEdit_Msg->append(QString("Asm Run Command Stop with mask 0x%1 front End 0x%2").arg(m_mask, 4, 16, QChar('0')).arg(BROADCAST_FE, 2, 16, QChar('0')));
        ret = p_asm->RunCmd(m_mask,BROADCAST_FE, 0);
    }
    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation Failed") ;
    }
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_AsmDaq::on_pushButton_Refresh_clicked()
{
    usage(m_use);
    display();
}

//===============================================
//===============================================
void Dial_AsmDaq::on_pushButton_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
void Dial_AsmDaq::display(void)
{
    QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm0, ui->checkBox_Asm1, ui->checkBox_Asm2, ui->checkBox_Asm3, ui->checkBox_Asm4, ui->checkBox_Asm5,
                               ui->checkBox_Asm6, ui->checkBox_Asm7, ui->checkBox_Asm8, ui->checkBox_Asm9, ui->checkBox_Asm10, ui->checkBox_Asm11 };

    int ok = 0;
    m_mask = 0;
    for(int i=0; i<NB_ASM; i++) {
        ck[i]->setCheckState(state[i]);
        if(state[i] == Qt::Checked) {
            ok++;
            m_mask |= 1 << i;
        }
    }
    if(ok == NB_ASM)
        ui->checkBox_All->setCheckState(Qt::Checked);
    else
        ui->checkBox_All->setCheckState(Qt::Unchecked);
    if(ok == 0)
        ui->checkBox_Clear->setCheckState(Qt::Checked);
    else
        ui->checkBox_Clear->setCheckState(Qt::Unchecked);

}
//===============================================
void Dial_AsmDaq::usage(u32 use)
{
    for(int i=0; i<NB_ASM; i++) {
        state[i] = use & (1<<i) ? (Qt::Checked) : (Qt::Unchecked);
    }
}

//===============================================
//===============================================
//===============================================
//===============================================
//===============================================


