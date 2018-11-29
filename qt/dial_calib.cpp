#include "dial_calib.h"
#include "ui_dial_calib.h"
#include "command_error.h"
#include "Asm.h"
#include <QThread>
#include "myplotsqwt.h"



Dial_calib::Dial_calib(CAmc *amc, CAsm* asmm, CThor *thor, u32 use, u16 *frontend,u32 usemask, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dial_calib)
    //DecodeFrame()
//===============================================
//===============================================
{
    ui->setupUi(this);

    p_asm = asmm;
    p_amc = amc;
    p_thor = thor;
//    Monitor = NULL;
    m_use = use;
    m_usemask = usemask;
    m_send = false;
    p_fe  = frontend;
    m_chan = frontend[use];
    m_mask = 1 << use;
    p_asm->SetMsgBox(ui->textEdit_Msg);
    setup();
    m_send = true;
    ui->CalculFreqBar->hide();
    nbstep = 0;


}

void Dial_calib::setup()
//===============================================
//===============================================
{
    ui->FrontEnd->setValue(m_use);
    ModeDaq = 0;
    ASMDATA *AsmData = p_asm->GetData(m_use);
    ui->DWriteDenable->setCurrentIndex((AsmData->rw.RegSpare2 & 0xc000)>>14);
    ui->ChoicePattern->setCurrentIndex(12); //A33 par default
    double step=0.0;
    for (int i=0;i<50;i++) {
      ui->PhaseDrsAdc->addItem(QString::number(step,'f',1));
      step += 7.2;
      if (t_phase[i] == ((AsmData->rw.Jitter_Clean[4] & 0x0001FC00) >> 10)) ui->PhaseDrsAdc->setCurrentIndex(i);
    }
    on_FrontEnd_editingFinished();

    if ((AsmData->rw.Mode & 0xFFFE) == 1) ui->AndOrQuartet->setCurrentIndex(1);
    else ui->AndOrQuartet->setCurrentIndex(0);

    ui->ModeDaq->setCurrentIndex((AsmData->rw.Mode & 0xE) >> 1);
    ui->FreqPied->setValue(AsmData->rw.FreqPied);
 //   Lockpll[chan]->setText(QString("0x%1").arg(p_asmdt->rd.Pll_Lock,4, 16, QChar('0')));

    Ch << ui->Ch0 << ui->Ch1 << ui->Ch2 << ui->Ch3 << ui->Ch4 << ui->Ch5 << ui->Ch6 << ui->Ch7
       << ui->Ch8 << ui->Ch9 << ui->Ch10 << ui->Ch11 << ui->Ch12 << ui->Ch13 << ui->Ch14 << ui->Ch15
       << ui->Ch16 << ui->Ch17 << ui->Ch18 << ui->Ch19 << ui->Ch20 << ui->Ch21 << ui->Ch22 << ui->Ch23;
    for (auto c : Ch) c->setChecked(true);
    on_CalculFreq_clicked();
}
void Dial_calib::Error()
{


}

void Dial_calib::CalibAuto()
//===============================================
//===============================================
{
/*   const unsigned int Pattern[6] = {1652,826,413,2254,1127,2611};
   bool ok = false;
   int nbtry = 0;

   ShdMem = new ShmRingBuffer<SharedMemory>(CAPACITY,false,SHM_ASM_DATA);

   bool oneSec = ui->OneSec->isChecked();
   ui->OneSec->setChecked(true);
   ASMDATA *AsmData = p_asm->GetData(m_use);
   ui->textEdit_Msg->append("Calcul Frequency ...");
   // Test la frequence 20 Mhz
   do {
        on_CalculFreq_clicked();
        unsigned int freq = ui->ValFreq->value();
        if ((freq >20.5) || (freq <19.5)) {
            on_CalibVco_clicked();
        }
        else ok = true;
   } while (!ok && (nbtry++ <3));

   if (nbtry >=2) {
       ui->textEdit_Msg->append("Calcul Frequency bad");
       Error();
       return;
   }

   ui->ModeDaq->setCurrentIndex(7);
   ui->textEdit_Msg->append("Calcul Frequency ok");
   AsmData->rw.Mode &= 0xfff1;
   AsmData->rw.Mode |= 0xE;    // Mode Pied
   p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x21 , (u16 *) &AsmData->rw.Mode),"Select Mode Daq " + QString::number(AsmData->rw.Mode,16));
   AsmData->rw.RegSpare2 &= 0xff8f; // active mezzanine
   p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x92 , (u16 *) &AsmData->rw.RegSpare2),"Select Mode Daq " + QString::number(AsmData->rw.RegSpare2,16));
   AsmData->rw.FreqPied = 16384; // environ 4 Hz
   p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x91 , (u16 *) &AsmData->rw.FreqPied),"Select Freq Pied " + QString::number(AsmData->rw.FreqPied,16));
   u16 Mode = AsmData->rw.Mode & 0xfff0;
   Mode |= 0xE;
   p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x21 , (u16 *) &Mode),"Select Mode Daq Pied " + QString::number(AsmData->rw.Mode,16));

   // Start Pattern
   on_StartCalib_clicked();
   unsigned long NbStepClk[24];

   while (1) {
    Run1sec();
    for (int i=0;i<24;i++) {
        Mean[i] = 0.0;Stdev[i] = 0.0;NbStepClk[i]=0;
    }
    ReadShmData(m_use);

    if (((int)Mean[0] == 2513.0) ||  ((int)Mean[0] == 3304.0)) {
        for (int i=0;i < 10;++i) {
            ui->textEdit_Msg->append("Step Clk Adc = "+ QString::number(i));
            p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),PhaseClkAdc),"Phasing adc clock"); //Phasee adc Pll FPGA
        }
    }
    else break;
   }

   u16 Val[2];
   for (int i=0;i<24;i++) {
     qDebug() << i << "  " << Mean[i];
     ui->textEdit_Msg->append("Mean = "+ QString::number(i) + "  " +QString::number(Mean[i]));
     if (Mean[i] == 1652.0)  NbStepClk[i]=5;
     else if (Mean[i] == 826.0)   NbStepClk[i]=4;
     else if (Mean[i] == 413.0)   NbStepClk[i]=3;
     else if (Mean[i] == 2254.0)  NbStepClk[i]=2;
     else if (Mean[i] == 1127.0)  NbStepClk[i]=1;
     else if (Mean[i] == 2611.0)  NbStepClk[i]=0;
     else {NbStepClk[i]=0;qDebug() << "Error Pattern";}
   }

   while (1) {
       uint32_t Channel = 0;
       for (int i=0;i<24;i++) {
           if (NbStepClk[i] >0 ){ Channel |= (1 << i);NbStepClk[i]--;}
       }
       if (Channel == 0) break;
       Val[0] = ((Channel & 0xff0000) >> 16) ;
       Val[1] = Channel & 0xffff;
       p_asm->Message(p_asm->WriteCmd(m_mask,getFrontEnd(m_chan), 2, 0x8e , &Val[0]),"Pattern High =0x" + QString::number(Val[1],16) + QString::number(Val[0],16));
       p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),StepClockAdc),"Step Clock Data ");
       ui->textEdit_Msg->append("Step Clock = 0x"+QString::number(Val[1],16)+QString::number(Val[0],16));
   }


   Run1sec();
   ui->OneSec->setChecked(oneSec);
   on_StopCalib_clicked();
   */
}

Dial_calib::~Dial_calib()
//===============================================
//===============================================
{
    p_asm->SetMsgBox(NULL);
    p_asm->setConsole(NULL);
    delete ui;
//    if (Monitor) Monitor->setWindowModality(Qt::WindowModal);
}

void Dial_calib::Message(int ret,QString message,const QString endmessage)
//===============================================
//===============================================
{
    if(ret == NO_ERROR) {
        ui->textEdit_Msg->setTextColor("green");
        ui->textEdit_Msg->append(message + " => Operation Successfull  " + endmessage);
    } else {
        ui->textEdit_Msg->setTextColor("red");
        ui->textEdit_Msg->append(message + " => failed " + endmessage + "Ret = " + QString::number(ret,10));
    }
}
void Dial_calib::decodeStatus()
//===============================================
//===============================================
{
    p_asm->decodeStatus(NULL,ui->StatPll,m_mask,m_chan);
}

void Dial_calib::setTipPllStat()
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    ui->StatPll->setText(QString("0x%1").arg(AsmData->rd.Pll_Lock,4, 16, QChar('0')));
    QString str = QString("<b>Pll Locked</b> 0x%1").arg(AsmData->rd.Pll_Lock,4, 16, QChar('0'));
    str.append("<ul>");
    if(AsmData->rd.Pll_Lock & (1 << 0)) str.append("<li>Pll Drs 0			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 0        : <font color='red'>not Locked</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 1)) str.append("<li>Pll Drs 1			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 1        : <font color='red'>not Locked</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 2)) str.append("<li>Pll Drs 2			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 2        : <font color='red'>not Locked</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 3)) str.append("<li>Pll Cdce62005		: <font color='green'>Locked</li>");else str.append("<li>Pll Cdce62005    : <font color='red'>not Locked</font></li>");

    if(AsmData->rd.Pll_Lock & (1 << 4)) str.append("<li>Pll 40Mhz to		: <font color='green'>Locked</li>");else str.append("<li>Pll 40Mhz to     : <font color='red'>not Locked</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 5)) str.append("<li>Pll Config (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Config (FPGA): <font color='red'>not Locked</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 6)) str.append("<li>Pll Sample (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Sample (FPGA): <font color='red'>not Locked</font></li>");

    if(AsmData->rd.Pll_Lock & (1 << 7)) str.append("<li>Reg0 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg0 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 8)) str.append("<li>Reg1 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg1 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 9)) str.append("<li>Reg2 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg2 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 10)) str.append("<li>Reg3 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg3 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 11)) str.append("<li>Reg4 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg4 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 12)) str.append("<li>Reg5 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg5 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 13)) str.append("<li>Reg6 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg6 Cdce62005: <font color='red'>not ok</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 14)) str.append("<li>Reg7 Cdce62005 : <font color='green'>ok</li>");else str.append("<li>Reg7 Cdce62005: <font color='red'>not ok</font></li>");

  //  if(AsmData->rd.Pll_Lock & (1 << 13)) str.append("<li>Bad clock : <font color='red'>F_ClkAdc</li>");else str.append("<li>Good Clock : <font color='green'>f_clkadc</font></li>");
  //  if(AsmData->rd.Pll_Lock & (1 << 14)) str.append("<li>Bad clock : <font color='red'>c_clk</li>");else str.append("<li>Good Clock : <font color='green'>C_Clk</font></li>");
    if(AsmData->rd.Pll_Lock & (1 << 15)) str.append("<li>Active clock : <font color='green'>c_clk</li>");else str.append("<li>Active Clock : <font color='red'>f_clkadc</font></li>");
    ui->StatPll->setToolTip(str);
}

void Dial_calib::on_StartCalib_clicked()
//===============================================
//===============================================
{
    Pattern    = ui->ChoicePattern->currentIndex(); //cp.toInt(0,16);
	ui->textEdit_Msg->append("Pattern = 0x" + QString::number(Pattern,16));
	ui->textEdit_Msg->append("fe = 0x" + QString::number(getFrontEnd(m_chan),16));

 //   Pattern = 0x8152;
    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 1, 0x8d, &Pattern),"Write Pattern");
    u16 reg[10];
    p_asm->Message(p_asm->ReadCmd(m_mask, getFrontEnd(m_chan), 2, 0x8d, &reg[0]),"Read Value 0x" + QString::number(reg[0],16));

    p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),CalibAdc9222),"Mode Calibration");

    if (p_thor && (ui->ModeDaq->currentIndex() !=7)) {
        u_int16_t val = 0x310; //3 Hz
        Message(p_thor->WriteCmd(0x1f, 1, 0x31,&val),"Frequence Thor ");
    }

    Run1sec();

}

void Dial_calib::on_FrontEnd_editingFinished()
//===============================================
//===============================================
{
    m_use = ui->FrontEnd->value();
    m_chan = p_fe[m_use];
    m_mask = 1 << m_use;

    p_asm->Dump(m_mask);
    ASMDATA *AsmData = p_asm->GetData(m_use);

    ui->StatPll->setText(QString("0x%1").arg(AsmData->rd.Pll_Lock,4, 16, QChar('0')));
    ui->textEdit_Msg->append("m_use " + QString::number(m_use,10));
	ui->textEdit_Msg->append("m_mask 0x" + QString::number(m_mask,16));
    ui->textEdit_Msg->append("FeId 0x" + QString::number(m_chan,16));
    decodeStatus();
    //setTipPllStat();
}


//===============================================
//===============================================
u16 Dial_calib::getFrontEnd(u32 chan)
{
    u16 * p= p_fe;
    for(u32 i=0; i<NB_ASM; i++, p++) {
       if(i == chan)
           return (*p);
    }
    return(BROADCAST_FE);
}


void Dial_calib::on_StepClk_clicked()
{
//===============================================
//===============================================
	uint32_t Channel = 0;
	for (int i=0;i<24;i++) {
		if (Ch[i]->checkState()) 
			Channel |= (1 << i);
	}

   ui->textEdit_Msg->append("Channel step " + QString::number(Channel,16));
   u16 Val[2];

   Val[0] = ((Channel & 0xff0000) >> 16) ;
   Val[1] = Channel & 0xffff;
   p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x8e , &Val[0]),"Pattern High =0x" + QString::number(Val[1],16) + QString::number(Val[0],16));
   p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),StepClockAdc),"Step Clock Data ");
   Run1sec();
   decodeStatus();
}



void Dial_calib::on_StopCalib_clicked()
//===============================================
//===============================================
{
    if (p_amc) Message(p_amc->RunCmd(0, 0),"Stop Daq AMC");
    p_asm->Message(p_asm->RunCmd(m_mask,getFrontEnd(m_chan),0),"Stop Daq ASM");
    if (p_thor) Message(p_thor->RunCmd(0x1f,0),"Stop Daq Thor");
    p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),StopCalibAdc),"Stop PatternAdc");
    decodeStatus();
}

void Dial_calib::on_PhaseAdc_clicked()
//===============================================
//===============================================
{
    for (int i=0;i < ui->NbStep->value();++i)
        p_asm->Message(p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),PhaseClkAdc),"PhaseAdc"); //Phasee adc Pll FPGA
    nbstep += ui->NbStep->value();
	 ui->textEdit_Msg->append(QString::number(nbstep,10));
 //   Run1sec();
 //   decodeStatus();
}


void Dial_calib::on_PhaseDrsAdc_activated(int index)
//===============================================
//===============================================
{
    ui->textEdit_Msg->append(QString::number(m_chan,10));
    ASMDATA *AsmData = p_asm->GetData(m_use);
    uint32_t Phase = t_phase[index] <<10;
    AsmData->rw.Jitter_Clean[4] &= 0xFFFE03ff;
    AsmData->rw.Jitter_Clean[4] |= Phase;

    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x74 , (u16 *) &AsmData->rw.Jitter_Clean[4]),"Shift Clock DRS-ADC");
    p_asm->Message(p_asm->ConfigCdce62005(m_mask,getFrontEnd(m_chan),4,false),"Config CDCE62005");
    Run1sec();
    decodeStatus();
}


void Dial_calib::Run1sec()
//===============================================
//===============================================
{
    if (ui->StartDaq->text() == "Start Daq") {
        ui->StartDaq->setText("Stop Daq");
        if (p_amc) Message(p_amc->RunCmd(1, 0),"Start Amc");
        p_asm->Message(p_asm->RunCmd(m_mask,getFrontEnd(m_chan),1),"Start Asm");

        if (((m_usemask & 0x1000) == 0x1000) && (ui->ModeDaq->currentIndex() !=7) && p_thor)  {
            THORDATA *p_thordt = p_thor->GetData();

            u64 mask = 0;
            u64 seven = 7;
            for(u64 i=0; i<NB_ASM; i++) {
                if(m_mask & (1<<i)) mask |= seven << (i*3);
                mask &= p_thordt->rw.PreTrig_Mask;

            }
            p_thor-> WriteCmd(BROADCAST_FE,4,p_thor->GetMaskAddr(),(u16*)&mask);

            Message(p_thor->RunCmd(0x1f,1),"Start thor");
        }

        if (ui->OneSec->isChecked()) {
            QThread::sleep(1);
            p_asm->Message(p_asm->RunCmd(m_mask,getFrontEnd(m_chan),0),"Stop Asm");
            if (((m_usemask & 0x1000) == 0x1000) && (ui->ModeDaq->currentIndex() !=7))    Message(p_thor->RunCmd(0x1f,0),"Stop thor");
            if (p_amc) Message(p_amc->RunCmd(0, 0),"Stop Amc");
            ui->StartDaq->setText("Start Daq");
        }
    }
    else {
        p_asm->Message(p_asm->RunCmd(m_mask,getFrontEnd(m_chan),0),"Stop Asm");
        if (((m_usemask & 0x1000) == 0x1000) && (ui->ModeDaq->currentIndex() !=7))    Message(p_thor->RunCmd(0x1f,0),"Stop thor");
        if (p_amc) Message(p_amc->RunCmd(0, 0),"Stop Amc");
        ui->StartDaq->setText("Start Daq");
    }

 //   decodeStatus();
}

void Dial_calib::on_CalibVco_clicked()
//===============================================
//===============================================
{
    ui->textEdit_Msg->append(QString::number(m_chan,10));
    ASMDATA *AsmData = p_asm->GetData(m_use);

    AsmData->rw.Jitter_Clean[6] &= 0xFBFFFFFF;
    p_asm->setConsole(ui->textEdit_Msg);

    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x78 , (u16 *) &AsmData->rw.Jitter_Clean[6]),"Mode Calibration VCO CDCE62005");
    p_asm->Message(p_asm->ConfigCdce62005(m_mask,getFrontEnd(m_chan),6,m_chan,false),"Calibration 0 VCO CDCE62005");

    AsmData->rw.Jitter_Clean[6] |= 0x80000000;
    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x78 , (u16 *) &AsmData->rw.Jitter_Clean[6]),"");
    p_asm->Message(p_asm->ConfigCdce62005(m_mask,getFrontEnd(m_chan),6,m_chan,false),"Calibration 1 VCO CDCE62005");

    AsmData->rw.Jitter_Clean[6] |= 0x84000000;
    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x78 , (u16 *) &AsmData->rw.Jitter_Clean[6]),"");
    p_asm->Message(p_asm->ConfigCdce62005(m_mask,getFrontEnd(m_chan),6,m_chan,false),"Calibration 2 VCO CDCE62005");

    AsmData->rw.Jitter_Clean[6] &= 0x7fffffff;
    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x78 , (u16 *) &AsmData->rw.Jitter_Clean[6]),"");
    p_asm->Message(p_asm->ConfigCdce62005(m_mask,getFrontEnd(m_chan),6,m_chan,false),"End Calibration VCO CDCE62005");

    decodeStatus();
}



void Dial_calib::on_CopyToEeprom_clicked()
//===============================================
//===============================================
{
    ui->textEdit_Msg->append(QString::number(m_chan,10));
    int ret;
    p_asm->Message(ret = p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),COPYEECDCE),"Operation Init Cdece62005 Successfull ","Operation Init Cdece62005 failed");
    decodeStatus();
}


void Dial_calib::on_SelectClock_clicked()
//===============================================
// Power Down CDCE62005 and power Up
//===============================================
{

    ui->textEdit_Msg->append(QString::number(m_chan,10));
    int ret;
    p_asm->Message(ret = p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),CYCLEPOWERCDCE),"Operation Power off/on Cdece62005 Successfull ","Operation Power off/on Cdece62005 failed");
    decodeStatus();

}

void Dial_calib::on_ShiftClockPllFpga_activated(int index)
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.ShiftPllAdc &= 0x8000;
    AsmData->rw.ShiftPllAdc |= index;
    ui->textEdit_Msg->append("Operation Successfull 0x" + QString::number(AsmData->rw.ShiftPllAdc,16));
    p_asm->Message(p_asm->WriteCmd(m_mask, getFrontEnd(m_chan), 2, 0x90 , (u16 *) &AsmData->rw.ShiftPllAdc),"Select Clock " + QString::number(AsmData->rw.ShiftPllAdc & 0x7,16));
    decodeStatus();
}


void Dial_calib::on_DWriteDenable_currentIndexChanged(int index)
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.RegSpare2 &= 0x3ffF;
    AsmData->rw.RegSpare2 |= (u16)(index <<14);
    ui->textEdit_Msg->append("Operation Successfull 0x" + QString::number(AsmData->rw.RegSpare2,16) + " index " +QString::number(index));
    if (m_send) {
        p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x92 , (u16 *) &AsmData->rw.RegSpare2),"Select Dwrite Denable " + QString::number(AsmData->rw.RegSpare2,16));
        decodeStatus();
    }
}

void Dial_calib::on_AndOrQuartet_activated(int index)
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.Mode &= 0xfffE;
    AsmData->rw.Mode |= index;
    ui->textEdit_Msg->append("Operation Successfull 0x" + QString::number(AsmData->rw.Mode,16) + " index " +QString::number(index));
    p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x21 , (u16 *) &AsmData->rw.Mode),"Select Mode Daq AND-OR " + QString::number(AsmData->rw.Mode,16));
    decodeStatus();
}

void Dial_calib::on_ModeDaq_activated(int index)
//========================================9=======
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.Mode &= 0xfff1;
    AsmData->rw.Mode |= index<<1;
    ui->textEdit_Msg->append("Operation Successfull Select Mode Daq 0x" + QString::number(AsmData->rw.Mode,16) + " index " +QString::number(index));
    p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x21 , (u16 *) &AsmData->rw.Mode),"Select Mode Daq " + QString::number(AsmData->rw.Mode,16));
    if (index == 7) {
        ui->FreqPied->setEnabled(true);
        ui->Label_freqPied->setEnabled(true);
    } else {
        ui->FreqPied->setEnabled(false);
        ui->Label_freqPied->setEnabled(false);
    }
    decodeStatus();
}

void Dial_calib::on_FreqPied_editingFinished()
//===============================================
//===============================================
{
    ASMDATA *AsmData = p_asm->GetData(m_use);
    AsmData->rw.FreqPied = ui->FreqPied->value();
    ui->textEdit_Msg->append("Operation Successfull Frequency 0x" + QString::number(ui->FreqPied->value(),16));
    p_asm->Message(p_asm->WriteCmd(m_mask, m_chan, 2, 0x91 , (u16 *) &AsmData->rw.FreqPied),"Select Mode Daq " + QString::number(AsmData->rw.FreqPied,16));
    decodeStatus();
}


void Dial_calib::ReadFrequency()
//===============================================
//===============================================
{
    int ret;
    u16 reg[9];
    p_asm->Message(ret=p_asm->ReadCmd(m_mask, getFrontEnd(m_chan), 9, 0x16, &reg[0]),"Read Value 0x" + QString::number(reg[0],16));
    long int freq  = (long int) (reg[0]) << 32 |reg[1] << 16 | reg[2];
    long int freq1 = (long int) (reg[3]) << 32 |reg[4] << 16 | reg[5];
    long int freq2 = (long int) (reg[6]) << 32 |reg[7] << 16 | reg[8];

    double val  = (double) freq  / 10.0;
    double val1 = (double) freq1 / 10.0;
    double val2 = (double) freq2 / 10.0;

    ui->ValFreq->display((val <10 ? "0" : "") + QString().setNum(val, 'f', 2)); //display((double)(freq/10));
    if (fabs(val - 30.0) > 0.5) ui->ValFreq->setPalette(Qt::red);
    else ui->ValFreq->setPalette(Qt::green);

    ui->ValFreq_2->display((val1 <10 ? "0" : "") + QString().setNum(val1, 'f', 2)); //display((double)(freq/10));
    if (fabs(val - 30.0) > 0.5) ui->ValFreq_2->setPalette(Qt::red);
    else ui->ValFreq_2->setPalette(Qt::green);

    ui->ValFreq_3->display((val2 <10 ? "0" : "") + QString().setNum(val2, 'f', 2)); //display((double)(freq/10));
    if (fabs(val - 30.0) > 0.5) ui->ValFreq_3->setPalette(Qt::red);
    else ui->ValFreq_3->setPalette(Qt::green);


}

void Dial_calib::on_CalculFreq_clicked()
//===============================================
//===============================================
{
    int ret;
    ui->CalculFreqBar->show();
    ui->CalculFreqBar->setRange(0,100);
    ui->CalculFreqBar->setValue(0);
    p_asm->Message(ret=p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),Spare3),"Reset frequency"); //Resert counter Frequency
    ui->CalculFreqBar->setValue(25);
    ReadFrequency();
    ui->CalculFreqBar->setValue(50);
    p_asm->Message(ret=p_asm->CalibCmd(m_mask,getFrontEnd(m_chan),Spare2),"Init Calcul frequency"); //Init counter Frequency
    ui->CalculFreqBar->setValue(75);
    ReadFrequency();
    ui->CalculFreqBar->setValue(100);
    if (m_send) decodeStatus();
    ui->CalculFreqBar->hide();
}


void Dial_calib::on_StartDaq_clicked()
//===============================================
//===============================================
{

    Run1sec();
    decodeStatus();
}

void Dial_calib::on_Monitoring_clicked()
//===============================================
//===============================================
{
 //   Monitor= new monitoring(NULL) ;
  //  dl.setModal(true);
 //   Monitor->setWindowModality(Qt::WindowModal);
 //   Monitor->show();
 /*   lMyPlotsQwt.clear();

    // Opening SHM
    //===============================================
        ShdMem = new ShmRingBuffer<SharedMemory>(CAPACITY,false,SHM_ASM_DATA);
    //      ShdNet = new ShmRingBuffer<sStatFrame>(CAPACITY,true,SHM_NETWORK);
    //===============================================
    QMainWindow *DisplayAsm = new QMainWindow();
    DisplayAsm->setWindowTitle("Asm" + QString::number(ui->FrontEnd->value()));
    QWidget *wid = new QWidget();
    QGridLayout* LayoutVerticalPage1 = new QGridLayout;


    for (int i=0;i<24;i++) {
       lMyPlotsQwt << new MyPlotsQwt("Ch" + QString::number(i),false);
       LayoutVerticalPage1->addWidget(lMyPlotsQwt.back(),i/4,i%4);
    }

    wid->setLayout(LayoutVerticalPage1);
    DisplayAsm->setCentralWidget(wid);
    DisplayAsm->show();

    //=== Display Regular interval Asm datas
    QTimer *timer = new QTimer();
    timer->connect(timer, SIGNAL(timeout()),this,SLOT(ReadShmData()));
    timer->start(1000); //in ms
    */
}


void Dial_calib::MeanStDev(QVector<double> Y,double *Mean,double *Stdev)
{
    double mean = std::accumulate(std::begin(Y), std::end(Y), 0)/static_cast<double>(Y.size());
    double sq_sum = std::inner_product(Y.begin(), Y.end(), Y.begin(), 0.0);
    *Stdev = std::sqrt(sq_sum / Y.size() - mean * mean);
    *Mean = mean;
}

void Dial_calib::ReadShmData(unsigned int NoBoard)
//===============================================
//===============================================
{
 /*  struct S_HeaderFrame *Header;
   uint16_t *buf;

   while (ShdMem->begin() != ShdMem->end()) {
 //       qInfo() << "Traitement Fragment";
        SharedMemory Packet = ShdMem->dump_front();
        SetPacket((uint16_t *) &Packet.raw[0],8000); //attention au calcul
        Header = (struct S_HeaderFrame *) &Packet.raw[0];

        int FeID = GetFeId(); //((ntohs(Header->FeIdK30) & 0x7f00) >> 8) - 0x10;
        if (FeID >= 0x10) FeID -= 0x10;
        if (FeID != NoBoard) break;

//		unsigned long  trigthor = (ntohs(Header->CptTriggerThorMsb) << 16) + ntohs(Header->CptTriggerThorLsb);

//		QString str = QString::number(trigthor,10);
//        QString str = QString::number(GetCptTriggerThor(),10);
 //       ui->TriggerThor->display(str);
//		qInfo() << "trig= " << trigthor << " " << hex << ntohs(Header->CptTriggerThorMsb) << " " << ntohs(Header->CptTriggerThorLsb) << dec;

        CptTrame++;

//        ui->CptTrame->display(CptTrame);
        unsigned short nbSamples = GetNbSamples(); //ntohs(Header->NbSample);

//        qInfo() << "Frond Id = " << hex << FeID + 0x10;
//        qInfo() << "NbSamples = " << nbSamples << " len = " << (nbSamples+2)*2;
        QVector <double> y;

        for (int j=0;j<4;++j) {
           y.clear();
           buf = GetChannel(j);
           unsigned short Ch = GetCh();
           for (int k=0;k<nbSamples;k++) {
              y.push_back((double) (ntohs(buf[2+k]) & 0xfff));

           }
           double meantemp,stdevtemp;
           MeanStDev(y,&meantemp,&stdevtemp);
           Mean[Ch] = meantemp;
           Stdev[Ch] = stdevtemp;
        }
    }
*/
}


void Dial_calib::on_Autocalib_clicked()
{
    CalibAuto();
}
