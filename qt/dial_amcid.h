//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcid.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_AMCID_H
#define DIAL_AMCID_H

//===============================================
#include <QDialog>

#include "Amc.h"

//===============================================
namespace Ui {
class dial_amcid;
}

//===============================================
class dial_amcid : public QDialog
{
    Q_OBJECT

public:
    explicit dial_amcid(CAmc *amc, QWidget *parent = 0);
    ~dial_amcid();

private slots:
    void on_pushButton_AmcId_Do_clicked();

    void on_pushButton_AmcId_Quit_clicked();

private:
    Ui::dial_amcid *ui;
    CAmc * p_amc;
};

//===============================================
#endif // DIAL_AMCID_H
