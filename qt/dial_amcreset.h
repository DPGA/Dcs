//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcreset.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_AMCRESET_H
#define DIAL_AMCRESET_H

//===============================================
#include <QDialog>
#include "Amc.h"

//===============================================
namespace Ui {
class Dial_AmcReset;
}

//===============================================
class Dial_AmcReset : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AmcReset(CAmc * amc, u32 use, QWidget *parent = 0);
    ~Dial_AmcReset();

private slots:
    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_AmcReset *ui;

    CAmc * p_amc;

    void setup(u32 use);
    u32 getrst(void);
};

//===============================================
#endif // DIAL_AMCRESET_H
