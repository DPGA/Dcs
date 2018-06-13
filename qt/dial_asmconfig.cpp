//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmconfig.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include "MyDefs.h"
#include "command_error.h"
#include "dial_asmconfig.h"
#include "ui_dial_asmconfig.h"


//===============================================
//===============================================
Dial_AsmConfig::Dial_AsmConfig(CAsm *asmm, u32 use, u16 *frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AsmConfig)
{
    p_asm = asmm;
    m_use = use;
    m_mask = 0;
    m_chan = 0;
    m_nbasm = 0;
    p_fe = frontend;
    usage(use);
    ui->setupUi(this);
    p_asm->SetMsgBox(ui->textEdit_Msg);
    m_filename.clear();
    setup();
    
    QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm_0, ui->checkBox_Asm_1, ui->checkBox_Asm_2, ui->checkBox_Asm_3,
                               ui->checkBox_Asm_4, ui->checkBox_Asm_5, ui->checkBox_Asm_6, ui->checkBox_Asm_7,
                               ui->checkBox_Asm_8, ui->checkBox_Asm_9, ui->checkBox_Asm_10, ui->checkBox_Asm_11 };
    for(int i=0; i<NB_ASM; i++)  ck[i]->setToolTip(QString("Asm %1 Selection").arg(i));
    ui->checkBox_All->setToolTip("Select All Asm");
    ui->checkBox_Clear->setToolTip("Unselect All Asm");
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->toolButton->setToolTip("File selection");
    ui->pushButton_Do->setToolTip("Configuration Action");
    ui->pushButton_Close->setToolTip("Close this window");
    ui->progressBar->setToolTip("Configuration Progress");
    ui->lineEdit_Fname->setToolTip("Selected File");
    for (int i=0;i<NB_ASM;++i) ColorCheck(i,true,true);
}

//===============================================
Dial_AsmConfig::~Dial_AsmConfig()
{
    p_asm->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_AsmConfig::on_checkBox_All_clicked()
{
    usage(0xffff);
    for (int i=0;i<NB_ASM;++i) ColorCheck(i,true,true);
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_0_clicked()
{
    m_state[0]= ui->checkBox_Asm_0->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_1_clicked()
{
    m_state[1]= ui->checkBox_Asm_1->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_2_clicked()
{
    m_state[2]= ui->checkBox_Asm_2->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_3_clicked()
{
    m_state[3]= ui->checkBox_Asm_3->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_4_clicked()
{
    m_state[4]= ui->checkBox_Asm_4->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_5_clicked()
{
    m_state[5]= ui->checkBox_Asm_5->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_6_clicked()
{
    m_state[6]= ui->checkBox_Asm_6->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_7_clicked()
{
    m_state[7]= ui->checkBox_Asm_7->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_8_clicked()
{
    m_state[8]= ui->checkBox_Asm_8->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_9_clicked()
{
    m_state[9]= ui->checkBox_Asm_9->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_10_clicked()
{
    m_state[10]= ui->checkBox_Asm_10->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Asm_11_clicked()
{
    m_state[11]= ui->checkBox_Asm_11->checkState();
    setup();
}

void Dial_AsmConfig::on_checkBox_Clear_clicked()
{
    usage(0);
    for (int i=0;i<NB_ASM;++i) ColorCheck(i,true,true);
    setup();
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_AsmConfig::on_toolButton_clicked()
{
    m_filename = QFileDialog::getOpenFileName(this, tr("Open File"),tr("/home","All Files (*.*);; XML Files (*.xml)"));
    std::cout << "file name " << m_filename.toStdString() << std::endl;
    setup();

}

void Dial_AsmConfig::ColorCheck(int ch,bool ok,bool init)
{
    QString Color;
    if (init) Color = "background-color:#EFEbE7";
    else
      if (ok) Color = "background-color:green";
      else Color = "background-color:red";
    switch (ch) {
        case 0 : ui->checkBox_Asm_0->setStyleSheet(Color);break;
        case 1 : ui->checkBox_Asm_1->setStyleSheet(Color);break;
        case 2 : ui->checkBox_Asm_2->setStyleSheet(Color);break;
        case 3 : ui->checkBox_Asm_3->setStyleSheet(Color);break;
        case 4 : ui->checkBox_Asm_4->setStyleSheet(Color);break;
        case 5 : ui->checkBox_Asm_5->setStyleSheet(Color);break;
        case 6 : ui->checkBox_Asm_6->setStyleSheet(Color);break;
        case 7 : ui->checkBox_Asm_7->setStyleSheet(Color);break;
        case 8 : ui->checkBox_Asm_8->setStyleSheet(Color);break;
        case 9 : ui->checkBox_Asm_9->setStyleSheet(Color);break;
        case 10: ui->checkBox_Asm_10->setStyleSheet(Color);break;
        case 11: ui->checkBox_Asm_11->setStyleSheet(Color);break;
    }
}

//===============================================
//===============================================
void Dial_AsmConfig::on_pushButton_Do_clicked()
{

    QString name;
    int rep, ret = 0;
    for (int i=0;i<NB_ASM;++i) ColorCheck(i,true,true);
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->progressBar->setValue(0);
	 int i;
    for(i=0; i<NB_ASM; i++) {
//			std::cout <<  i << " state " << m_state[i] << std::endl;
        if(m_state[i]== Qt::Checked) {
            m_mask = 1 << i;
            if(m_nbasm == 1) {
                if(m_filename.isEmpty()) {
                    name.clear();
                } else {
                    name = m_filename;
                }   // if(m_filename.isEmpty())
            } else {
                name.clear();
            }   // if(m_nbasm == 1)
 //           ui->textEdit_Msg->append("Asm Config Command Module " + QString::number(i));
            ui->textEdit_Msg->append(QString("Asm Config Command Module %1 with Mask 0x%2 Font End 0x%3").arg(i).arg(m_mask, 4, 16, QChar('0')).arg(getFrontEnd(i),2,16,QChar('0'))); 
            rep = p_asm->ConfigCmd(m_mask, getFrontEnd(i), name);
 //           std::cout << __FUNCTION__ <<  " " << __LINE__ << " rep :" << rep << " ret :" << ret << " i" << i << " m_mask " << m_mask << " frontend " << getFrontEnd(i) << std::endl;
            if(rep == NO_ERROR) {
                ui->textEdit_Msg->setTextColor("green");
                ui->textEdit_Msg->append("Operation Successfull");
                ColorCheck(i,true,false);
            } else {
                ret ++;
                ui->textEdit_Msg->setTextColor("red");
                ui->textEdit_Msg->append("Operation Failed");
                ColorCheck(i,false,false);
           }
            ui->textEdit_Msg->setTextColor(c);
        } // if(m_state[i]== Qt::Checked)
        ui->progressBar->setValue(i+1);  
    }   // for loop
//              std::cout << __FUNCTION__ <<  " " << __LINE__ << " rep :" << rep << " ret :" << ret << " i" << i << std::endl;
//	if(ret)
//		QMessageBox::warning(this, "Asm Config", "Operation failled");
//               std::cout << __FUNCTION__ <<  " " << __LINE__ << " rep :" << rep << " ret :" << ret << " i" << i << std::endl;
 
}

//===============================================
//===============================================
void Dial_AsmConfig::on_pushButton_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
void Dial_AsmConfig::usage (u32 use)
{
    for(int i=0; i<NB_ASM; i++) {
         m_state[i] = use & (1<<i) ? (Qt::Checked) : (Qt::Unchecked);
     }
}

//===============================================
//===============================================
void Dial_AsmConfig::setup(void)
{
    QString name;
    QCheckBox * ck[NB_CHAN] ={ ui->checkBox_Asm_0, ui->checkBox_Asm_1, ui->checkBox_Asm_2, ui->checkBox_Asm_3,
                               ui->checkBox_Asm_4, ui->checkBox_Asm_5, ui->checkBox_Asm_6, ui->checkBox_Asm_7,
                               ui->checkBox_Asm_8, ui->checkBox_Asm_9, ui->checkBox_Asm_10, ui->checkBox_Asm_11 };
    m_nbasm = 0;
    m_mask = 0;
    for(int i=0; i<NB_ASM; i++) {
        ck[i]->setCheckState(m_state[i]);
        if(m_state[i] == Qt::Checked) {
            m_chan = i;
            m_nbasm++;
            m_mask |= 1 << i;
        }
    }
 //   std::cout << "m_chan " << m_chan << " m_nbasm " << m_nbasm << " m_mask " << m_mask << std::endl;
    switch (m_nbasm) {
        case 0:
            ui->checkBox_Clear->setCheckState(Qt::Checked);
            ui->checkBox_All->setCheckState(Qt::Unchecked);
            name = "Automatic Default File";
            break;
        case 1:
            ui->checkBox_All->setCheckState(Qt::Unchecked);
            ui->checkBox_Clear->setCheckState(Qt::Unchecked);
            if(m_filename.isEmpty()) {
				char *dirdcs = getenv("DIR_DCS");
				if(dirdcs) {
					name = QString("%1/ConfigDcs/RegAsm_%2xml").arg(dirdcs).arg(m_chan);
				} else {
					name = "../../ConfigDcs/RegAsm_" + QString::number(m_chan) + ".xml";
				}
             } else {
                name = m_filename;
			}
            break;
        case NB_ASM:
            ui->checkBox_All->setCheckState(Qt::Checked);
            ui->checkBox_Clear->setCheckState(Qt::Unchecked);
            name = "Automatic Default File";
            break;
        default :
            ui->checkBox_All->setCheckState(Qt::Unchecked);
            ui->checkBox_Clear->setCheckState(Qt::Unchecked);
            name = "Automatic Default File";
    }
    ui->lineEdit_Fname->setText(name);
}

//===============================================
//===============================================
//===============================================
//===============================================
u16 Dial_AsmConfig::getFrontEnd(u32 chan)
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
//===============================================
//===============================================

