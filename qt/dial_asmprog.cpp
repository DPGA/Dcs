//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmprg.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <iostream>
#include <fstream>
using namespace std;
#include <QFileDialog>
#include <QMessageBox>
#include "dial_asmprog.h"
#include "ui_dial_asmprog.h"

//===============================================
//===============================================
Dial_AsmProg::Dial_AsmProg(CAsm *asmm, u32 mask, u16 * frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_AsmProg)
{
    p_asm = asmm;
    m_usemask = mask;
    usage(m_usemask);
    ui->setupUi(this);

    p_fe   = frontend;
    m_elapsed = 0;
    m_filename.clear();


    dirdcs = getenv("DIR_DCS");
    if(dirdcs) {
        m_fname = QString("%1/ConfigDcs/Firmware/asm.rpd").arg(dirdcs);
	} else {
		m_fname = "../../ConfigDcs/asm.rpd";
	}

    p_asm->SetMsgBox(ui->textEdit_Msg);
    p_asm->SetBar(ui->progressBar);
    ui->textEdit_Msg->clear();
    ui->lineEdit_Fname->setText(m_fname);
    ui->lineEdit_EPCS->clear();
    ui->progressBar->setValue(0);
    ui->checkBox_Erase->setCheckState(Qt::Unchecked);
//    m_chan = ui->spinBox_Asm->value();
//    m_state = ((m_mask >> m_chan & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
//    ui->checkBox_Actived->setCheckState(m_state);
    
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->lineEdit_Fname->setToolTip("Selected File");
    ui->lineEdit_EPCS->setToolTip("EPCS Memory Type");
    ui->progressBar->setToolTip("Prorgramming Progress");
    ui->checkBox_Erase->setToolTip("EPCS Memory Erase Status");
//    ui->spinBox_Asm->setToolTip("Select ASM");
//    ui->checkBox_Actived->setToolTip("Asm Active Status");
    ui->toolButton_File->setToolTip("File Selection");
    ui->pushButton_Do->setToolTip("Programming Action");
    ui->pushButton_Close->setToolTip("Close This Window");
    display();
}

//===============================================
//===============================================
Dial_AsmProg::~Dial_AsmProg()
{
    p_asm->SetMsgBox(NULL);
    p_asm->SetBar(NULL);
    delete ui;
}


void Dial_AsmProg::on_checkBox_Asm_0_clicked()
{
    state[0]= ui->checkBox_Asm_0->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_1_clicked()
{
    state[1]= ui->checkBox_Asm_1->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_2_clicked()
{
    state[2]= ui->checkBox_Asm_2->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_3_clicked()
{
    state[3]= ui->checkBox_Asm_3->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_4_clicked()
{
    state[4]= ui->checkBox_Asm_4->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_5_clicked()
{
    state[5]= ui->checkBox_Asm_5->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_6_clicked()
{
    state[6]= ui->checkBox_Asm_6->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_7_clicked()
{
    state[7]= ui->checkBox_Asm_7->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_8_clicked()
{
    state[8]= ui->checkBox_Asm_8->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_9_clicked()
{
    state[9]= ui->checkBox_Asm_9->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_10_clicked()
{
    state[10]= ui->checkBox_Asm_10->checkState();
    display();
}

void Dial_AsmProg::on_checkBox_Asm_11_clicked()
{
    state[11]= ui->checkBox_Asm_11->checkState();
    display();
}


void Dial_AsmProg::on_checkBox_Clear_clicked()
{
    usage(0);
    display();
    ui->checkBox_All->setCheckState(Qt::Unchecked);
}

void Dial_AsmProg::on_checkBox_All_clicked()
{
    usage(0xFFFF);
    ui->checkBox_Clear->setCheckState(Qt::Unchecked);
    display();
}

//===============================================
//===============================================
void Dial_AsmProg::usage(u32 use)
{
    for(int i=0; i<NB_ASM; i++) {
        state[i] = use & (1<<i) ? (Qt::Checked) : (Qt::Unchecked);
    }
}	// usage

//===============================================
//===============================================
void Dial_AsmProg::display()
{
    QCheckBox * ck[NB_ASM] = { ui->checkBox_Asm_0, ui->checkBox_Asm_1, ui->checkBox_Asm_2, ui->checkBox_Asm_3,
                               ui->checkBox_Asm_4, ui->checkBox_Asm_5, ui->checkBox_Asm_6, ui->checkBox_Asm_7,
                               ui->checkBox_Asm_8, ui->checkBox_Asm_9, ui->checkBox_Asm_10, ui->checkBox_Asm_11
                             };

    int ok = 0;
    m_usemask = 0;
    for(int i=0; i<NB_ASM; i++) {
        ck[i]->setCheckState(state[i]);
        if(state[i] == Qt::Checked) {
            ok++;
            m_usemask |= 1 << i;
        }
    }
}
//===============================================
//===============================================
void Dial_AsmProg::on_toolButton_File_clicked()
{
    QString FirmwareDir(dirdcs);
    FirmwareDir.append("/ConfigDcs/Firmware");
    m_filename = QFileDialog::getOpenFileName(this, tr("Open File"),tr(FirmwareDir.toStdString().c_str(),"All Files (*.*);; Programming Files (*.rpd)"));
    if(!m_filename.isEmpty()) {
        ui->textEdit_Msg->append(m_filename + " is selected");
        ui->lineEdit_Fname->setText(m_filename);
//    if(p_thor->GetVerbose()) std::cout << "file name " << m_filename.toStdString() << std::endl;
    }
}

//===============================================
//===============================================
void Dial_AsmProg::on_spinBox_Asm_editingFinished()
{
//    m_chan = ui->spinBox_Asm->value();
    m_state = ((m_mask >> m_chan & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
//    ui->checkBox_Actived->setCheckState(m_state);
}

//===============================================
//===============================================
void Dial_AsmProg::on_spinBox_Asm_valueChanged(int arg1)
{
    m_chan = arg1;
    m_state = ((m_mask >> m_chan & 0x1) == 1) ? (Qt::Checked) : (Qt::Unchecked);
//    ui->checkBox_Actived->setCheckState(m_state);
}

//===============================================
//===============================================
void Dial_AsmProg::on_pushButton_Do_clicked()
{
    int ret;
    vector <u8> Memory;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    m_time.start();
    u16 StatusEpc[2];
    m_chan = 0xff;
    u16 frontend = getFrontEnd(m_chan);
 //   u16 m_mask1 = 1 << m_chan;
    ret = p_asm->WaitEpcsBusy(m_usemask, frontend,  &StatusEpc[0], 3,1000);
    if(ret != NO_ERROR) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Epcs is Busy : try later");
        ui->textEdit_Msg->setTextColor(c);
        return;
    }

    switch (StatusEpc[0] & 0xff) {
        case EPCS1  : EpcsCheck("EPCS1");  break;
        case EPCS4  : EpcsCheck("EPCS4");  break;
        case EPCS16 : EpcsCheck("EPCS16"); break;
        case EPCS64 : EpcsCheck("EPCS64"); break;
        default     :
            ui->lineEdit_EPCS->setText("Unkown");
            ui->textEdit_Msg->setTextColor("red");
            ui->textEdit_Msg->append("Can't Identify EPCS Type");
            ui->textEdit_Msg->setTextColor(c);
            QMessageBox::warning(this,"Preog EPCS", "Can't Identify EPCS Type");
            return;
    }

    if(!m_filename.isEmpty())
        m_fname = m_filename;
    ui->textEdit_Msg->append("Open File " + m_fname);
    ifstream f(m_fname.toStdString());
    if(!f.is_open()) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Can't open file " + m_fname);
        ui->textEdit_Msg->setTextColor(c);
        return;
    }
    ui->label_2->setText("Erasing ");
    ui->textEdit_Msg->append("Erase EPCS");

    ret = p_asm->EraseEpcs(m_usemask, frontend,false);
    if(ret != NO_ERROR) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Can' t Erase EPCS");
        ui->textEdit_Msg->setTextColor(c);
        f.close();
       return;
    }
    ret = p_asm->EraseEpcs(m_usemask, frontend,true);
    if(ret != NO_ERROR) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Can' t Erase EPCS");
        ui->textEdit_Msg->setTextColor(c);
        f.close();
       return;
    }
    ui->checkBox_Erase->setChecked(true);

    ui->textEdit_Msg->append("Read File " + m_fname);
    while (f.good()) {
        Memory.push_back ((u8) f.get());
    }
    f.close();
    ui->label_2->setText("Programming ");
    ui->textEdit_Msg->append("Programming EPCS");
    ret = p_asm->WritePage(m_usemask, frontend, &Memory);
    if(ret != NO_ERROR) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Error During Programmation");
        ui->textEdit_Msg->setTextColor(c);
    } else {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("End of Programming");
    }
    m_elapsed = m_time.elapsed();
    ui->textEdit_Msg->append("Time elapsed: " + QString::number(m_elapsed) + "ms");
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_AsmProg::on_pushButton_Close_clicked()
{
    this->close();
}

//===============================================
//===============================================
//===============================================
//===============================================
u16 Dial_AsmProg::getFrontEnd(u32 chan)
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
void Dial_AsmProg::EpcsCheck(QString str)
{
    ui->lineEdit_EPCS->setText(str);
    ui->textEdit_Msg->append(str + " dedected ");
}

//===============================================
//===============================================
//===============================================
//===============================================

