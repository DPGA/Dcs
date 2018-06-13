//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	d.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ALLDAQ_H
#define DIAL_ALLDAQ_H

//===============================================
//===============================================
#include <QDialog>
#include "MyDefs.h"
#include "command_error.h"
#include "Amc.h"
#include "Asm.h"
#include "Thor.h"

//===============================================
//===============================================
namespace Ui {
class Dial_AllDaq;
}

//===============================================
//===============================================
class Dial_AllDaq : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AllDaq(CAmc *amc, CAsm* asmm, CThor *thor, u32 mask, u16 seq, QWidget *parent = 0);
    ~Dial_AllDaq();

private slots:

    void ColorCheck(int ch,bool ok,bool init);

    void on_pushButton_Close_clicked();

    void on_checkBox_Amc_clicked();

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

    void on_checkBox_Thor_clicked();

    void on_checkBox_Clear_clicked();

    void on_checkBox_All_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_Start_clicked();

    void on_ModeDaqThor_activated(int index);

    void on_FreqPiedThor_valueChanged(int arg1);

    void on_FreqPiedThor_editingFinished();

    void on_delayTrigthor_editingFinished();

    void on_delayTrigthor_valueChanged(int arg1);

private:
    Ui::Dial_AllDaq *ui;
    CAmc * p_amc;
    CAsm * p_asm;
    CThor* p_thor;
    AMCDATA *p_amcdt;
    ASMDATA *p_asmdt;
    THORDATA *p_thordt;
    int  m_daq;
    u32  m_usemask;
    u16  m_Onsequence;
    u16  m_Offsequence;
    QColor m_color;
    Qt::CheckState state[NB_CHAN];

    
    void tip(void);
    void display(void);
    void usage(u32 use);
    int  AmcSwitch(u32 run);
    int  ThorSwitch(u32 run);
    int  AsmSwitch(u32 run);
    int  FastSwitch(u32 run);

    int  GetStatus(void);
    u64  ComputeMask(u32 use, u64 def_mask);
};

//===============================================
//===============================================
#endif // DIAL_ALLDAQ_H