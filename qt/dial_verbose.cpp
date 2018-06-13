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
#include "dial_verbose.h"
#include "ui_dial_verbose.h"

//===============================================
//===============================================
Dial_Verbose::Dial_Verbose(bool flag, u32 *verb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_Verbose)
{
    m_level = *verb;
    p_verb  = verb;
    ui->setupUi(this);
    ui->spinBox_Verb_Level->setValue(m_level);
    QString str = (flag) ? "Nios Verbose Level" : "Prog Verbose Level";
    ui->label->setText(str);
    ui->spinBox_Verb_Level->setToolTip(str);
    ui->pushButton_Verb_Do->setToolTip("Set " + str + " Action");
    ui->pushButton_Verb_Cancel->setToolTip("Close this window");
}

//===============================================
//===============================================
Dial_Verbose::~Dial_Verbose()
{
    delete ui;
}

//===============================================
//===============================================
void Dial_Verbose::on_spinBox_Verb_Level_valueChanged(int arg1)
{
    m_level = arg1;
 }
//===============================================
//===============================================
void Dial_Verbose::on_spinBox_Verb_Level_editingFinished()
{
    m_level = ui->spinBox_Verb_Level->value();
}

//===============================================
//===============================================
//===============================================
//===============================================
void Dial_Verbose::on_pushButton_Verb_Do_clicked()
{
    *p_verb = m_level;
    this->close();
}

//===============================================
//===============================================
void Dial_Verbose::on_pushButton_Verb_Cancel_clicked()
{
    this->close();
}

//===============================================
//===============================================
