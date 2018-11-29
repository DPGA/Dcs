#ifndef DIAL_MEZZA_H
#define DIAL_MEZZA_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class Dial_mezza;
}

class Dial_mezza : public QDialog
{
    Q_OBJECT

public:
    explicit Dial_mezza(u_int64_t Mask,QWidget *parent = 0);
    ~Dial_mezza();

private slots:
    void on_buttonBox_accepted();

signals:
     void valueChanged(u_int64_t newValeur);

private:
    Ui::Dial_mezza *ui;
    QVector<QCheckBox *> Mezza;
};

#endif // DIAL_MEZZA_H
