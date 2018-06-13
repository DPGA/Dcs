//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorreg.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <iostream>
#include <QMessageBox>
#include "dial_thorreg.h"
#include "ui_dial_thorreg.h"
#include "MyDefs.h"
#include "command_error.h"
#include "Thor.h"

//===============================================
//===============================================
Dial_ThorReg::Dial_ThorReg(CThor *thor, u32 use, u16 frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_ThorReg)
{
    p_thor = thor;
    p_val = NULL;
    m_frontend = frontend;
    m_addr = 0;
    m_len = 0;
    m_base = 10;
    m_state = (use << THOR_CHAN) ? (Qt::Checked) : (Qt::Unchecked);
    ui->setupUi(this);
    p_thor->SetMsgBox(ui->textEdit_Msg);
    ui->textEdit_Read->clear();
    ui->textEdit_Write->clear();
    rd_display();
    ui->lineEdit_Address->setText(QString::number(m_addr, m_base));
    
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->textEdit_Read->setToolTip("Read Data");
    ui->textEdit_Write->setToolTip("data to Write");
    ui->lineEdit_Address->setToolTip("First Address of Transfert");
    ui->spinBox_Length->setToolTip("Length of Read Frame");
    ui->checkBox_Actived->setToolTip("Thor Avtived Status");
    ui->checkBox_Hexa->setToolTip("Values in Hexadecimal or Decimal Mode");
    ui->lineEdit_FrontEnd->setToolTip("Thor Front End Number");
    ui->pushButton_Read->setToolTip("Read Action");
    ui->pushButton_Write->setToolTip("Write Action");
    ui->pushButton_Close->setToolTip("Close this window");
}

//===============================================
//===============================================
Dial_ThorReg::~Dial_ThorReg()
{
    p_thor->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_ThorReg::on_pushButton_Close_clicked()
{
    this->close();
}

//===============================================
//===============================================
void Dial_ThorReg::on_pushButton_Write_clicked()
{
    bool ok;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Read->clear();
    m_addr = ui->lineEdit_Address->text().toULong(&ok, m_base);
    QString str = ui->textEdit_Write->toPlainText();
     if(str.isEmpty()) {
         QMessageBox::critical(this, " Thor Read/Write Registers", " Nothing to Write");
         return;
     }

     u16 *mem = new u16 [m_len];
     if(mem == NULL) {
         QMessageBox::critical(this, "Thor Read/Write Registers", "Can't Create Buffer");
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
 //    ui->textEdit_Msg->append("Thor Write " + QString::number(cnt) + " Registers from Address 0x" + QString::number(m_addr,16));
     ui->textEdit_Msg->append(QString("Thor Write %1 Registers From Address 0x%2").arg(cnt).arg(m_addr, 4, 16, QChar('0'))); 
     int ret = p_thor->WriteCmd(m_frontend, cnt, m_addr, mem);
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
void Dial_ThorReg::on_pushButton_Read_clicked()
{
    int ret;
    bool ok;
    u32 tmp;

    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->textEdit_Read->clear();
    m_len = ui->spinBox_Length->value();
    m_addr = ui->lineEdit_Address->text().toULong(&ok, m_base);
    if(m_len == 0) {
         QMessageBox::warning(this, "Thor Read/Write Registers", "The Length must be > 0");
         return;
    }
    u16 *mem = new u16 [m_len];
    if(mem == NULL) {
         QMessageBox::critical(this, "Thor Read/Write Registers", "Can't Create Buffer");
         return;
     }
     p_val = mem;
     for(u32 i=0; i<m_len; i++, p_val++) *p_val = 0;
//     ui->textEdit_Msg->append("Thor Read " + QString::number(m_len) + " Registers from Address 0x" + QString::number(m_addr,16));
     ui->textEdit_Msg->append(QString("Thor Read %1 Registers from Address 0x%2").arg(m_len).arg(m_addr, 4, 16, QChar('0')));
     ret = p_thor->ReadCmd(m_frontend, m_len, m_addr, mem);
     if(ret == NO_ERROR) {
         p_val = mem;
         for(u32 i=0; i<m_len; i++, p_val++) {
             tmp = *p_val;
             if(m_base == 16)
                ui->textEdit_Read->append(QString("%1").arg(tmp, 4 , m_base, QChar('0')));
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
void Dial_ThorReg::on_checkBox_Hexa_clicked()
{
    ui->textEdit_Read->clear();
    rd_display();
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_ThorReg::rd_display(void)
{
    ui->checkBox_Actived->setCheckState(m_state);
    m_base = ui->checkBox_Hexa->isChecked() ? 16 : 10;
 //    ui->lineEdit_Address->setText(QString::number(m_addr, m_base));
    ui->lineEdit_FrontEnd->setText(QString::number(m_frontend, m_base));
}

//===============================================
//===============================================
//===============================================
//===============================================
