//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorconfig.cpp
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
#include "dial_thorconfig.h"
#include "ui_dial_thorconfig.h"

//===============================================
//===============================================
Dial_ThorConfig::Dial_ThorConfig(CThor *thor, u16 frontend, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_ThorConfig)
{
    p_thor = thor;
    m_frontend = frontend;
    ui->setupUi(this);
    p_thor->SetMsgBox(ui->textEdit_Msg);
    m_filename.clear();
    setup();
    
    ui->lineEdit_Fname->setToolTip("Selected File");
    ui->toolButton_Select->setToolTip("Select File");
    ui->pushButton_Do->setToolTip("Configuration Action");
    ui->pushButton_Close->setToolTip("Close this window");
    ui->progressBar->setToolTip("Configuration Progress");
    ui->textEdit_Msg->setToolTip("Message Box");

}

//===============================================
//===============================================
Dial_ThorConfig::~Dial_ThorConfig()
{
    p_thor->SetMsgBox(NULL);
    delete ui;
}

//===============================================
//===============================================
void Dial_ThorConfig::on_toolButton_Select_clicked()
{
    m_filename = QFileDialog::getOpenFileName(this, tr("Open File"),tr("/home","All Files (*.*);; XML Files (*.xml)"));
    std::cout << "file name " << m_filename.toStdString() << std::endl;
    setup();

}

//===============================================
//===============================================
void Dial_ThorConfig::on_pushButton_Do_clicked()
{
    QString name;
    int ret;
    QColor c = ui->textEdit_Msg->textColor();
    ui->textEdit_Msg->clear();
    ui->progressBar->setValue(0);
    if(m_filename.isEmpty())
        name.clear();
    else
        name = m_filename;
    ui->textEdit_Msg->append("Thor Config Command");
    ret = p_thor->ConfigCmd(m_frontend, name);
    std::cout << __FUNCTION__ << __LINE__ << " ret :" << ret << std::endl;
    if(ret == NO_ERROR) {
           ui->textEdit_Msg->setTextColor("green");
           ui->textEdit_Msg->append("Operation Successfull");
    } else {
           ui->textEdit_Msg->setTextColor("red");
           ui->textEdit_Msg->append("Operation Failed");
    }
    std::cout << __FUNCTION__ << __LINE__ << " ret :" << ret << std::endl;
    ui->progressBar->setValue(100);
    std::cout << __FUNCTION__ << __LINE__ << " ret :" << ret << std::endl;
    ui->textEdit_Msg->setTextColor(c);
    std::cout << __FUNCTION__ << __LINE__ << " ret :" << ret << std::endl;
 }

//===============================================
//===============================================
void Dial_ThorConfig::on_pushButton_Close_clicked()
{
    this->close();
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_ThorConfig::setup(void)
{
    QString name;
    if(m_filename.isEmpty()) {
		char* dirdcs = getenv("DIR_DCS");
		if(dirdcs) {
			name = QString("%1/ConfigDcs/RegThor.xml").arg(dirdcs);
		} else {
			name = "../../ConfigDcs/RegThor.xml";
		}
     } else {
        name = m_filename;
	}
    ui->lineEdit_Fname-> setText(name);

}

//===============================================
//===============================================
//===============================================
//===============================================
