//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_asmconfig.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_ASMCONFIG_H
#define DIAL_ASMCONFIG_H

//===============================================
#include <QDialog>
#include "MyDefs.h"
#include "Asm.h"

//===============================================
namespace Ui {
class Dial_AsmConfig;
}

//===============================================
class Dial_AsmConfig : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AsmConfig(CAsm *asmm, u32 use, u16 *frontend, QWidget *parent = 0);
    ~Dial_AsmConfig();

private slots:
    void on_checkBox_All_clicked();

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

//    void on_pushButton_File_clicked();

    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

    void on_toolButton_clicked();
    void ColorCheck(int ch,bool ok,bool init);

private:
    Ui::Dial_AsmConfig *ui;

    Qt::CheckState m_state[NB_ASM];
    u16 * p_fe;
    CAsm * p_asm;
    u32 m_mask;
    u32 m_use;
    u32 m_chan;
    u32 m_nbasm;
    QString m_filename;
    QColor ColorCheckButton;

     void usage (u32 use);
     void setup(void);
     u16 getFrontEnd(u32 chan);
};

//===============================================
#endif // DIAL_ASMCONFIG_H
