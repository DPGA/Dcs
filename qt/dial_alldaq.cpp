//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_alldaq.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#include <iostream>
using namespace std;

#include "dial_alldaq.h"
#include "ui_dial_alldaq.h"
#include <QDir>
#include <QDate>
#include <QXmlStreamWriter>
#include <dial_mezza.h>
#include <unistd.h>

//===============================================
//===============================================
Dial_AllDaq::Dial_AllDaq(CAmc *amc, CAsm* asmm, CThor *thor, u32 mask, u16 seq, QString path, u64 TrigMaskThor, QWidget *parent) :
  QDialog(parent),ipc(),
  ui(new Ui::Dial_AllDaq)
{
  p_amc = amc;
  p_asm = asmm;
  p_thor = thor;
  m_usemask = mask;
  Path= path;
  m_Onsequence = seq & 0xff;
  m_Offsequence = (seq >> 8) & 0xff;
  usage(m_usemask);
  ui->setupUi(this);
  p_amc->SetMsgBox(ui->textEdit_Msg);
  p_asm->SetMsgBox(ui->textEdit_Msg);
  p_thor->SetMsgBox(ui->textEdit_Msg);
  ui->checkBox_Amc->setCheckState(Qt::Checked);
  m_daq = GetStatus();
  ui->ThorMaskTrig->setText("0x" + QString::number(TrigMaskThor,16));
  ui->radioButton_seqON_0->setText("Amc  Thor Asm");
  ui->radioButton_seqON_1->setText("Amc  Asm  Thor");
  ui->radioButton_seqON_2->setText("Thor Amc  Asm");
  ui->radioButton_seqON_3->setText("Thor Asm  Amc");
  ui->radioButton_seqON_4->setText("Asm  Amc  Thor");
  ui->radioButton_seqON_5->setText("Asm  Thor Amc");
  ui->radioButton_seqOFF_0->setText("Asm  Thor Amc");
  ui->radioButton_seqOFF_1->setText("Thor Asm  Amc");
  ui->radioButton_seqOFF_2->setText("Asm  Amc  Thor");
  ui->radioButton_seqOFF_3->setText("Amc  Asm  Thor");
  ui->radioButton_seqOFF_4->setText("Thor Amc  Asm");
  ui->radioButton_seqOFF_5->setText("Amc  Thor Asm");
  switch (m_Onsequence ) {
  case 0  : ui->radioButton_seqON_0->setChecked(true); break;
  case 1  : ui->radioButton_seqON_1->setChecked(true); break;
  case 2  : ui->radioButton_seqON_2->setChecked(true); break;
  case 3  : ui->radioButton_seqON_3->setChecked(true); break;
  case 4  : ui->radioButton_seqON_4->setChecked(true); break;
  case 5  : ui->radioButton_seqON_5->setChecked(true); break;
  default : ui->radioButton_seqON_6->setChecked(true); break;
  }
  switch (m_Offsequence ) {
  case 0  : ui->radioButton_seqOFF_0->setChecked(true); break;
  case 1  : ui->radioButton_seqOFF_1->setChecked(true); break;
  case 2  : ui->radioButton_seqOFF_2->setChecked(true); break;
  case 3  : ui->radioButton_seqOFF_3->setChecked(true); break;
  case 4  : ui->radioButton_seqOFF_4->setChecked(true); break;
  case 5  : ui->radioButton_seqOFF_5->setChecked(true); break;
  default : ui->radioButton_seqOFF_6->setChecked(true); break;
  }
  for (int i=0;i<NB_ASM;++i) ColorCheck(i,true,true);
  tip();
  display();
  p_asmdt = p_asm->GetData(0);
  ui->NbSamples->setValue(p_asmdt->rw.NbSamples);
  ui->EnableRoi->setChecked((bool) (p_asmdt->rw.RegSpare3 & 0x100));
  bool Mode;
  if (p_asmdt->rw.Mode == 14) Mode = false; else Mode = true;
  ui->AsmModeThor->setChecked(Mode);

  p_thordt = p_thor->GetData();
  ui->ModeDaqThor->setCurrentIndex(p_thordt->rw.Trigger_Type & 0x0007);
  ui->FreqPiedThor->setValue(p_thordt->rw.PreTrig_PedFreq);
  ui->delayTrigthor->setValue(p_thordt->rw.Trig_Retardment);

}
//===============================================
//===============================================
Dial_AllDaq::~Dial_AllDaq()
{
  p_amc->SetMsgBox(NULL);
  p_asm->SetMsgBox(NULL);
  p_thor->SetMsgBox(NULL);
  delete ui;
}

//===============================================
//===============================================

void Dial_AllDaq::ColorCheck(int ch,bool ok,bool init)
{
  QString Color;
  if (init) Color = "background-color:#EFEbE7";
  else
    if (ok) Color = "background-color:green";
    else Color = "background-color:red";
  switch (ch) {
  case 0 : ui->checkBox_Asm_0->setStyleSheet(Color);break;
  case 1 : ui->checkBox_Asm_1->setStyleSheet(Color);break;
  case 2 : ui->checkBox_Asm_2->setStyleSheet(Color);break;
  case 3 : ui->checkBox_Asm_3->setStyleSheet(Color);break;
  case 4 : ui->checkBox_Asm_4->setStyleSheet(Color);break;
  case 5 : ui->checkBox_Asm_5->setStyleSheet(Color);break;
  case 6 : ui->checkBox_Asm_6->setStyleSheet(Color);break;
  case 7 : ui->checkBox_Asm_7->setStyleSheet(Color);break;
  case 8 : ui->checkBox_Asm_8->setStyleSheet(Color);break;
  case 9 : ui->checkBox_Asm_9->setStyleSheet(Color);break;
  case 10: ui->checkBox_Asm_10->setStyleSheet(Color);break;
  case 11: ui->checkBox_Asm_11->setStyleSheet(Color);break;
  }
}


//===============================================
//===============================================
//===============================================
//===============================================
void Dial_AllDaq::on_pushButton_Close_clicked()
{
  this->close();
}	// on_pushButton_Close_clicked

//===============================================
//===============================================
//===============================================
//===============================================

void Dial_AllDaq::on_checkBox_Amc_clicked()
{
  m_daq = 2;
  display();
}

void Dial_AllDaq::on_checkBox_Asm_0_clicked()
{
  m_daq = 2;
  state[0]= ui->checkBox_Asm_0->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_1_clicked()
{
  m_daq = 2;
  state[1]= ui->checkBox_Asm_1->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_2_clicked()
{
  m_daq = 2;
  state[2]= ui->checkBox_Asm_2->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_3_clicked()
{
  m_daq = 2;
  state[3]= ui->checkBox_Asm_3->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_4_clicked()
{
  m_daq = 2;
  state[4]= ui->checkBox_Asm_4->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_5_clicked()
{
  m_daq = 2;
  state[5]= ui->checkBox_Asm_5->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_6_clicked()
{
  m_daq = 2;
  state[6]= ui->checkBox_Asm_6->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_7_clicked()
{
  m_daq = 2;
  state[7]= ui->checkBox_Asm_7->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_8_clicked()
{
  m_daq = 2;
  state[8]= ui->checkBox_Asm_8->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_9_clicked()
{
  m_daq = 2;
  state[9]= ui->checkBox_Asm_9->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_10_clicked()
{
  m_daq = 2;
  state[10]= ui->checkBox_Asm_10->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Asm_11_clicked()
{
  m_daq = 2;
  state[11]= ui->checkBox_Asm_11->checkState();
  display();
}

void Dial_AllDaq::on_checkBox_Thor_clicked()
{
  m_daq = 2;
  state[THOR_CHAN]= ui->checkBox_Thor->checkState();
  display();
}


void Dial_AllDaq::on_checkBox_Clear_clicked()
{
  m_daq = 2;
  usage(0);
  ui->checkBox_Amc->setCheckState(Qt::Unchecked);
  display();
}

void Dial_AllDaq::on_checkBox_All_clicked()
{
  m_daq = 2;
  usage(0xFFFF);
  ui->checkBox_Amc->setCheckState(Qt::Checked);
  display();

}
//===============================================
//===============================================
//===============================================
//===============================================
void Dial_AllDaq::tip(void)
{

  ui->pushButton_Start->setToolTip("Daq Start");
  ui->pushButton_Stop->setToolTip("Daq Stop");
  ui->lineEdit_stat->setToolTip("Current DAQ Status");


  ui->radioButton_seqON_0->setToolTip("Daq Start Sequence 0");
  ui->radioButton_seqON_1->setToolTip("Daq Start Sequence 1");
  ui->radioButton_seqON_2->setToolTip("Daq Start Sequence 2");
  ui->radioButton_seqON_3->setToolTip("Daq Start Sequence 3");
  ui->radioButton_seqON_4->setToolTip("Daq Start Sequence 4");
  ui->radioButton_seqON_5->setToolTip("Daq Start Sequence 5");
  ui->radioButton_seqON_6->setToolTip("Daq Start Sequence 6");
  ui->radioButton_seqOFF_0->setToolTip("Daq Stop Sequence 0");
  ui->radioButton_seqOFF_1->setToolTip("Daq Stop Sequence 1");
  ui->radioButton_seqOFF_2->setToolTip("Daq Stop Sequence 2");
  ui->radioButton_seqOFF_3->setToolTip("Daq Stop Sequence 3");
  ui->radioButton_seqOFF_4->setToolTip("Daq Stop Sequence 4");
  ui->radioButton_seqOFF_5->setToolTip("Daq Stop Sequence 5");
  ui->radioButton_seqOFF_6->setToolTip("Daq Stop Sequence 6");
  ui->pushButton_Close->setToolTip("Close this window"),
    ui->textEdit_Msg->setToolTip("Message Box");
  ui->checkBox_Amc->setToolTip("Include Amc in the Sequential Action");
  ui->checkBox_Asm_0->setToolTip("Include Asm 0 in the Sequential Action");
  ui->checkBox_Asm_1->setToolTip("Include Asm 1 in the Sequential Action");
  ui->checkBox_Asm_2->setToolTip("Include Asm 2 in the Sequential Action");
  ui->checkBox_Asm_3->setToolTip("Include Asm 3 in the Sequential Action");
  ui->checkBox_Asm_4->setToolTip("Include Asm 4 in the Sequential Action");
  ui->checkBox_Asm_5->setToolTip("Include Asm 5 in the Sequential Action");
  ui->checkBox_Asm_6->setToolTip("Include Asm 6 in the Sequential Action");
  ui->checkBox_Asm_7->setToolTip("Include Asm 7 in the Sequential Action");
  ui->checkBox_Asm_8->setToolTip("Include Asm 8 in the Sequential Action");
  ui->checkBox_Asm_9->setToolTip("Include Asm 9 in the Sequential Action");
  ui->checkBox_Asm_10->setToolTip("Include Asm 10 in the Sequential Action");
  ui->checkBox_Asm_11->setToolTip("Include Asm 11 in the Sequential Action");
  ui->checkBox_Thor->setToolTip("Include Thor in the Sequential Action");
  ui->checkBox_All->setToolTip("All Modules Selection in the Sequential Action");
  ui->checkBox_Clear->setToolTip("Clear Selection in the Sequential Action");

}	// Tip
//===============================================
//===============================================
void Dial_AllDaq::display(void)
{
  QCheckBox * ck[NB_CHAN] = { ui->checkBox_Asm_0, ui->checkBox_Asm_1, ui->checkBox_Asm_2, ui->checkBox_Asm_3, ui->checkBox_Asm_4, ui->checkBox_Asm_5,
			      ui->checkBox_Asm_6, ui->checkBox_Asm_7, ui->checkBox_Asm_8, ui->checkBox_Asm_9, ui->checkBox_Asm_10, ui->checkBox_Asm_11,
			      ui->checkBox_Thor };

  int ok = 0;
  m_usemask = 0;
  for(int i=0; i<NB_CHAN; i++) {
    ck[i]->setCheckState(state[i]);
    if(state[i] == Qt::Checked) {
      ok++;
      m_usemask |= 1 << i;
    }
  }
  if((ok == NB_CHAN) && (ui->checkBox_Amc->isChecked()== true))
    ui->checkBox_All->setCheckState(Qt::Checked);
  else
    ui->checkBox_All->setCheckState(Qt::Unchecked);
  if((ok == 0) && (ui->checkBox_Amc->isChecked()== false))
    ui->checkBox_Clear->setCheckState(Qt::Checked);
  else
    ui->checkBox_Clear->setCheckState(Qt::Unchecked);

  switch (m_daq) {
  case 0 : ui->lineEdit_stat->setText("Stopped"); break;
  case 1 : ui->lineEdit_stat->setText("Started"); break;
  default: ui->lineEdit_stat->setText("Unknown"); break;
  }
}	// Display

//===============================================&tmp
//===============================================
void Dial_AllDaq::usage(u32 use)
{
  for(int i=0; i<NB_CHAN; i++) {
    state[i] = use & (1<<i) ? (Qt::Checked) : (Qt::Unchecked);
  }
}	// usage

//===============================================
//===============================================
int Dial_AllDaq::AmcSwitch(u32 run)
{
  QString str;
  int ret = NO_ERROR;
  if(ui->checkBox_Amc->isChecked()) {
    str = (run ==1) ? ("Start") : ("Stop");
    ui->textEdit_Msg->append("Amc Run Command " + str);
    ret = p_amc->RunCmd(run, 0);
    if(ret != NO_ERROR) {
      ui->textEdit_Msg->setTextColor("red");
      ui->textEdit_Msg->append("Operation Failed");
      ui->textEdit_Msg->setTextColor(m_color);
    }
  }
  return (ret);
}	// AmcSwitch

//===============================================
//===============================================
int Dial_AllDaq::ThorSwitch(u32 run)
{
  QString str;
  int ret = NO_ERROR;
  if(ui->checkBox_Thor->isChecked()) {
    str = (run ==1) ? ("Start") : ("Stop");
    ui->textEdit_Msg->append("Thor Run Command " + str);
    ret = p_thor->RunCmd(BROADCAST_FE, run);
    if(ret != NO_ERROR) {
      ui->textEdit_Msg->setTextColor("red");
      ui->textEdit_Msg->append("Operation Failed");
      ui->textEdit_Msg->setTextColor(m_color);
    }
  }
  return (ret);
}	// ThorSwitch

//===============================================
//===============================================
int Dial_AllDaq::AsmSwitch(u32 run)
{
  QString str;
  int ret = NO_ERROR;
  bool Err[NB_ASM];
  u32 mask = m_usemask & 0xfff;
  if(mask !=0 ) {
    str = (run==1) ? ("Start") : ("Stop");
    ui->textEdit_Msg->append("Asm Run Command " + str);
    ret = p_asm->RunCmdMulti(mask, BROADCAST_FE, run,&Err[0]);
    if(ret != NO_ERROR) {
      ui->textEdit_Msg->setTextColor("red");
      ui->textEdit_Msg->append("Operation Failed");
      ui->textEdit_Msg->setTextColor(m_color);
    }

    for(int i=0;i<NB_ASM;++i)
      if (Err[i]) ColorCheck(i,false,false);
      else ColorCheck(1,true,false);
  }
  return (ret);
}	// AsmSwitch

//===============================================
//===============================================
int Dial_AllDaq::FastSwitch(u32 run)
{
  int ret;
  QString str;

  str = (run ==1) ? ("Start") : ("Stop");
  ui->textEdit_Msg->append("All Run Command " + str);
  ret = p_amc->RunCmd(run, m_usemask);
  if(ret != NO_ERROR) {
    ui->textEdit_Msg->setTextColor("red");
    ui->textEdit_Msg->append("Operation Failed");
    ui->textEdit_Msg->setTextColor(m_color);
  }
  return (ret);
}	// FastSwitch

//===============================================
//===============================================
//===============================================
//===============================================
//===============================================
//===============================================

void Dial_AllDaq::on_pushButton_Stop_clicked()
{
  int ret[3] = {0,0,0};
  const u32 run = 0;
  u64 tmp = ComputeMask(m_usemask,p_thordt->rw.PreTrig_Mask);

  m_color = ui->textEdit_Msg->textColor();
  ui->textEdit_Msg->clear();
  if(ui->checkBox_Thor->isChecked()) {
    p_thor-> WriteCmd(BROADCAST_FE,4,p_thor->GetMaskAddr(),(u16*)&tmp);
    ui->textEdit_Msg->append(QString("New Mask 0x").arg(tmp, 12, 16, QChar('0')));
  }
  if (ui->radioButton_seqOFF_0->isChecked()) {
    ret[2] = AsmSwitch(run);
    ret[1] = ThorSwitch(run);
    ret[0] = AmcSwitch(run);
  } else if (ui->radioButton_seqOFF_1->isChecked()) {
    ret[1] = ThorSwitch(run);
    ret[2] = AsmSwitch(run);
    ret[0] = AmcSwitch(run);
  } else if (ui->radioButton_seqOFF_2->isChecked()) {
    ret[2] = AsmSwitch(run);
    ret[0] = AmcSwitch(run);
    ret[1] = ThorSwitch(run);
  } else if (ui->radioButton_seqOFF_3->isChecked()) {
    ret[0] = AmcSwitch(run);
    ret[2] = AsmSwitch(run);
    ret[1] = ThorSwitch(run);
  } else if (ui->radioButton_seqOFF_4->isChecked()) {
    ret[1] = ThorSwitch(run);
    ret[0] = AmcSwitch(run);
    ret[2] = AsmSwitch(run);
  } else if (ui->radioButton_seqOFF_5->isChecked()) {
    ret[0] = AmcSwitch(run);
    ret[1] = ThorSwitch(run);
    ret[2] = AsmSwitch(run);
  } else if (ui->radioButton_seqOFF_6->isChecked()) {
    ret[0] = FastSwitch(run);
  }

  if((ret[0] == NO_ERROR) && (ret[1] == NO_ERROR) && (ret[2] == NO_ERROR)) {
    m_daq = 0;
    ui->textEdit_Msg->setTextColor("green");
    ui->textEdit_Msg->append("Operation Successfull");
  }
  else
    m_daq = -1;
  display();
  ui->textEdit_Msg->setTextColor(m_color);
  ipcSend(IPCSTOP,"Stop");

} // on_pushButton_Stop_clicked

//===============================================
//===============================================
//===============================================
//===============================================
u64 Dial_AllDaq::ComputeMask(u32 use, u64 def_mask)
{
  u64 mask = 0;
  u64 seven = 7;
  for(u64 i=0; i<NB_ASM; i++) {
    if(use & (1<<i)) {
      mask |= seven << (i*3);
      /*			mask |= 1 << (i*3);
				mask |= 1 << ((i*3) +1);
				mask |= 1 << ((i*3) +2);
      */
    }
    cout << "use 0x " << hex  << use << "\t" << (1<<i) << "\t" << dec << i << " Compute 0x" << hex << mask << dec << endl; 
  }
  mask &= def_mask;
  cout << "mask applied " << hex << mask << "PreTrig_Mask " << p_thordt->rw.PreTrig_Mask << dec << endl;
  //****************************
  //   mask = 0x7000;
  //****************************
  return mask;
}
//===============================================
//===============================================
void Dial_AllDaq::SavetoFile()

{
  int ret;
  u32 m_mask = m_usemask & 0xfff;
  QString FileRecord ="";
  QDir dir(Path);

  p_asm->setConsole(ui->textEdit_Msg);
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
  ipcSend(IPCNBSAMPLES,ui->NbSamples->value());
  sleep(1);
  // QTimer::singleShot(200, this, SLOT(updateCaption()));
 
 
  ipcSend(IPCDAQ,ui->MonitoringValue->value());

  for (int i=0;i< NB_ASM;i++) {
    p_asm->Dump(1 << i,i);
    writerXml(strtemp,(u16 *)&p_asmdt[i],sizeof(ASMDATA),"ConfigAsm_",i);
  }

  p_thor->Dump();
  writerXml(strtemp,(u16 *) p_thor->GetData(),sizeof(THORDATA),"ConfigThor_",0);

  //void writerXml                    (QString dir,u16 *pData,u32 sizeData,QString fileName,u8 num)
}





void Dial_AllDaq::on_pushButton_Start_clicked()
{
  //*******************************

  ipcSend(IPCDAQ,ui->MonitoringValue->value());
  int ret[3] = {0,0,0};
  u32 run = 1;
  u32 mask = m_usemask & 0xfff;
  qDebug() << "mask = " << mask;
  u64 tmp = ComputeMask(mask,0xfffffffff);//ui->ThorMaskTrig->text().toInt()) ; //0xfffffffff); //ui->ThorTrigMask->value()); //p_thordt->rw.PreTrig_Mask);
  m_color = ui->textEdit_Msg->textColor();
  ui->textEdit_Msg->clear();

  u16 Mode = 0xe;
  u16 Roi  = 0xff;
  if (ui->AsmModeThor->isChecked()) Mode = 0x0;
  if (ui->EnableRoi->isChecked()) Roi = 0x1ff;
  p_asm->Message(p_asm->WriteCmd(mask, BROADCAST_FE, 1, 0x93 , (u16 *) &Roi),"Select Mode Roi ");
  p_asm->Message(p_asm->WriteCmd(mask, BROADCAST_FE, 1, 0x21 , (u16 *) &Mode),"Select Mode Daq ");
  u16 temp = ui->NbSamples->value();
  p_asm->Message(p_asm->WriteCmd(mask, BROADCAST_FE, 1, 0x84 , (u16 *) &temp),"Select Nb Samples ");

  if(ui->checkBox_Thor->isChecked()) {
    cout << "Mask Addr 0x" << hex  << p_thor->GetMaskAddr() << "\tUse 0x"<< m_usemask << "\tPretrigMask 0x" << p_thordt->rw.PreTrig_Mask << "\tMask 0x" << hex << tmp << dec << endl;
    //       p_thor-> WriteCmd(BROADCAST_FE,1,p_thor->GetMaskAddr(),&(p_thordt->rw.PreTrig_Mask));
    //       p_thor-> WriteCmd(BROADCAST_FE,1,p_thor->GetMaskAddr(),&tmp);
    p_thor-> WriteCmd(BROADCAST_FE,4,p_thor->GetMaskAddr(),(u16*)&tmp);
    ui->textEdit_Msg->append(QString("New Mask 0x%1").arg(tmp, 12, 16, QChar('0')));
  }

  SavetoFile(); // Dump All Board and Save Config

  if(p_amc->ResetCmd((u32) 0x1000) == NO_ERROR) {
    qDebug() << "command reset ok";
      ui->textEdit_Msg->setTextColor(QColor("green"));
      ui->textEdit_Msg->append("SuccessFul");
  } else {
    qDebug() << "Command reset error";
      ui->textEdit_Msg->setTextColor(QColor("red"));
      ui->textEdit_Msg->append("Error during execution");
  }
  sleep(1);
  if (ui->radioButton_seqON_0->isChecked()) {
    ret[0] = AmcSwitch(run);
    ret[1] = ThorSwitch(run);
    ret[2] = AsmSwitch(run);
  } else if (ui->radioButton_seqON_1->isChecked()) {
    ret[0] = AmcSwitch(run);
    ret[2] = AsmSwitch(run);
    ret[1] = ThorSwitch(run);
  } else if (ui->radioButton_seqON_2->isChecked()) {
    ret[1] = ThorSwitch(run);
    ret[0] = AmcSwitch(run);
    ret[2] = AsmSwitch(run);
  } else if (ui->radioButton_seqON_3->isChecked()) {
    ret[1] = ThorSwitch(run);
    ret[2] = AsmSwitch(run);
    ret[0] = AmcSwitch(run);
  } else if (ui->radioButton_seqON_4->isChecked()) {
    ret[2] = AsmSwitch(run);
    ret[0] = AmcSwitch(run);
    ret[1] = ThorSwitch(run);
  } else if (ui->radioButton_seqON_5->isChecked()) {
    ret[2] = AsmSwitch(run);
    ret[1] = ThorSwitch(run);
    ret[0] = AmcSwitch(run);
  } else if (ui->radioButton_seqON_6->isChecked()) {
    ret[0] =FastSwitch(run);
  }
  if((ret[0] == NO_ERROR) && (ret[1] == NO_ERROR) && (ret[2] == NO_ERROR)) {
    m_daq = 1;
    ui->textEdit_Msg->setTextColor("green");
    ui->textEdit_Msg->append("Operation Successfull");
  } else
    m_daq = 2;
  ui->textEdit_Msg->setTextColor(m_color);
  display();

}   // on_pushButton_Start_clicked

//===============================================
//===============================================
//===============================================
//===============================================
int  Dial_AllDaq::GetStatus(void)
{
  int stat = -1;
  int tmp;
  for(int chan=0; chan<NB_ASM; chan++) {
    p_asmdt = p_asm->GetData(chan);
    if(m_usemask & (1 <<chan)) {           // In Use
      tmp = (p_asmdt->rd.Stat_GXB >> 31) & 0x1;
      switch (stat) {
      case -1 : stat = tmp; break;
      case 0  : stat = (stat!= tmp) ? 2 : 0; break;
      case 1  : stat = (stat!= tmp) ? 2 : 1; break;
	//                case 2  : stat = 2; break;
      default : stat = 2;
      }
      cout << "Asm chan : " << chan << "\ttmp : " << tmp << "\tstat : " << stat << endl;
    }
  }
  if(m_usemask & (1 <<THOR_CHAN)) {
    p_thordt = p_thor->GetData();
    tmp = (p_thordt->rd.Global_Status >>8) & 0x1;
    switch (stat) {
    case -1 : stat = tmp; break;
    case 0  : stat = (stat!= tmp) ? 2 : 0; break;
    case 1  : stat = (stat!= tmp) ? 2 : 1; break;
      //            case 2  : stat = 2; break;
    default : stat = 2;
    }
    cout << "Thor : 0x" << hex << m_usemask << dec << "\ttmp : " << tmp << "\tstat : " << stat << endl;
  }
  if(ui->checkBox_Amc->isChecked()== true) {
    p_amcdt = p_amc->GetData(); //
    tmp = p_amcdt->cnfg.daq;
    switch (stat) {
    case -1 : stat = tmp; break;
    case 0  : stat = (tmp == 1) ? 2 : 0; break;
    case 1  : stat = (tmp == 1) ? 1 : 2; break;
      //           case 2  : stat = 2; break;
    default : stat = 2;
    }
    cout << "Amc : "  << "\ttmp : " << tmp << "\tstat : " << stat << endl;
  }

  return(stat);
}   // GetStatus

//===============================================
//===============================================
//===============================================
//===============================================

void Dial_AllDaq::on_ModeDaqThor_activated(int index)
{
  THORDATA *dtThor = p_thor->GetData();
  dtThor->rw.Trigger_Type = index;
  p_thor-> WriteCmd(BROADCAST_FE,1,0x19,&dtThor->rw.Trigger_Type);
}

void Dial_AllDaq::on_FreqPiedThor_valueChanged(int arg1)
{
  double freq = (1/((double) (arg1*16)) * 1000);
  ui->Label_freqPiedthorCalcul->setText(QString::number(freq) + "  KHz");
}

void Dial_AllDaq::on_FreqPiedThor_editingFinished()
{
  u16 temp = ui->FreqPiedThor->value();
  p_thor-> WriteCmd(BROADCAST_FE,1,0x31,&temp); //&p_thordt->rw.PreTrig_PedFreq);
}

void Dial_AllDaq::on_delayTrigthor_editingFinished()
{
  u16 temp = ui->delayTrigthor->value();
  p_thor-> WriteCmd(BROADCAST_FE,1,0x33,&temp); //b&p_thordt->rw.Trig_Retardment);
}

void Dial_AllDaq::on_delayTrigthor_valueChanged(int arg1)
{
  double delay = (double) (arg1 *6.66);
  ui->Label_DelayCalcul->setText(QString::number(delay)+ "  ns");
}

void Dial_AllDaq::on_AsmModeThor_clicked()
{
  if (ui->AsmModeThor->isChecked()) {

  }
}

void Dial_AllDaq::on_MonitoringValue_editingFinished()
{
  ipcSend(IPCINTERVAL,ui->MonitoringValue->value());
}


void Dial_AllDaq::setMask(u_int64_t val)
{
  ui->ThorMaskTrig->setText("0x"+QString::number(val,16));

}

void Dial_AllDaq::on_pushButton_clicked()
{
  Dial_mezza MezzaSel(ui->ThorMaskTrig->text().toLong());
  QObject::connect(&MezzaSel, SIGNAL(valueChanged(u_int64_t)),this, SLOT(setMask(u_int64_t)));
  MezzaSel.setModal(true);
  MezzaSel.exec();
}

void Dial_AllDaq::on_DeadTime_editingFinished()
{
  u16 temp = ui->DeadTime->value();
  THORDATA *dtThor = p_thor->GetData();
  u16 tt = dtThor->rw.Trigger_Type & 0x7;
  dtThor->rw.Trigger_Type = (temp <<4) |tt;
  qDebug() << dtThor->rw.Trigger_Type;
  p_thor-> WriteCmd(BROADCAST_FE,1,0x19,&dtThor->rw.Trigger_Type);
  ui->CalDeadTime->setText(QString::number(1e9/((temp << 6)*5.55))+" hz");
}


void Dial_AllDaq::on_PulseTrigAsm_editingFinished()
{
    u32 temp = ui->PulseTrigAsm->value();
    for (int ch=0;ch<NB_ASM;ch++) {
        ASMDATA *dtAsm = p_asm->GetData(ch);
        u32 Thr = dtAsm->rw.Filter_Thr_L & 0x0FFFFFF;
        dtAsm->rw.Filter_Thr_L = (temp <<28) | Thr;
        qDebug() << dtAsm->rw.Filter_Thr_L << " thr " << Thr;
        int mask = 1 << ch;
        p_asm->Message(p_asm->WriteCmd(mask,BROADCAST_FE,2,0x8a, (u16 *) &dtAsm->rw.Filter_Thr_L),"Pulse trigger Asm ");
    }

}

void Dial_AllDaq::on_LowThrAsm_editingFinished()
{
    u32 temp = ui->LowThrAsm->value();
    for (int ch=0;ch<NB_ASM;ch++) {
      ASMDATA *dtAsm = p_asm->GetData(ch);
      int mask = 1 << ch;
      for (int i=0;i<24;i++) {

        dtAsm->rw.Threshold[i].Low = ui->LowThrAsm->value();
      }
      p_asm->Message(p_asm->WriteCmd(mask,BROADCAST_FE,48,0x3c, (u16 *) & dtAsm->rw.Threshold[0].Low),"Threshold low Asm ");

    }
    p_asm->Message(p_asm->CalibCmd(0xFff,BROADCAST_FE,InitT1),"Init T1");
    p_asm->Message(p_asm->CalibCmd(0xFff,BROADCAST_FE,InitT2),"Init T2");
    p_asm->Message(p_asm->CalibCmd(0xFff,BROADCAST_FE,InitT3),"Init T3");
}
