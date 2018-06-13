//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcreset.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "dial_amcreset.h"
#include "ui_dial_amcreset.h"
#include "command_error.h"
#include "Amc.h"
//===============================================
//===============================================
Dial_AmcReset::Dial_AmcReset(CAmc * amc,u32 use, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AmcReset)
{
    p_amc = amc;
    ui->setupUi(this);
    p_amc->SetMsgBox(ui->textEdit_Msg);
    setup(use);
    
    ui->checkBox_Asm_Rx_0->setToolTip("Asm 0 Rx Part");
    ui->checkBox_Asm_Rx_1->setToolTip("Asm 1 Rx Part");
    ui->checkBox_Asm_Rx_2->setToolTip("Asm 2 Rx Part");
    ui->checkBox_Asm_Rx_3->setToolTip("Asm 3 Rx Part");
    ui->checkBox_Asm_Rx_4->setToolTip("Asm 4 Rx Part");
    ui->checkBox_Asm_Rx_5->setToolTip("Asm 5 Rx Part");
    ui->checkBox_Asm_Rx_6->setToolTip("Asm 6 Rx Part");
    ui->checkBox_Asm_Rx_7->setToolTip("Asm 7 Rx Part");
    ui->checkBox_Asm_Rx_8->setToolTip("Asm 8 Rx Part");
    ui->checkBox_Asm_Rx_9->setToolTip("Asm 9 Rx Part");
    ui->checkBox_Asm_Rx_10->setToolTip("Asm 10 Rx Part");
    ui->checkBox_Asm_Rx_11->setToolTip("Asm 11 Rx Part");
    ui->checkBox_Asm_Tx->setToolTip("All Asm Tx Part");

    ui->checkBox_Thor_Rx->setToolTip("Thor Rx Part");
    ui->checkBox_Thor_Tx->setToolTip("Thor Tx Part");

    ui->checkBox_Daq->setToolTip("Daq Part");
    ui->checkBox_Gbe->setToolTip("10Gbe Part");
    ui->checkBox_Tse->setToolTip("Tse Part");

    ui->textEdit_Msg->setToolTip("Message Box");
    ui->pushButton_Do->setToolTip("Reset Selected Parts Action");
    ui->pushButton_Close->setToolTip("Close this window");
    
}

//===============================================
//===============================================
Dial_AmcReset::~Dial_AmcReset()
{
    p_amc->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_AmcReset::on_pushButton_Do_clicked()
{
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    u32 val = getrst();
 //   QString str = "Send Reset Command with arg 0x" + QString::number(val,16);
    ui->textEdit_Msg->append(QString("Send Reset Command with arg 0x%1").arg(val, 4, 16 , QChar('0')));
    if(p_amc->ResetCmd(val) == NO_ERROR) {
        ui->textEdit_Msg->setTextColor(QColor("green"));
        ui->textEdit_Msg->append("SuccessFul");
    } else {
        ui->textEdit_Msg->setTextColor(QColor("red"));
        ui->textEdit_Msg->append("Error during execution");
    }
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_AmcReset::on_pushButton_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
void Dial_AmcReset::setup(u32 use)
{
    bool ok;
    QCheckBox * ck[NB_CHAN] = {
        ui->checkBox_Actived_0, ui->checkBox_Actived_1, ui->checkBox_Actived_2, ui->checkBox_Actived_3, ui->checkBox_Actived_4, ui->checkBox_Actived_5,
        ui->checkBox_Actived_6, ui->checkBox_Actived_7, ui->checkBox_Actived_8, ui->checkBox_Actived_9, ui->checkBox_Actived_10, ui->checkBox_Actived_11 };
    for(int i=0; i<NB_ASM; i++) {
        ok = (use & (1<<i)) ? true : false;
        ck[i]->setChecked(ok);
        ck[i]->setToolTip(QString("Asm %1 Activation Status").arg(i));
    }
    ok = (use & (1 << THOR_CHAN)) ? true : false;
    ui->checkBox_ThorActived->setChecked(ok);
    ui->checkBox_ThorActived->setToolTip("Thor Activation Status");
}

//===============================================
//===============================================
u32 Dial_AmcReset::getrst(void)
{
    u32 val = 0;
    if(ui->checkBox_Asm_Rx_0->isChecked())  val |= 0x1;
    if(ui->checkBox_Asm_Rx_1->isChecked())  val |= 0x2;
    if(ui->checkBox_Asm_Rx_2->isChecked())  val |= 0x4;
    if(ui->checkBox_Asm_Rx_3->isChecked())  val |= 0x8;
    if(ui->checkBox_Asm_Rx_4->isChecked())  val |= 0x10;
    if(ui->checkBox_Asm_Rx_5->isChecked())  val |= 0x20;
    if(ui->checkBox_Asm_Rx_6->isChecked())  val |= 0x40;
    if(ui->checkBox_Asm_Rx_7->isChecked())  val |= 0x80;
    if(ui->checkBox_Asm_Rx_8->isChecked())  val |= 0x100;
    if(ui->checkBox_Asm_Rx_9->isChecked())  val |= 0x200;
    if(ui->checkBox_Asm_Rx_10->isChecked()) val |= 0x400;
    if(ui->checkBox_Asm_Rx_11->isChecked()) val |= 0x800;
    if(ui->checkBox_Asm_Tx->isChecked())    val |= 0x1000;

    if(ui->checkBox_Thor_Rx->isChecked())   val |= 0x2000;
    if(ui->checkBox_Thor_Tx->isChecked())   val |= 0x4000;

    if(ui->checkBox_Daq->isChecked())       val |= 0x8000;
    if(ui->checkBox_Gbe->isChecked())       val |= 0x10000;
    if(ui->checkBox_Tse->isChecked())       val |= 0x20000;

    return(val);
}

//===============================================
//===============================================
//===============================================
