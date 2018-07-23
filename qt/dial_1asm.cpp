#include "dial_1asm.h"
#include "ui_dial_1asm.h"
#include "command_error.h"
#include "dial_calib.h"
#include "dial_hvonoff.h"
#include <QDir>
#include <QDate>
#include <QXmlStreamWriter>

dial_1Asm::dial_1Asm(CAsm *asmm, CHvClient *hv,u32 use, u16 *frontend,QString path,QWidget *parent) :
    QDialog(parent), ipc(),
    ui(new Ui::dial_1Asm)
{
    ui->setupUi(this);
    p_asm = asmm;
    p_hv = hv;
    m_use = use;
    p_fe  = frontend;
    ui->AsmBoard->display((int)use);
    ui->AsmBoard->setHexMode();
    ui->AsmBoard->setSegmentStyle(QLCDNumber::Flat);
    m_chan = frontend[use];
    m_mask = 1 << use;
    p_asm->SetMsgBox(ui->AsmConsole);
    ui->SaveTest->setChecked(false);
    ui->SaveFile->setChecked(false);
    ui->RunNb->setEnabled(ui->SaveFile->isChecked());
    ui->labelRunNb->setEnabled(ui->SaveFile->isChecked());
    Path= path;
    Refresh();

    char *dirdcs = getenv("DIR_DCS");
    if(dirdcs) m_filename = QString("%1/ConfigDcs/RegAsm_%2.xml").arg(dirdcs).arg(m_use);
    else       m_filename = "../../ConfigDcs/RegAsm_" + QString::number(m_chan) + ".xml";
    ui->AsmFileName->setText(m_filename);
    


    ASMDATA *AsmData = p_asm->GetData(m_use);
    double step=0.0;
    for (int i=0;i<32;i++) {
      ui->PhaseDrsAdc->addItem(QString::number(step,'f',1));
      step += 11.25;
      if (t_phase[i] == ((AsmData->rw.Jitter_Clean[4] & 0x0001FC00) >> 10)) ui->PhaseDrsAdc->setCurrentIndex(i);
    }


}

void dial_1Asm::Refresh()
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);

    if ((AsmData->rw.Mode & 0xFFFE) == 1) ui->AsmchoiceLogic->setChecked(true);

    ui->AsmModeDaq->setCurrentIndex((AsmData->rw.Mode & 0xE) >> 1);
    ui->AsmFreqPied->setValue(AsmData->rw.FreqPied);
    ui->AsmdelayTrig->setValue(AsmData->rw.Blonde_Offset);
    ui->cfgRegDrs->setValue(AsmData->rw.RegSpare3);
    double freq = (1/(double) (ui->AsmFreqPied->value()*32)*1000);
    ui->labelFreq->setText(QString::number(freq,'f',4)+ " KHz");
    if (ui->AsmModeDaq->currentIndex() == 7) {
        ui->AsmFreqPied->setEnabled(true);
        ui->Label_AsmfreqPied->setEnabled(true);
    } else {
        ui->AsmFreqPied->setEnabled(false);
        ui->Label_AsmfreqPied->setEnabled(false);
    }
    
    decodeStatus();
}


void dial_1Asm::decodeStatus()
{
    p_asm->decodeStatus(ui->Asmstatus,ui->AsmstatusPll,m_mask,m_chan,true);
    if(ui->Asmstatus->text().toInt() & (1 << 31)) ui->AsmstatusDaq->setText("Started"); else ui->AsmstatusDaq->setText("Stopped");
}

dial_1Asm::~dial_1Asm()
{
    p_asm->SetMsgBox(NULL);
    delete ui;
}

void dial_1Asm::on_AsmModeDaq_activated(int index)
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.Mode &= 0xfff1;
    AsmData->rw.Mode |= index<<1;
    ui->AsmConsole->append("Change Select Mode Daq 0x" + QString::number(AsmData->rw.Mode,16) + " index " +QString::number(index));
    int ret;
    int loop=0;
    do {
      p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x21 , (u16 *) &AsmData->rw.Mode),"Select Mode Daq " + QString::number(AsmData->rw.Mode,16));
    }  while ((ret != NO_ERROR) && (loop++ <4));

    if (index == 7) {    if(ui->SaveFile->isChecked())
            ui->RunNb->show();
        ui->AsmFreqPied->setEnabled(true);
        ui->Label_AsmfreqPied->setEnabled(true);
    } else {
        ui->AsmFreqPied->setEnabled(false);
        ui->Label_AsmfreqPied->setEnabled(false);
    }
    decodeStatus();
}

void dial_1Asm::on_AsmchoiceLogic_clicked()
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    if (ui->AsmchoiceLogic->isChecked()) AsmData->rw.Mode |= 0x0001; else AsmData->rw.Mode &= 0xFFFE;
    int ret;
    int loop=0;
    do {
        p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x21 , (u16 *) &AsmData->rw.Mode),"Select Mode Daq " + QString::number(AsmData->rw.Mode,16));
    }  while ((ret != NO_ERROR) && (loop++ <4));
    decodeStatus();
}



void dial_1Asm::on_AsmdelayTrig_editingFinished()
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.Blonde_Offset = ui->AsmdelayTrig->value();
    int ret;
    int loop=0;
    do {
        p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x8c , (u16 *) &AsmData->rw.Blonde_Offset),"Delay Trigger " + QString::number(AsmData->rw.Blonde_Offset,16));
    }  while ((ret != NO_ERROR) && (loop++ <4));
    decodeStatus();
}

void dial_1Asm::on_AsmFreqPied_editingFinished()
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.FreqPied = ui->AsmFreqPied->value();
    int ret;
    int loop=0;
    do {
        p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x91 , (u16 *) &AsmData->rw.FreqPied),"Frequence pied " + QString::number(AsmData->rw.FreqPied,16));
    }  while ((ret != NO_ERROR) && (loop++ <4));
    decodeStatus();
}

void dial_1Asm::on_AsmConfig_clicked()
{
    int ret;
    int loop=0;
    do {
        p_asm->Message(ret=p_asm->ConfigCmd(m_mask, m_chan, m_filename),"Config carte Asm  0x" + QString::number(m_chan,16));
    }  while ((ret != NO_ERROR) && (loop++ <4));
    decodeStatus();
}

/*
QXmlStreamReader reader; // Objet servant à la lecture du fichier Xml
   QString fileXmlName = "c:/bzhenel.xml";
   QFile fileXml(fileXmlName);
   QString fileTxtName = "c:/bzhenel.txt";
   QFile fileTxt(fileTxtName);
   QString element = "contributeur";

   // Ouverture du fichier texte en lecture seule et en mode texte (Sort de la fonction si le fichier ne peut etre ouvert).
   if(!fileTxt.open(QIODevice::WriteOnly | QIODevice::Text))
       return false;
   QTextStream out(&fileTxt);//Objet pour l'ecriture du fichier texte

   // Ouverture du fichier XML en lecture seule et en mode texte (Sort de la fonction si le fichier ne peut etre ouvert).
   if (!fileXml.open(QFile::ReadOnly | QFile::Text))
       return false;

   // Initialise l'instance reader avec le flux XML venant de file
   reader.setDevice(&fileXml);

   //Le but de cette boucle est de parcourir le fichier et de vérifier si l'on est a les éléments "contributeur" et "age".
   reader.readNext();
   while (!reader.atEnd())
   {
       if (reader.isStartElement())
       {
           //Si l'element est celui recherché
           if(reader.name() == element)
           {
               //Si nous venont de trouver un contributeur, il faut chercher son nom.
               if (element == "contributeur")
                   element = "nom";

               //Si nous venant de trouvez le nom du contributeur, il faut chercher son age aprés avoir renségné le fichier texte.
               else if (element == "nom")
               {
                   out << reader.readElementText();
                   element = "age";
               }

               //Si nous avons trouvez l'age du contributeur, Nous pouvons chercher l'existence d'une autre contributeur aprés avoir renseigné le fichier texte
               else if (element == "age")
               {
                   out << " a " << reader.readElementText() << " ans\r";
                   element = "contributeur";
               }
           }
       }
       else if (reader.isEndElement())
       {
           // Permet d'éviter de chercher des informations manquantes au detriments de la recherche de contributeur
           if(reader.name() == "contributeur")
           {
               if(element == "age")
                   out << " n'a pas donné(e) son age\r";
               element = "contributeur";
           }
       }
       reader.readNext(); // On va au prochain élément
   }
   fileXml.close();
   fileTxt.close();
*/
void dial_1Asm::writerXml(QString dir)
{

       ASMDATA *AsmData = p_asm->GetData(m_use);
       QString fileXmlName = dir + "/" + "config_asm" + QString("%1").arg(m_use,2, 10, QChar('0'))+ ".xml";
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
       u16 *datatemp = (u16 *) malloc(sizeof(ASMDATA));
       memcpy(datatemp,AsmData,sizeof(ASMDATA));
       for (unsigned int i=0;i<sizeof(ASMDATA)/2;i++) {
            writer.writeStartElement("REG");
            writer.writeAttribute("TITLE", "");
            writer.writeAttribute("ADDRESS", QString("0x%1").arg((u16) i,4,16,QChar('0')));
            writer.writeAttribute("VALUE", QString("0x%1").arg((u16) datatemp[i],4,16,QChar('0')));
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

void dial_1Asm::on_AsmStart_clicked()
{
    int ret;
    QString FileRecord ="";
    QDir dir(Path);
    p_asm->setConsole(ui->AsmConsole);
    if (ui->SaveFile->isChecked()) {
        QDate mDate(QDate::currentDate());
        QString date = mDate.toString("MMddyy");


        dir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot | QDir::NoSymLinks);
        dir.setSorting(QDir::Name);

        QStringList nameFilter;
        nameFilter << "Run_*";

        QFileInfoList list = dir.entryInfoList(nameFilter,QDir::Dirs);

        QList <long> index;
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            index << fileInfo.fileName().split("_")[1].toLong();
        }
        long temp = 1;
        if (index.size() > 0)
        {
          std::sort(index.begin(), index.end(), std::less<int>());
          temp = index.at(index.count()-1)+1;
        }
        FileRecord = "Run_"+ QString("%1").arg(temp,6, 10, QChar('0'));
        dir.mkdir(FileRecord);
        ui->RunNb->display((double)temp);
    } else if (ui->SaveTest->isChecked()) {
        FileRecord = "RunTest";
        dir.mkdir(FileRecord);

    }
    QString strtemp = Path + "/" + FileRecord;
	 if (!FileRecord.isEmpty()) ipcSend(IPCRECORD,strtemp.toStdString().c_str());
    else ipcSend(IPCWITHOUTFILE,"");
    ipcSend(IPCDAQ,ui->MonitoringValue->value());

    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.RegSpare2 &= 0xff8f;
    u16 temp = (ui->Mezza2->checkState() << 5) | (ui->Mezza1->checkState() << 4) | (ui->Mezza0->checkState() << 3);
    AsmData->rw.RegSpare2 |= temp;
    p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x92 , (u16 *) &AsmData->rw.RegSpare2),"Actived Mezzanine " + QString::number(AsmData->rw.RegSpare2,16));
    p_asm->Dump(m_mask);
    writerXml(strtemp);
    int loop=0;
    do {
        p_asm->Message(p_asm->RunCmd(m_mask,m_chan,1),"Start Asm");
    }  while ((ret != NO_ERROR) && (loop++ <4));

    decodeStatus();



}

void dial_1Asm::on_AsmStop_clicked()
{
    int ret;
    int loop=0;
    do {
        p_asm->Message(p_asm->RunCmd(m_mask,m_chan,0),"Start Asm");
    }  while ((ret != NO_ERROR) && (loop++ <4));
    ipcSend(IPCSTOP,"Stop");
    decodeStatus();
}

void dial_1Asm::on_AsmCalib_clicked()
{
    Dial_calib dl(NULL,p_asm,NULL,m_use, p_fe,m_mask);
    dl.setModal(true);
    dl.exec();
//    decodeStatus();
}

void dial_1Asm::on_AsmFreqPied_valueChanged(int arg1)
{
    double freq = (1/(double) (arg1*16)*1000);
    ui->labelFreq->setText(QString::number(freq,'f',4)+ " KHz");
}

void dial_1Asm::on_MonitoringValue_editingFinished()
{
    ipcSend(IPCINTERVAL,ui->MonitoringValue->value());
}

void dial_1Asm::on_SaveFile_clicked()
{
    ui->RunNb->setEnabled(ui->SaveFile->isChecked());
    ui->labelRunNb->setEnabled(ui->SaveFile->isChecked());
    if (ui->SaveFile->isChecked()) ui->SaveTest->setChecked(false);
}

void dial_1Asm::on_SaveTest_clicked()
{
    ui->SaveFile->setChecked(not ui->SaveTest->isChecked());
    ui->labelRunNb->setEnabled(ui->SaveFile->isChecked());
}

void dial_1Asm::on_Hv_clicked()
{
    Dial_HvONOff dl(p_hv);
    dl.setModal(true);
    dl.exec();
}

void dial_1Asm::on_NbSamples_editingFinished()
{
    int ret;
    u16 tmp = ui->NbSamples->value();
    p_asm->Message(ret=p_asm->WriteCmd(m_mask, m_chan, 1, 0x84 , (u16 *) &tmp),"Nb Samples " + QString::number(ui->NbSamples->value()));
}

void dial_1Asm::on_PhaseDrsAdc_activated(int index)
{
     ASMDATA *AsmData = p_asm->GetData(m_use);
     uint32_t Phase = t_phase[index] <<10;
     AsmData->rw.Jitter_Clean[3] &= 0xFFFE03ff;
     AsmData->rw.Jitter_Clean[3] |= Phase;

     p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x72 , (u16 *) &AsmData->rw.Jitter_Clean[3]),"Shift Clock DRS-ADC");
     p_asm->Message(p_asm->ConfigCdce62005(m_mask,m_chan,4,false),"Config CDCE62005");
     p_asm->Message(p_asm->CalibCmd(m_mask,m_chan,SYNCCDCE),"Sync Cdce 62005");

}

void dial_1Asm::on_cfgRegDrs_editingFinished()
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.RegSpare3 = ui->cfgRegDrs->value();

    p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x93 , (u16 *) &AsmData->rw.RegSpare3),"Reg Cfg Drs");
    p_asm->Message(p_asm->CalibCmd(m_mask,m_chan,INITCFGDRS),"Init Cfg Drs");

}
