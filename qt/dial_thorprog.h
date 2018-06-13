//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorprog.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_THORPROG_H
#define DIAL_THORPROG_H

//===============================================
#include <QDialog>
#include <QTime>

#include "MyDefs.h"
#include "command_error.h"
#include "Thor.h"

//===============================================
namespace Ui {
class Dial_ThorProg;
}

//==============================================
class Dial_ThorProg : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_ThorProg(CThor *thor, u16 frontend, QWidget *parent = 0);
    ~Dial_ThorProg();

private slots:
    void on_toolButton_File_clicked();

    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_ThorProg *ui;
    CThor * p_thor;
    u16     m_frontend;
    QString m_filename;
    QString m_fname;
    Qt::CheckState m_state;
    QTime   m_time;
    int     m_elapsed;
    u32     m_status;

    void EpcsCheck(QString str);
};

//===============================================
#endif // DIAL_THORPROG_H
