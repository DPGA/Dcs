//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorprog.cpp
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
#include "dial_thorprog.h"
#include "ui_dial_thorprog.h"

//===============================================
//===============================================
Dial_ThorProg::Dial_ThorProg(CThor *thor, u16 frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_ThorProg)
{
    p_thor = thor;
    m_frontend = frontend;
    m_filename.clear();
    m_elapsed = 0;
    ui->setupUi(this);
    p_thor->SetMsgBox(ui->textEdit_Msg);
    p_thor->SetBar(ui->progressBar);
    ui->textEdit_Msg->clear();
	char * dirdcs= getenv("DIR_DCS");
	if(dirdcs) {
		m_fname = QString("%1/ConfigDcs/thor.rpd").arg(dirdcs);
	} else {
		m_fname = "../../ConfigDcs/thor.rpd";
	}
    ui->lineEdit_Fname->setText(m_fname);
    ui->progressBar->setValue(0);
    ui->lineEdit_EPCS->setText("");
    ui->checkBox_Erase->setCheckState(Qt::Unchecked);
    
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->lineEdit_Fname->setToolTip("Selected File");
    ui->progressBar->setToolTip("Programming Progress");
    ui->lineEdit_EPCS->setToolTip("EPCS Memory Type");
    ui->checkBox_Erase->setToolTip("EPCSMemory Erase Status");
    ui->toolButton_File->setToolTip("Select File");
    ui->pushButton_Do->setToolTip("Programming Action");
    ui->pushButton_Close->setToolTip("Close this window");
}

//===============================================
//===============================================
Dial_ThorProg::~Dial_ThorProg()
{
    p_thor->SetMsgBox(NULL);
    p_thor->SetBar(NULL);
    delete ui;
}
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
void Dial_ThorProg::on_toolButton_File_clicked()
{
    m_filename = QFileDialog::getOpenFileName(this, tr("Open File"),tr("/home","All Files (*.*);; Programming Files (*.rpd)"));
    if(!m_filename.isEmpty()) {
        ui->textEdit_Msg->append(m_filename + " is selected");
//    if(p_thor->GetVerbose()) std::cout << "file name " << m_filename.toStdString() << std::endl;
    }
}

//===============================================
//===============================================
void Dial_ThorProg::on_pushButton_Do_clicked()
{
    int ret;
    vector <u8> Memory;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    m_time.start();

    ret = p_thor->WaitEpcsBusy(m_frontend, &m_status, 3);
    if(ret != NO_ERROR) {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append("Epcs is Busy : try later");
        ui->textEdit_Msg->setTextColor(c);
        return;
    }

    switch (m_status & 0xff) {
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

    ui->textEdit_Msg->append("Erase EPCS");
    ret = p_thor->EraseEpcs(m_frontend);
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

    ret = p_thor->WritePage(m_frontend, &Memory);
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
void Dial_ThorProg::on_pushButton_Close_clicked()
{
    this->close();
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_ThorProg::EpcsCheck(QString str)
{
    ui->lineEdit_EPCS->setText("EPCS1");
    ui->textEdit_Msg->append(str + " dedected (0x" + QString::number(m_status & 0xff, 16) + ")");
}

//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
