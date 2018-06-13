//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorreg.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_THORREG_H
#define DIAL_THORREG_H

//===============================================
#include <QDialog>
#include "Thor.h"

//===============================================
namespace Ui {
class Dial_ThorReg;
}

//===============================================
class Dial_ThorReg : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_ThorReg(CThor * thor, u32 use, u16 frontend, QWidget *parent = 0);
    ~Dial_ThorReg();

private slots:
    void on_pushButton_Close_clicked();

    void on_pushButton_Write_clicked();

    void on_pushButton_Read_clicked();

    void on_checkBox_Hexa_clicked();

//   void on_spinBox_editingFinished();

//    void on_spinBox_valueChanged(int arg1);

private:
    Ui::Dial_ThorReg *ui;
    CThor * p_thor;
    u16   * p_val;
    u32     m_addr;
    u32     m_len;
    u32     m_mask;
    u32     m_frontend;
    u32     m_base;
    Qt::CheckState m_state;

    void    rd_display(void);
};

//===============================================
#endif // DIAL_THORREG_H
