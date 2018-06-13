//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcdcde.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "dial_amcdcdce.h"
#include "ui_dial_amcdcdce.h"

#include "command_error.h"

//===============================================
//===============================================
Dial_AmcDCDCE::Dial_AmcDCDCE(CAmc *amc, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AmcDCDCE)
{
    int i;
    p_amc = amc;
    p_amcdt = p_amc->GetData();
    for(i=0; i< NB_CDCE_REG; i++) {
        for(int j=0 ; j<NB_CDCE; j++) {
            m_rval[i][j] = 0;
        }
    }
    m_wval = 0;
    ui->setupUi(this);
    p_amc->SetMsgBox(ui->textEdit_Msg);
    m_chip = ui->spinBox->value();
    rd_display();
    wr_display();
    
    ui->checkBox->setToolTip("Hexadecimal or Decimal Display");
    QLineEdit * rd[NB_CDCE_REG] = { ui->lineEdit_Read0, ui->lineEdit_Read1, ui->lineEdit_Read2, ui->lineEdit_Read3, ui->lineEdit_Read4,
                                    ui->lineEdit_Read5, ui->lineEdit_Read6, ui->lineEdit_Read7,ui->lineEdit_Read8 };
    for (int i=0; i<NB_CDCE_REG; i++) {
        rd[i]->setToolTip(QString("Read Value of The Selected Chip Regsiter %1").arg(i));
    }
    ui->textEdit_Msg->setToolTip(" Message Box");
    ui->spinBox->setToolTip("CDCE Chip Selection");
    ui->lineEdit_Write->setToolTip("Write All Values to the Selected Chip");
    ui->pushButton_Read->setToolTip("Read Action");
    ui->pushButton_Write->setToolTip("Write Action");
    ui->pushButton_Save->setToolTip("Save Action");
    ui->Close->setToolTip("Close this window");
}

//===============================================
//===============================================
Dial_AmcDCDCE::~Dial_AmcDCDCE()
{
    p_amc->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_checkBox_clicked()
{
    rd_display();
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_spinBox_editingFinished()
{
    m_chip = ui->spinBox->value();
    rd_display();
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_spinBox_valueChanged(int arg1)
{
    m_chip = arg1; //ui->spinBox->value();
   rd_display();
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_pushButton_Read_clicked()
{
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Msg->append("Amc Read CDCE chip " + QString::number(m_chip));
    if(p_amc->ReadCDCE(m_chip,"")== NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation failled");
    }
    ui->textEdit_Msg->setTextColor(c);
    
    for (int i=0; i<NB_CDCE_REG; i++) {
        m_rval[i][m_chip] = p_amcdt->cdce.Chip[m_chip].reg[i];
    }

    rd_display();
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_pushButton_Write_clicked()
{
    bool ok;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Msg->append("Amc Write CDCE " + QString::number(m_chip));
    QString str = ui->lineEdit_Write->text();
    m_wval = str.toLong(&ok, m_base);   
    if(p_amc->WriteCDCE(m_chip, m_wval, "")== NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation failled");
    }
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_pushButton_Save_clicked()
{
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Msg->append("Amc Save CDCE"+ QString::number(m_chip));
    if(p_amc->SaveCDCE(m_chip)== NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation failled");
    }
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_AmcDCDCE::on_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
void Dial_AmcDCDCE::rd_display(void)
{
    QLineEdit * rd[NB_CDCE_REG] = { ui->lineEdit_Read0, ui->lineEdit_Read1, ui->lineEdit_Read2, ui->lineEdit_Read3, ui->lineEdit_Read4,
                                    ui->lineEdit_Read5, ui->lineEdit_Read6, ui->lineEdit_Read7,ui->lineEdit_Read8 };
    m_base = (ui->checkBox->isChecked()) ? 16 :10;

    for (int i=0; i<NB_CDCE_REG; i++) {
        rd[i]->setText(QString::number(m_rval[i][m_chip],m_base));
    }
}
//===============================================
void Dial_AmcDCDCE::wr_display(void)
{
    m_base = (ui->checkBox->isChecked()) ? 16 :10;
    ui->lineEdit_Write->setText(QString::number(m_wval,m_base));
}


//===============================================
//===============================================
//===============================================
