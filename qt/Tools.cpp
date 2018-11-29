//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	Tools.cpp
//	Author		:	?
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//================================================
//================================================
#include <iostream>
#include "Tools.h"
#include <QXmlStreamWriter>
#include <QFile>
//================================================
//================================================
const char * CtColor::none = "\033[00m";
const char * CtColor::bold = "\033[01m";
const char * CtColor::underscore = "\033[04m";
const char * CtColor::blink = "\033[05m";
const char * CtColor::reverse = "\033[07m";
const char * CtColor::cancel = "\033[08m";

const char * FgColor::black = "\033[30m";
const char * FgColor::red = "\033[31m";
const char * FgColor::green = "\033[32m";
const char * FgColor::yellow = "\033[33m";
const char * FgColor::blue = "\033[34m";
const char * FgColor::magenta = "\033[35m";
const char * FgColor::cyan = "\033[36m";
const char * FgColor::white = "\033[37m";
const char * FgColor::test1 = "\033[38m";
const char * FgColor::test2 = "\033[39m";

const char * BgColor::black = "\033[40m";
const char * BgColor::red = "\033[41m";
const char * BgColor::green = "\033[42m";
const char * BgColor::yellow = "\033[43m";
const char * BgColor::blue = "\033[44m";
const char * BgColor::magenta = "\033[45m";
const char * BgColor::cyan = "\033[46m";
const char * BgColor::white = "\033[47m";
const char * BgColor::test1 = "\033[48m";
const char * BgColor::test2 = "\033[49m";

//================================================
//================================================
void TimerMessage(QString title, QString text, int ms)
{
    QTimer * timer = new QTimer();
    QMessageBox *  msgBox = new QMessageBox();
    msgBox->setWindowTitle(title);
    msgBox->setText(text);
    timer->singleShot(ms, msgBox, SLOT(hide()));
    msgBox->exec();
    msgBox->close();
    delete msgBox;
    delete timer;
}   // Message

//================================================
//================================================
QString ElapsedTime (u32 var)
{
    QString Str;
    u16  h,m,s;
    h = var / 3600;
    m = (var - (h*3600)) /60;
    s = var - (h*3600) - (m*60);
    Str = QString("%1:%2:%3").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));
//    std::cout << "var:" << var << " h:" << h << " m:" << m << " s:" << s << " ..." << Str.toStdString() << std::endl;
    return Str;
}
//================================================
//================================================
u8 Reverse  (u8 c)
{
    u8 ret;
    ret = (((c & 0x80) >> 7) | ((c & 0x40) >> 5) | ((c & 0x20) >> 3) | ((c & 0x10) >> 1) |
          ((c & 0x08) << 1) | ((c & 0x04) << 3) | ((c & 0x02) << 5) | ((c & 0x01) << 7));
    return (ret);

}	// Reverse

//================================================
//================================================
u32 QsOrdorn (u32 var)
{
		union {
		u32 u;
		char ch[4];
	}un;
	un.u = var;
//	std::cout << std::hex << "var " << var << std::dec  << std::endl;
	char tmp = un.ch[0];
	un.ch[0] = un.ch[3];
	un.ch[3] = tmp;
	tmp = un.ch[1];
	un.ch[1] = un.ch[2];
	un.ch[2] = tmp;
	var = un.u; 
//	std::cout << std::hex << "var " << var << std::dec  << std::endl;
	return (var);
}
//================================================
//================================================
//================================================
//================================================
void writerXml(QString dir,u16 *pData,u32 sizeData,QString fileName,u8 num)
{

 //      ASMDATA *AsmData = p_asm->GetData(m_use);
       QString fileXmlName = dir + "/" + fileName + QString("%1").arg(num,2, 10, QChar('0'))+ ".xml";
       QFile fileXml(fileXmlName);

    // Ouverture du fichier en écriture et en texte. (sort de la fonction si le fichier ne s'ouvre pas)
       if(!fileXml.open(QFile::WriteOnly | QFile::Text)) return;
       QXmlStreamWriter writer(&fileXml);

       // Active l'indentation automatique du fichier XML pour une meilleur visibilité
       writer.setAutoFormatting(true);

       // Insert la norme de codification du fichier XML :
       writer.writeStartDocument();

       // Élément racine du fichier XML
       writer.writeStartElement("CATALOG");
   //    u16 *datatemp = (u16 *) malloc(sizeof(ASMDATA));
   //    memcpy(datatemp,AsmData,sizeof(ASMDATA));
       for (unsigned int i=0;i<sizeData/2;i++) {
            writer.writeStartElement("REG");
            writer.writeAttribute("TITLE", "");
            writer.writeAttribute("ADDRESS", QString("0x%1").arg((u16) i,4,16,QChar('0')));
            writer.writeAttribute("VALUE", QString("0x%1").arg((u16) pData[i],4,16,QChar('0')));
            writer.writeAttribute("SIZE", "1");
            writer.writeAttribute("RW", "");
            writer.writeAttribute("COMMENT", "");
            writer.writeEndElement();
       }

       // Ferme l'élément site
       writer.writeEndElement();

       // Finalise le document XML
       writer.writeEndDocument();

       // Fermer le fichier pour bien enregistrer le document et ferme l'élément root
       fileXml.close();

}
