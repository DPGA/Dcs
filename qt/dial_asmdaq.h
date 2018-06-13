//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmdaq.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ASMDAQ_H
#define DIAL_ASMDAQ_H

//===============================================
//===============================================
#include <QDialog>
#include "Asm.h"

//===============================================
//===============================================
namespace Ui {
class Dial_AsmDaq;
}

//===============================================
//===============================================
class Dial_AsmDaq : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AsmDaq(CAsm *asmm, bool flag, u32 use, QWidget *parent = 0);
    ~Dial_AsmDaq();

private slots:


    void on_checkBox_All_clicked();

    void on_checkBox_Asm0_clicked();

    void on_checkBox_Asm1_clicked();

    void on_checkBox_Asm2_clicked();

    void on_checkBox_Asm3_clicked();

    void on_checkBox_Asm4_clicked();

    void on_checkBox_Asm5_clicked();

    void on_checkBox_Asm6_clicked();

    void on_checkBox_Asm7_clicked();

    void on_checkBox_Asm8_clicked();

    void on_checkBox_Asm9_clicked();

    void on_checkBox_Asm10_clicked();

    void on_checkBox_Asm11_clicked();

    void on_checkBox_Clear_clicked();

    void on_pushButton_Do_clicked();

    void on_pushButton_Refresh_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_AsmDaq *ui;
    CAsm * p_asm;
    bool  m_flag;
    u32   m_use;
    u32   m_mask;
    Qt::CheckState state[NB_ASM];

    void display(void);
    void usage(u32 use);
};

//===============================================
#endif // DIAL_ASMDAQ_H
