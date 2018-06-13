//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_hvconfig.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//==========================================================
#ifndef DIAL_HVCONFIG_H
#define DIAL_HVCONFIG_H
//==========================================================
#include <QDialog>
#include <MyDefs.h>
#include "HvClient.h"
//==========================================================
namespace Ui {
class Dial_HvConfig;
}

//==========================================================
//==========================================================
class Dial_HvConfig : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_HvConfig(CHvClient *hv, QWidget *parent = 0);
    ~Dial_HvConfig();

private slots:
    void on_lineEdit_File_editingFinished();

    void on_toolButton_File_clicked();

    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_HvConfig *ui;
    CHvClient * p_hv;
    QString     m_filename;

    void setup(void);
};

//==========================================================
#endif // DIAL_HVCONFIG_H
