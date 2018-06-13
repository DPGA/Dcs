//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_about.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <QPixmap>
#include "dial_about.h"
#include "ui_dial_about.h"
#include "version.h"

//===============================================
Dial_About::Dial_About(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dial_About)
 
 {   

	ui->setupUi(this);

    ui->Version->setStyleSheet("QLabel { background-color : red; color : blue; } ");
//	ui->label_Version->setText(" Experimental Version : 2.00");
    ui->Version->setText("Version DCS " + QString(__DCS_VERSION_STRING__)+ "\n" + QString(__DATE__) +"\t" + QString(__TIME__));
	QPixmap image (":images/catfootprint.png");
	ui->label_img->setPixmap(image);
}

//===============================================
Dial_About::~Dial_About()
{
	delete ui;
}

//===============================================
void Dial_About::on_pushButton_Close_clicked()
{
	this->close();
}
//===============================================
//===============================================
