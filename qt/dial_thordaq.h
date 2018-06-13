//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thordaq.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_THORDAQ_H
#define DIAL_THORDAQ_H

#include <QDialog>
#include "Thor.h"

//===============================================
namespace Ui {
class Dial_ThorDaq;
}

//===============================================
class Dial_ThorDaq : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_ThorDaq(CThor *thor, bool flag, u32 use, u16 frontend, QWidget *parent = 0);
    ~Dial_ThorDaq();

private slots:
    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_ThorDaq *ui;
    CThor * p_thor;
    bool m_flag;
    u16  m_frontend;
    Qt::CheckState m_state;
};

//===============================================
#endif // DIAL_THORDAQ_H
