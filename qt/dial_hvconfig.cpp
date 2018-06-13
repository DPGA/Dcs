//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_hvconfig.cpp
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
#include "command_error.h"
#include "dial_hvconfig.h"
#include "ui_dial_hvconfig.h"

//===============================================
//===============================================
Dial_HvConfig::Dial_HvConfig(CHvClient *hv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_HvConfig)
{
    p_hv = hv;
    ui->setupUi(this);
    p_hv->SetMsgBox(ui->textEdit_Msg);
    m_filename.clear();
    setup();
    ui->lineEdit_File->setToolTip("Selected File");
    ui->toolButton_File->setToolTip("Select File");
    ui->pushButton_Do->setToolTip("Load Action");
    ui->pushButton_Close->setToolTip("Close this window");
    ui->textEdit_Msg->setToolTip("Message Box");
    ui->radioButton_Sel_0->setToolTip("Module 0 Selection");
    ui->radioButton_Sel_1->setToolTip("Module 1 Selection");
    ui->radioButton_Sel_2->setToolTip("Module 2 Selection");
    ui->radioButton_Sel_3->setToolTip("Module 3 Selection");
 }

//===============================================
//===============================================
Dial_HvConfig::~Dial_HvConfig()
{
    p_hv->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_HvConfig::on_lineEdit_File_editingFinished()
{

}

//===============================================
//===============================================
void Dial_HvConfig::on_toolButton_File_clicked()
{
    m_filename = QFileDialog::getOpenFileName(this, tr("Open File"),tr("/home","All Files (*.*);; XML Files (*.xml)"));
    std::cout << "file name " << m_filename.toStdString() << std::endl;
    setup();
}

//===============================================
//===============================================
void Dial_HvConfig::on_pushButton_Do_clicked()
{
    QString name;
    int ret = 0;
    int mod;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    if(m_filename.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			name = QString("%1/ConfigDcs/HvOrder.xml").arg(dirdcs);
		} else {
#ifdef Q_OS_OSX
			name  = "../../../../config/HvOrder.xml";
#else
			name = "../../ConfigDcs/HvOrder.xml";
#endif
		}
     } else {
        name = m_filename;
     }
    ui->lineEdit_File-> setText(name);
    ui->textEdit_Msg->append("Load Orders Command with " + name);
    ret = p_hv->ParseXmlOrder(name);
    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append("Parse Xml Successfull");
        if (ui->radioButton_Sel_All->isChecked()) {
            for(mod=0; mod<NB_HVMOD; mod++) {
                int r = p_hv->LoadOrder(mod);
                if(r) {
                    ret = r;
                    ui->textEdit_Msg->setTextColor("red");
                    ui->textEdit_Msg->append(QString("Operation Failed on module %1").arg(mod));
                }
            }
        } else {
            if     (ui->radioButton_Sel_0->isChecked()) mod = 0;
            else if(ui->radioButton_Sel_1->isChecked()) mod = 1;
            else if(ui->radioButton_Sel_2->isChecked()) mod = 2;
            else if(ui->radioButton_Sel_3->isChecked()) mod = 3;
            else mod = 0;
            ret = p_hv->LoadOrder(mod);
        }
    }
    if(ret == NO_ERROR) {
            ui->textEdit_Msg->setTextColor("green");
            ui->textEdit_Msg->append("Load Operation Successfull");
    } else {
            ui->textEdit_Msg->setTextColor("red");
            ui->textEdit_Msg->append("Operation Failed");
    }
    ui->textEdit_Msg->setTextColor(c);
}

//===============================================
//===============================================
void Dial_HvConfig::on_pushButton_Close_clicked()
{
    this->close();
}
//===============================================
//===============================================
//===============================================
//===============================================
void Dial_HvConfig::setup(void)
{
    QString name;
    if(m_filename.isEmpty()) {
		char * dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			name = QString("%1/ConfigDcs/HvOrder.xml").arg(dirdcs);
		} else {
#ifdef Q_OS_OSX
			name = "../../../../config/HvOrder.xml";
#else
			name = "../../ConfigDcs/HvOrder.xml";
#endif
		}
    } else
        name = m_filename;
    ui->lineEdit_File-> setText(name);

}


//===============================================
//===============================================

//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
