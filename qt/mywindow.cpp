#include "mywindow.h"
#include <QDebug>
#include <QCloseEvent>

MyWindow::MyWindow(QWidget *parent) : QMainWindow(parent)
{

}

void MyWindow::changeEvent(QEvent * e) // overloading event(QEvent*) method of QMainWindow
{
    if(this->isActiveWindow())
        emit Actived();
    else
        emit Desactived();
}

void MyWindow::closeEvent(QCloseEvent *event)
{
    qInfo() << "Close event mywindow";
    emit close();
    event->accept();
}
