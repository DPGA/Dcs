//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmprog.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ASMPROG_H
#define DIAL_ASMPROG_H

//===============================================
#include <QDialog>
#include <QTime>

#include "MyDefs.h"
#include "command_error.h"
#include "Asm.h"

//===============================================
namespace Ui {
class Dial_AsmProg;
}

class Dial_AsmProg : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AsmProg(CAsm *asmm, u32 mask, u16 * frontend, QWidget *parent = 0);
    ~Dial_AsmProg();

private slots:
    void on_spinBox_Asm_editingFinished();

    void on_spinBox_Asm_valueChanged(int arg1);

    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

    void on_toolButton_File_clicked();

    void on_checkBox_Asm_0_clicked();

    void on_checkBox_Asm_1_clicked();

    void on_checkBox_Asm_2_clicked();

    void on_checkBox_Asm_3_clicked();

    void on_checkBox_Asm_4_clicked();

    void on_checkBox_Asm_5_clicked();

    void on_checkBox_Asm_6_clicked();

    void on_checkBox_Asm_7_clicked();

    void on_checkBox_Asm_8_clicked();

    void on_checkBox_Asm_9_clicked();

    void on_checkBox_Asm_10_clicked();

    void on_checkBox_Asm_11_clicked();

    void on_checkBox_Clear_clicked();

    void on_checkBox_All_clicked();

    void display();

    void usage(u32 use);

private:
    Ui::Dial_AsmProg *ui;
    CAsm *p_asm;
    u32   m_mask;
    u16 * p_fe;
    QString m_filename;
    QString m_fname;
    Qt::CheckState m_state;
    Qt::CheckState state[NB_ASM];
    QTime m_time;
    int   m_elapsed;
    u32   m_status;
    int   m_chan;
    u32   m_usemask;
    char *dirdcs;

    u16   getFrontEnd(u32 chan);
    void  EpcsCheck(QString str);
};

//===============================================
#endif // DIAL_ASMPROG_H
