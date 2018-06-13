//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmreg.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================

#include <iostream>
#include <QMessageBox>
#include "dial_asmreg.h"
#include "ui_dial_asmreg.h"
#include "MyDefs.h"
#include "command_error.h"
#include "Asm.h"

//===============================================
//===============================================
Dial_AsmReg::Dial_AsmReg(CAsm * asmm, u32 use, u16 *frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AsmReg)
{
    p_asm = asmm;
    ui->setupUi(this);
    p_asm->SetMsgBox(ui->textEdit_Msg);
    ui->textEdit_Read->clear();
    ui->textEdit_Write->clear();
    p_val = NULL;
    p_fe = frontend;
    m_addr = 0;
    m_len = 0;
    m_mask = 0;
    m_chan = 0;
    m_use = use;
    m_base = 10;
    rd_display();
    ui->lineEdit_Address->setText(QString::number(m_addr, m_base));
    
    ui->lineEdit_Address->setToolTip("First Address of the Transfert");
    ui->spinBox_Select->setToolTip("Asm Select");
    ui->checkBox_Avtived->setToolTip("Asm Actived Status");
    ui->checkBox_Hexa->setToolTip("Values in Hexadecimal or Decimal Mode");
    ui->lineEdit_FE->setToolTip("Front End Nb of the Selected Asm");
    ui->spinBox_Length->setToolTip("Length of thr read Transfert");
    ui->textEdit_Read->setToolTip("Read Data");
    ui->textEdit_Write->setToolTip("Date to Write");
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->pushButton_Read->setToolTip("Read Action");
    ui->pushButton_Write->setToolTip("Write Action");
    ui->pushButton_Close->setToolTip("Close this window");
   
}

//===============================================
//===============================================
Dial_AsmReg::~Dial_AsmReg()
{
    p_asm->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
//===============================================
void Dial_AsmReg::on_pushButton_Close_clicked()
{
    this->close();
}

//===============================================
//===============================================
void Dial_AsmReg::on_pushButton_Write_clicked()
{
    bool ok;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Read->clear();
 //   m_len= ui->spinBox_Length->value();
    m_addr = ui->lineEdit_Address->text().toULong(&ok, m_base);
    m_mask = 1 << m_chan;
//    if(m_len == 0) {
//        QMessageBox::warning(this, "Asm Read/Write Registers", "The Length must be > 0");
//        return;
//    }
    QString str = ui->textEdit_Write->toPlainText();
    if(str.isEmpty()) {
        QMessageBox::critical(this, " Asm Read/Write Registers", " Nothing to Write");
        return;
    }

    u16 *mem = new u16 [m_len];
    if(mem == NULL) {
        QMessageBox::critical(this, "Asm Read/Write Registers", "Can't Create Buffer");
        return;
    }

    p_val = mem;
    if(str.endsWith("\n") == false)
        str.append("\n");
    QChar *data = str.data();
    QString stmp;
    stmp.clear();
    u32 cnt =0;
    for (int i=0; i<str.size(); i++) {
//        QChar ch = str.at(i);
//        std::cout << i << " " << ch.toLatin1()  << std::hex  << " 0x" << ((int)ch.toLatin1()) << "..0x" << ((int)data->toLatin1()) << std::dec << std::endl;
        if(*data == 0xa) {
            cnt ++;
            std::cout << "cnt " << cnt << ".." << stmp.toStdString() << std::endl;
            *p_val = stmp.toUShort();
            p_val++;
            stmp.clear();
        } else {
            stmp.append(*data);
        }
        data++;
    }
//    ui->textEdit_Msg->append("Asm " + QString::number(m_chan) + " Write " + QString::number(cnt) + " Registers from Address 0x" + QString::number(m_addr,16));
    ui->textEdit_Msg->append(QString("Asm %1 Write %2 Registers from Address 0x%3").arg(m_chan).arg(cnt).arg(m_addr, 4, 16, QChar('0')));
    int ret = p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), cnt, m_addr , mem);
    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation failed");
        QMessageBox::warning(this, "Asm Read/Write Registers", "Operation failled");
    }
    ui->textEdit_Msg->setTextColor(c);
    delete [] mem;
}

//===============================================
//===============================================
void Dial_AsmReg::on_pushButton_Read_clicked()
{
    bool ok;
    int ret = 0;
    u32 tmp;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Read->clear();
    m_len= ui->spinBox_Length->value();
    m_addr = ui->lineEdit_Address->text().toULong(&ok,m_base);
    m_mask = 1 << m_chan;
    if(m_len == 0) {
        QMessageBox::warning(this, "Asm Read/Write Registers", "The Length must be > 0");
        return;
    }
    u16 *mem = new u16 [m_len];
    if(mem == NULL) {
        QMessageBox::critical(this, "Asm Read/Write Registers", "Can't Create Buffer");
        return;
    }
    p_val = mem;
    for(u32 i=0; i<m_len; i++, p_val++) *p_val = 0;

//    ui->textEdit_Msg->append("Asm " + QString::number(m_chan) + " Read " + QString::number(m_len) + " Registers from Address 0x" + QString::number(m_addr,16));
    ui->textEdit_Msg->append(QString("Asm %1 Read %2 Registers from Address 0x%3").arg(m_chan).arg(m_len).arg(m_addr,  4, 16, QChar('0')));
    ret = p_asm->ReadCmd(m_mask, getFrontEnd(m_chan), m_len, m_addr, mem);
    if(ret == NO_ERROR) {
        p_val = mem;
        for(u32 i=0; i<m_len; i++, p_val++) {
            tmp = *p_val;
            if(m_base == 16)
					ui->textEdit_Read->append(QString("%1").arg(tmp, 4, m_base, QChar('0')));
            else
					ui->textEdit_Read->append(QString::number(tmp,m_base));
        }
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Operation Successfull");
    } else {
        for(u32 i=0; i<m_len; i++) {
            ui->textEdit_Read->append("Nav");
        }
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Operation failed");
    }
    delete[] mem;
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
//===============================================
void Dial_AsmReg::on_checkBox_Hexa_clicked()
{
    ui->textEdit_Read->clear();
    rd_display();
}

//===============================================
//===============================================
void Dial_AsmReg::on_spinBox_Select_editingFinished()
{
    m_chan = ui->spinBox_Select->value();
    m_mask = 1 << m_chan;
    ui->textEdit_Read->clear();
    rd_display();
}

//===============================================ui->lineEdit_Address->setText(QString::number(m_addr, m_base));
//===============================================
void Dial_AsmReg::on_spinBox_Select_valueChanged(int arg1)
{
    m_chan= arg1;
    m_mask = 1 << m_chan;
    ui->textEdit_Read->clear();
    rd_display();
}
//===============================================
//===============================================
//===============================================
//===============================================
void Dial_AsmReg::rd_display(void)
{
    Qt::CheckState state = ((m_use >> m_chan & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
    ui->checkBox_Avtived->setCheckState(state);
    m_base = ui->checkBox_Hexa->isChecked() ? 16 : 10;
//    ui->lineEdit_Address->setText(QString::number(m_addr, m_base));
    ui->lineEdit_FE->setText(QString::number(getFrontEnd(m_chan),m_base));

}
//===============================================
//===============================================
u16 Dial_AsmReg::getFrontEnd(u32 chan)
{
    u16 * p= p_fe;
    for(u32 i=0; i<NB_ASM; i++, p++) {
       if(i == chan)
           return (*p);
    }
    return(BROADCAST_FE);
}

//===============================================
//===============================================
//===============================================
//===============================================
