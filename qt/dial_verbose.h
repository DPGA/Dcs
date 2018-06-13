//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_verbose.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_VERBOSE_H
#define DIAL_VERBOSE_H

//===============================================
#include <QDialog>

#include "Amc.h"

//===============================================
namespace Ui {
class Dial_Verbose;
}

//===============================================
class Dial_Verbose : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_Verbose(bool flag, u32 *verb, QWidget *parent = 0);
    ~Dial_Verbose();

private slots:
    void on_spinBox_Verb_Level_valueChanged(int arg1);

    void on_pushButton_Verb_Do_clicked();

    void on_pushButton_Verb_Cancel_clicked();

    void on_spinBox_Verb_Level_editingFinished();

private:
    Ui::Dial_Verbose *ui;
    u32  * p_verb;
    u32  m_level;
};

//===============================================
#endif // DIAL_VERBOSE_H
