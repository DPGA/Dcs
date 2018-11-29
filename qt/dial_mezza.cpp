#include "dial_mezza.h"
#include "ui_dial_mezza.h"

#include <QLayout>
#include <QVector>
#include <QGroupBox>
#include <QLabel>

Dial_mezza::Dial_mezza(u_int64_t Mask,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_mezza)
{
    ui->setupUi(this);


    QGridLayout *LayoutLabel = new QGridLayout;
    QGridLayout *LayoutVerticalMezza = new QGridLayout;
    for (int index = 0;index <36;index++) {
       Mezza.push_back(new QCheckBox());
       if ((Mask >> index) && 1== 1) Mezza.back()->setChecked(Qt::Checked);
       else Mezza.back()->setChecked(Qt::Unchecked);
       LayoutLabel->addWidget(new QLabel("Asm " + QString::number(index%3)),index%3,0);

       LayoutVerticalMezza->addWidget( Mezza.back(),index/3,index%3);
    }
    ui->groupBox->setLayout(LayoutVerticalMezza);
}

Dial_mezza::~Dial_mezza()
{
    delete ui;
}

void Dial_mezza::on_buttonBox_accepted()
{
    u_int64_t MaskMezza=0;
    int index = 0;
    for (auto &indexMezza : Mezza) {
        if (indexMezza->isChecked())
            MaskMezza |= 1 << index;
        index++;
    }
    emit valueChanged(MaskMezza);
}
