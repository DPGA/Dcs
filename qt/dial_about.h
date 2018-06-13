//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_about.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ABOUT_H
#define DIAL_ABOUT_H

//===============================================
#include <iostream>
#include <QDialog>

//===============================================
namespace Ui {
class Dial_About;
}

//===============================================
class Dial_About : public QDialog
{
	Q_OBJECT

public:
	explicit Dial_About(QWidget *parent = 0);
	~Dial_About();

private slots:
	void on_pushButton_Close_clicked();

private:
	Ui::Dial_About *ui;
};

//===============================================
#endif // DIAL_ABOUT_H
