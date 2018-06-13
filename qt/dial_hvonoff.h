//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_hvonoff.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_HVONOFF_H
#define DIAL_HVONOFF_H

//===============================================
#include <QDialog>

#include "HvClient.h"

//===============================================
namespace Ui {
class Dial_HvONOff;
}

//===============================================
class Dial_HvONOff : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_HvONOff(CHvClient * hv, QWidget *parent = 0);
    ~Dial_HvONOff();

private slots:
    void on_pushButton_HVOn_clicked();

    void on_pushButton_HvOff_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_HvONOff *ui;
    u32 m_mask;

    CHvClient * p_hv;
    bool hvon;
    void switchhv (bool flag);
    void update(void);
};

//===============================================
#endif // DIAL_HVONOFF_H
