//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_thorconfig.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_THORCONFIG_H
#define DIAL_THORCONFIG_H

//===============================================
#include <QDialog>
#include <MyDefs.h>
#include <Thor.h>

//===============================================
namespace Ui {
class Dial_ThorConfig;
}

//===============================================
class Dial_ThorConfig : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_ThorConfig(CThor * thor, u16 frontend, QWidget *parent = 0);
    ~Dial_ThorConfig();

private slots:
    void on_toolButton_Select_clicked();

    void on_pushButton_Do_clicked();

    void on_pushButton_Close_clicked();

private:
    Ui::Dial_ThorConfig *ui;

    Qt::CheckState m_state;
    u16 m_frontend;
    CThor * p_thor;
    u32 m_mask;
    QString m_filename;

    void setup (void);
};

//===============================================
#endif // DIAL_THORCONFIG_H
