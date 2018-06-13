#ifndef MONITORING_H
#define MONITORING_H

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QTimer>
#include <QThread>
#include <QLCDNumber>
#include <QEvent>


#include "shdmem.h"
#include "ringshm.h"
#include "decodeframe.h"

#include "myplotsqwt.h"


class monitoring : public QObject, public DecodeFrame
{
    Q_OBJECT

public:
    explicit monitoring( QWidget *parent = 0);
    ~monitoring();
 //   bool eventFilter(QObject *object, QEvent *event);

public slots:


protected:

private slots:
       void process();
       void ReadShmData();
       void ClearGraph();
       void setModal();
       void unsetModal();

private:
    QDockWidget *dock;
    double CptTrame = 0;
    ShmRingBuffer<SharedMemory> *ShdMem;
    QList <class MyPlotsQwt *> lMyPlotsQwt;
    QList <QLCDNumber *> lNumber;
    QPushButton *bClear;
    QMainWindow *DisplayAsm;
};

#endif // MONITORING_H
