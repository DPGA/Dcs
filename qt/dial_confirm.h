//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_confirm.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_CONFIRM_H
#define DIAL_CONFIRM_H

//===============================================
#include <QDialog>
#include "MyDefs.h"
#include "command_error.h"
#include "Amc.h"
#include "Asm.h"
#include "Thor.h"

//===============================================
namespace Ui {
class Dial_Confirm;
}

//===============================================
class Dial_Confirm : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_Confirm(CAmc *amc, CAsm *asmm, CThor *thor, u32 mask, u16 frontend, u16 cmd, QWidget *parent = 0);
    ~Dial_Confirm();

private slots:
    void on_pushButton_Yes_clicked();

    void on_pushButton_No_clicked();

private:
    Ui::Dial_Confirm *ui;
    CAmc *  p_amc;
    CAsm *  p_asm;
    CThor * p_thor;
    u32     m_mask;
    u16     m_frontend;
    u16     m_cmd;

};

//===============================================
#endif // DIAL_CONFIRM_H
