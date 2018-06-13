//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_hvonoff.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//=========================================================
//=========================================================
#include "dial_hvonoff.h"
#include "ui_dial_hvonoff.h"

#include "MyDefs.h"
#include "command_error.h"
//=========================================================
//=========================================================
//=========================================================
//=========================================================
Dial_HvONOff::Dial_HvONOff(CHvClient *hv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_HvONOff)
{
    p_hv = hv;
    m_mask = 0;

    ui->setupUi(this);
    p_hv->SetMsgBox(ui->textEdit_Msg);

    ui->checkBox_HVSel_0->setChecked(true);
    ui->checkBox_HVSel_1->setChecked(true);
    ui->checkBox_HVSel_2->setChecked(true);
    ui->checkBox_HVSel_3->setChecked(true);

    ui->checkBox_HVSel_0->setToolTip("Module 0 Selection");
    ui->checkBox_HVSel_1->setToolTip("Module 1 Selection");
    ui->checkBox_HVSel_2->setToolTip("Module 2 Selection");
    ui->checkBox_HVSel_3->setToolTip("Module 3 Selection");

    ui->pushButton_Close->setToolTip("Close this window");
    ui->pushButton_HvOff->setToolTip("Hv Switch Off Action");
    ui->pushButton_HVOn->setToolTip("Hv Switch On Action");
    ui->textEdit_Msg->setToolTip("Message Box");
    update();
}

//=========================================================
Dial_HvONOff::~Dial_HvONOff()
{
    p_hv->SetMsgBox(NULL);
    delete ui;
}

//=========================================================
//=========================================================
//=========================================================
void Dial_HvONOff::on_pushButton_HVOn_clicked()
{
    switchhv (true);
}

//=========================================================
//=========================================================
void Dial_HvONOff::on_pushButton_HvOff_clicked()
{
    switchhv (false);
}

//=========================================================
//=========================================================
void Dial_HvONOff::on_pushButton_Close_clicked()
{
    this->close();
}
//=========================================================
//=========================================================
//=========================================================
void Dial_HvONOff::switchhv (bool flag)
{
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();

    m_mask = 0;
    if(ui->checkBox_HVSel_0->checkState() == Qt::Checked)  m_mask |= 0x1;
    if(ui->checkBox_HVSel_1->checkState() == Qt::Checked)  m_mask |= 0x2;
    if(ui->checkBox_HVSel_2->checkState() == Qt::Checked)  m_mask |= 0x4;
    if(ui->checkBox_HVSel_3->checkState() == Qt::Checked)  m_mask |= 0x8;

    int ret = p_hv->SwitchHV (m_mask, flag);
    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation Failed");
    }
    ui->textEdit_Msg->setTextColor(c);
    p_hv->SetMsgBox(ui->textEdit_Msg);
    update();
}

//=========================================================
//=========================================================
void Dial_HvONOff::update(void)
{
    ui->checkBox_HvOn_0->setChecked(p_hv->GetHvSat(0));
    ui->checkBox_HvOn_1->setChecked(p_hv->GetHvSat(1));
    ui->checkBox_HvOn_2->setChecked(p_hv->GetHvSat(2));
    ui->checkBox_HvOn_3->setChecked(p_hv->GetHvSat(3));
    if(p_hv->GetHvSat(0))
        ui->checkBox_HvOn_0->setToolTip("Module 0 High Voltage Status : ON" );
    else
        ui->checkBox_HvOn_0->setToolTip("Module 0 High Voltage Status : off");
    if(p_hv->GetHvSat(1))
        ui->checkBox_HvOn_1->setToolTip("Module 1 High Voltage Status : ON" );
    else
        ui->checkBox_HvOn_1->setToolTip("Module 1 High Voltage Status : off");
    if(p_hv->GetHvSat(2))
        ui->checkBox_HvOn_2->setToolTip("Module 2 High Voltage Status : ON" );
    else
        ui->checkBox_HvOn_2->setToolTip("Module 2 High Voltage Status : off");
    if(p_hv->GetHvSat(3))
        ui->checkBox_HvOn_3->setToolTip("Module 3 High Voltage Status : ON" );
    else
        ui->checkBox_HvOn_3->setToolTip("Module 3 High Voltage Status : off");

}

//=========================================================
//=========================================================
//=========================================================
//=========================================================
