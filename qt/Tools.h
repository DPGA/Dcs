//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Tools.h
//	Author		:	?
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef TOOLS_H
#define TOOLS_H
//=========================================================
#include <QObject>
#include <QTimer>
#include <QMessageBox>

#include <string>
#include <cstdio>
#include "MyDefs.h"
//=========================================================
//=========================================================
template <typename T>
std::string CFormat(const char* format, const T value)
{
    char buffer[128];
    std::sprintf(buffer, format, value);
    return (std::string(buffer));
}
//=========================================================
//=========================================================
class CtColor {
public:
    static const char* none;
    static const char* bold;
    static const char* underscore;
    static const char* blink;
    static const char* reverse;
    static const char* cancel;
};

class FgColor {
public:
    static const char* green;
    static const char* white;
    static const char* normal;
    static const char* cyan;
    static const char* yellow;
    static const char* red;
    static const char* black;
    static const char* blue;
    static const char* magenta;
    static const char* test1;
    static const char* test2;
};

class BgColor {
public:
    static const char* green;
    static const char* white;
    static const char* normal;
    static const char* cyan;
    static const char* yellow;
    static const char* red;
    static const char* black;
    static const char* blue;
    static const char* magenta;
    static const char* test1;
    static const char* test2;
};

//=========================================================
//=========================================================
void    TimerMessage (QString title, QString text, int ms = TIMERDFLT);
QString ElapsedTime  (u32 var);
u8      Reverse      (u8 c);
u32     QsOrdorn     (u32 var);
void    writerXml    (QString dir,u16 *pData,u32 sizeData,QString fileName,u8 num);
//=========================================================
//=========================================================
//=========================================================
//=========================================================
//=========================================================
//=========================================================
#endif // TOOLS_H
