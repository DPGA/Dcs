//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	main.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include "mainwindow.h"
#include <QApplication>

//===============================================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_OSX
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);
 #endif
    MainWindow w;
    w.show();

    return a.exec();
}

//===============================================
//===============================================
