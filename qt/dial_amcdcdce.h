//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcdccde.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_AMCDCDCE_H
#define DIAL_AMCDCDCE_H

//===============================================
#include <QDialog>
#include "MyDefs.h"
#include "Amc.h"

//===============================================
namespace Ui {
class Dial_AmcDCDCE;
}

//===============================================
class Dial_AmcDCDCE : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_AmcDCDCE(CAmc *amc, QWidget *parent = 0);
    ~Dial_AmcDCDCE();

private slots:
    void on_checkBox_clicked();

    void on_spinBox_editingFinished();

    void on_spinBox_valueChanged(int arg1);

    void on_pushButton_Read_clicked();

    void on_pushButton_Write_clicked();

    void on_pushButton_Save_clicked();

    void on_Close_clicked();

private:
    Ui::Dial_AmcDCDCE *ui;
    CAmc * p_amc;
    AMCDATA *p_amcdt;
    
    u32 m_chip;
    u32 m_rval[NB_CDCE_REG][NB_CDCE];
    u32 m_wval;
    int m_base;

    void rd_display(void);
    void wr_display(void);
};

//===============================================
#endif // DIAL_AMCDCDCE_H
