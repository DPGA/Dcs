//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_finder.cpp
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
//==========================================================
//==========================================================
#include "dial_finder.h"
#include "ui_dial_finder.h"

#include "MyDefs.h"
#include "command_error.h"
//==========================================================
//==========================================================
//==========================================================
//==========================================================
Dial_finder::Dial_finder(CAsm * asmm, CThor * thor, u16 * fe, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dial_finder)
{
	p_asm = asmm;
	p_thor = thor;
	for(int i=0; i<NB_CHAN; i++) {
		m_frontend[i] = *fe;
		fe++;
	}
	ui->setupUi(this);
	cnfg();
	on_pushButton_Clear_clicked();
	for(int i=0; i<NB_ASM; i++) {
		ock[i]->setToolTip(QString("Connection Status Asm %1").arg(i));
		fli[i]->setToolTip(QString("Front End Configuration File Asm %1").arg(i));
		mli[i]->setToolTip(QString("Dip Switch Asm %1").arg(i));
	}
	ock[THOR_CHAN]->setToolTip(QString("Connectin Status Thor"));
	fli[THOR_CHAN]->setToolTip(QString("Front End Configuration File Thor"));
	mli[THOR_CHAN]->setToolTip(QString("Dip Switch Thor"));
	ui->pushButton_Do->setToolTip(QString("Search Module"));
	ui->pushButton_Clear->setToolTip(QString("Clear Display"));
	ui->pushButton_Close->setToolTip(QString("Exit This Box"));
}

//==========================================================
Dial_finder::~Dial_finder()
{
	delete ui;
}

//==========================================================
//==========================================================
//==========================================================
//==========================================================
void Dial_finder::on_pushButton_Do_clicked()
{
	u32 mask;
	ui->progressBar->setValue(0);
	for(int chan=0; chan<NB_ASM; chan++) {
		fli[chan]->setText(QString("0x%1").arg(m_frontend[chan], 2, 16, QChar('0')));
		mask = 1 << chan;
		p_asmdt = p_asm->GetData(chan);
		if(p_asm->Dump(mask) == NO_ERROR) {
			mli[chan]->setText(QString("0x%1").arg(p_asmdt->rd.AddrConf, 2 , 16,  QChar('0')));
		} else {
			mli[chan]->setText(QString("Nav"));
		}
		if(m_frontend[chan] == p_asmdt->rd.AddrConf) {
			ock[chan]->setCheckState(Qt::Checked);
		} else {
			ock[chan]->setCheckState(Qt::Unchecked);
		}
        if(p_asm->Dump(mask) == NO_ERROR) {
            Firmrev[chan]->setText(QString("%1.%2").arg(p_asmdt->rd.FwRev_Maj).arg(p_asmdt->rd.FwRev_Min,2, 10, QChar('0')));
        } else {
            Firmrev[chan]->setText(QString("Nav"));
        }
        if(p_asm->Dump(mask) == NO_ERROR) {
          for (int i=0;i<3;++i) {
            if(p_asmdt->rd.Stat_GXB & (1 << (i+28))) Calibok[chan][i]->setCheckState(Qt::Checked); else Calibok[chan][i]->setCheckState(Qt::Unchecked);
          }
          Lockpll[chan]->setText(QString("0x%1").arg(p_asmdt->rd.Pll_Lock,4, 16, QChar('0')));
          QString str = QString("<b>Pll Locked</b> 0x%1").arg(p_asmdt->rd.Pll_Lock,4, 16, QChar('0'));
          str.append("<ul>");
          if(p_asmdt->rd.Pll_Lock & (1 << 0)) str.append("<li>Pll Drs 0			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 0        : <font color='red'>not Locked</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 1)) str.append("<li>Pll Drs 1			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 1        : <font color='red'>not Locked</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 2)) str.append("<li>Pll Drs 2			: <font color='green'>Locked</li>");else str.append("<li>Pll Drs 2        : <font color='red'>not Locked</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 3)) str.append("<li>Pll Cdce62005		: <font color='green'>Locked</li>");else str.append("<li>Pll Cdce62005    : <font color='red'>not Locked</font></li>");

          if(p_asmdt->rd.Pll_Lock & (1 << 4)) str.append("<li>Pll 40Mhz to		: <font color='green'>Locked</li>");else str.append("<li>Pll 40Mhz to     : <font color='red'>not Locked</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 5)) str.append("<li>Pll Config (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Config (FPGA): <font color='red'>not Locked</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 6)) str.append("<li>Pll Sample (FPGA): <font color='green'>Locked</li>");else str.append("<li>Pll Sample (FPGA): <font color='red'>not Locked</font></li>");

          if(p_asmdt->rd.Pll_Lock & (1 << 13)) str.append("<li>Bad clock : <font color='red'>F_ClkAdc</li>");else str.append("<li>Good Clock : <font color='green'>f_clkadc</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 14)) str.append("<li>Bad clock : <font color='red'>c_clk</li>");else str.append("<li>Good Clock : <font color='green'>C_Clk</font></li>");
          if(p_asmdt->rd.Pll_Lock & (1 << 15)) str.append("<li>Active clock : <font color='green'>c_clk</li>");else str.append("<li>Active Clock : <font color='red'>f_clkadc</font></li>");
          Lockpll[chan]->setToolTip(str);
        }

		ui->progressBar->setValue(chan+1);
	}
	fli[THOR_CHAN]->setText(QString("0x%1").arg(m_frontend[THOR_CHAN], 2, 16, QChar('0')));
	p_thordt = p_thor->GetData();
	if(p_thor->Dump() == NO_ERROR) {
        Firmrev[THOR_CHAN]->setText(QString("%1.%2").arg(p_thordt->rd.FwRev_Maj).arg(p_thordt->rd.FwRev_Min,2, 10, QChar('0')));
		mli[THOR_CHAN]->setText(QString("0x%1").arg(p_thordt->rd.Addr_Conf, 2, 16, QChar('0')));
	} else {
		mli[THOR_CHAN]->setText(QString("Nav"));      
        Firmrev[THOR_CHAN]->setText(QString("Nav"));
	}
	if(m_frontend[THOR_CHAN] == p_thordt->rd.Addr_Conf) {
		ock[THOR_CHAN]->setCheckState(Qt::Checked);
	} else {
		ock[THOR_CHAN]->setCheckState(Qt::Unchecked);
	}
	ui->progressBar->setValue(THOR_CHAN +1);
}

//==========================================================
//==========================================================
void Dial_finder::on_pushButton_Clear_clicked()
{
	for(int i=0; i<NB_CHAN; i++) {
		ock[i]->setCheckState(Qt::Unchecked);
		fli[i]->clear();
		mli[i]->clear();
	}
	ui->progressBar->setValue(0);
}

//==========================================================
//==========================================================
void Dial_finder::on_pushButton_Close_clicked()
{
   this->close();
}

//==========================================================
//==========================================================
//==========================================================
void Dial_finder::cnfg(void)
{
	ock[0]  =  ui->checkBox_Asm_0;
	ock[1]  =  ui->checkBox_Asm_1;
	ock[2]  =  ui->checkBox_Asm_2;
	ock[3]  =  ui->checkBox_Asm_3;
	ock[4]  =  ui->checkBox_Asm_4;
	ock[5]  =  ui->checkBox_Asm_5;
	ock[6]  =  ui->checkBox_Asm_6;
	ock[7]  =  ui->checkBox_Asm_7;
	ock[8]  =  ui->checkBox_Asm_8;
	ock[9]  =  ui->checkBox_Asm_9;
	ock[10] =  ui->checkBox_Asm_10;
	ock[11] =  ui->checkBox_Asm_11;
	ock[12] =  ui->checkBox_Thor;
	fli[0]  =  ui->lineEdit_FAsm_0;
	fli[1]  =  ui->lineEdit_FAsm_1;
	fli[2]  =  ui->lineEdit_FAsm_2;
	fli[3]  =  ui->lineEdit_FAsm_3;
	fli[4]  =  ui->lineEdit_FAsm_4;
	fli[5]  =  ui->lineEdit_FAsm_5;
	fli[6]  =  ui->lineEdit_FAsm_6;
	fli[7]  =  ui->lineEdit_FAsm_7;
	fli[8]  =  ui->lineEdit_FAsm_8;
	fli[9]  =  ui->lineEdit_FAsm_9;
	fli[10] =  ui->lineEdit_FAsm_10;
	fli[11] =  ui->lineEdit_FAsm_11;
	fli[12] =  ui->lineEdit_FThor;
	mli[0]  =  ui->lineEdit_Asm_0;
	mli[1]  =  ui->lineEdit_Asm_1;
	mli[2]  =  ui->lineEdit_Asm_2;
	mli[3]  =  ui->lineEdit_Asm_3;
	mli[4]  =  ui->lineEdit_Asm_4;
	mli[5]  =  ui->lineEdit_Asm_5;
	mli[6]  =  ui->lineEdit_Asm_6;
	mli[7]  =  ui->lineEdit_Asm_7;
	mli[8]  =  ui->lineEdit_Asm_8;
	mli[9]  =  ui->lineEdit_Asm_9;
	mli[10] =  ui->lineEdit_Asm_10;
	mli[11] =  ui->lineEdit_Asm_11;
	mli[12] =  ui->lineEdit_Thor;

    Lockpll[0]  = ui->lineEdit_Lock_Pll_0;
    Lockpll[1]  = ui->lineEdit_Lock_Pll_1;
    Lockpll[2]  = ui->lineEdit_Lock_Pll_2;
    Lockpll[3]  = ui->lineEdit_Lock_Pll_3;
    Lockpll[4]  = ui->lineEdit_Lock_Pll_4;
    Lockpll[5]  = ui->lineEdit_Lock_Pll_5;
    Lockpll[6]  = ui->lineEdit_Lock_Pll_6;
    Lockpll[7]  = ui->lineEdit_Lock_Pll_7;
    Lockpll[8]  = ui->lineEdit_Lock_Pll_8;
    Lockpll[9]  = ui->lineEdit_Lock_Pll_9;
    Lockpll[10] = ui->lineEdit_Lock_Pll_10;
    Lockpll[11] = ui->lineEdit_Lock_Pll_11;

    Firmrev[0]  = ui->lineEdit_Firm_Rev_0;
    Firmrev[1]  = ui->lineEdit_Firm_Rev_1;
    Firmrev[2]  = ui->lineEdit_Firm_Rev_2;
    Firmrev[3]  = ui->lineEdit_Firm_Rev_3;
    Firmrev[4]  = ui->lineEdit_Firm_Rev_4;
    Firmrev[5]  = ui->lineEdit_Firm_Rev_5;
    Firmrev[6]  = ui->lineEdit_Firm_Rev_6;
    Firmrev[7]  = ui->lineEdit_Firm_Rev_7;
    Firmrev[8]  = ui->lineEdit_Firm_Rev_8;
    Firmrev[9]  = ui->lineEdit_Firm_Rev_9;
    Firmrev[10] = ui->lineEdit_Firm_Rev_10;
    Firmrev[11] = ui->lineEdit_Firm_Rev_11;
    Firmrev[12] = ui->lineEdit_Firm_Rev_Thor;

    Calibok[0][0] = ui->checkBox_Asm_DRS_0_0;
    Calibok[0][1] = ui->checkBox_Asm_DRS_0_1;
    Calibok[0][2] = ui->checkBox_Asm_DRS_0_2;

    Calibok[1][0] = ui->checkBox_Asm_DRS_1_0;
    Calibok[1][1] = ui->checkBox_Asm_DRS_1_1;
    Calibok[1][2] = ui->checkBox_Asm_DRS_1_2;

    Calibok[2][0] = ui->checkBox_Asm_DRS_2_0;
    Calibok[2][1] = ui->checkBox_Asm_DRS_2_1;
    Calibok[2][2] = ui->checkBox_Asm_DRS_2_2;

    Calibok[3][0] = ui->checkBox_Asm_DRS_3_0;
    Calibok[3][1] = ui->checkBox_Asm_DRS_3_1;
    Calibok[3][2] = ui->checkBox_Asm_DRS_3_2;

    Calibok[4][0] = ui->checkBox_Asm_DRS_4_0;
    Calibok[4][1] = ui->checkBox_Asm_DRS_4_1;
    Calibok[4][2] = ui->checkBox_Asm_DRS_4_2;

    Calibok[5][0] = ui->checkBox_Asm_DRS_5_0;
    Calibok[5][1] = ui->checkBox_Asm_DRS_5_1;
    Calibok[5][2] = ui->checkBox_Asm_DRS_5_2;

    Calibok[6][0] = ui->checkBox_Asm_DRS_6_0;
    Calibok[6][1] = ui->checkBox_Asm_DRS_6_1;
    Calibok[6][2] = ui->checkBox_Asm_DRS_6_2;

    Calibok[7][0] = ui->checkBox_Asm_DRS_7_0;
    Calibok[7][1] = ui->checkBox_Asm_DRS_7_1;
    Calibok[7][2] = ui->checkBox_Asm_DRS_7_2;

    Calibok[8][0] = ui->checkBox_Asm_DRS_8_0;
    Calibok[8][1] = ui->checkBox_Asm_DRS_8_1;
    Calibok[8][2] = ui->checkBox_Asm_DRS_8_2;

    Calibok[9][0] = ui->checkBox_Asm_DRS_9_0;
    Calibok[9][1] = ui->checkBox_Asm_DRS_9_1;
    Calibok[9][2] = ui->checkBox_Asm_DRS_9_2;

    Calibok[10][0] = ui->checkBox_Asm_DRS_10_0;
    Calibok[10][1] = ui->checkBox_Asm_DRS_10_1;
    Calibok[10][2] = ui->checkBox_Asm_DRS_10_2;

    Calibok[11][0] = ui->checkBox_Asm_DRS_11_0;
    Calibok[11][1] = ui->checkBox_Asm_DRS_11_1;
    Calibok[11][2] = ui->checkBox_Asm_DRS_11_2;
}
//==========================================================
//==========================================================
//==========================================================
//==========================================================
//==========================================================
