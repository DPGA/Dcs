//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmreg.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ASMREG_H
#define DIAL_ASMREG_H

//===============================================
#include <QDialog>
#include "Asm.h"

//================================================
namespace Ui {
class Dial_AsmReg;
}

//===============================================
class Dial_AsmReg : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AsmReg(CAsm * asmm, u32 use, u16 *frontend, QWidget *parent = 0);
    ~Dial_AsmReg();

private slots:
    void on_pushButton_Close_clicked();

    void on_pushButton_Write_clicked();

    void on_pushButton_Read_clicked();

    void on_checkBox_Hexa_clicked();

    void on_spinBox_Select_editingFinished();

    void on_spinBox_Select_valueChanged(int arg1);

private:
    Ui::Dial_AsmReg *ui;
    CAsm *p_asm;

    u16 *p_val;
    u16 *p_fe;
    u32 m_addr;
    u32 m_len;
    u32 m_mask;
    u32 m_chan;
    u32 m_use;
    u32 m_base;

    void rd_display(void);
    u16  getFrontEnd(u32 chan);
};

//===============================================
#endif // DIAL_ASMREG_H
