//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_finder.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_FINDER_H
#define DIAL_FINDER_H

//===============================================
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>


#include "MyDefs.h"
#include "Asm.h"
#include "Thor.h"

//===============================================
namespace Ui {
class Dial_finder;
}

//===============================================
class Dial_finder : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_finder(CAsm *asmm, CThor *thor, u16 * fe, QWidget *parent = 0);
    ~Dial_finder();

private slots:
    void on_pushButton_Do_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_finder *ui;
    CAsm  *p_asm;
    CThor * p_thor;
    u16 m_frontend[NB_CHAN];
    ASMDATA * p_asmdt;
    THORDATA * p_thordt;
    
    QCheckBox * ock[NB_CHAN];
    QLineEdit * fli[NB_CHAN];
    QLineEdit * mli[NB_CHAN];
    QLineEdit * Firmrev[NB_CHAN];
    QCheckBox * Calibok[NB_CHAN][3];
    QLineEdit * Lockpll[NB_CHAN];
    void cnfg (void);
};

//===============================================
#endif // DIAL_FINDER_H
