//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	dial_amcreg.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef DIAL_AMCREG_H
#define DIAL_AMCREG_H

#include <QDialog>
#include "MyDefs.h"
#include "Amc.h"
#include "Nios_system.h"

//===============================================
#define NB_AUTHORIZATION (97)
//===============================================
namespace Ui {
class Dial_AmcReg;
}
//===============================================
typedef struct _REGNAM _REGNAM;
struct _REGNAM {
	QString nam;
	u32     addr;
	bool    wr;
};

//===============================================
//===============================================
class Dial_AmcReg : public QDialog
{
	Q_OBJECT

public:
	explicit Dial_AmcReg(CAmc *amc, QWidget *parent = 0);
	~Dial_AmcReg();

private slots:
	void on_checkBox_AmcReg_clicked();

	void on_pushButton_AmcReg_Read_clicked();

	void on_pushButton_AmcReg_Write_clicked();

	void on_pushButton_AmcReg_Cancel_clicked();

	void on_listWidget_itemSelectionChanged();

private:
	Ui::Dial_AmcReg *ui;
	CAmc * p_amc;
	u32 m_addr;
	u32 m_rval;
	u32 m_wval;
	int m_base;

	void display(void);

	const _REGNAM reglist[NB_AUTHORIZATION] = {
		{ "JTAG_UART",                JTAG_UART_BASE,                false },   // 0x8200100
		{ "SYSTIMER",                 SYSTIMER_BASE,                 false },   // 0x8200300
		{ "HIGHRESTIMER",             HIGHRESTIMER_BASE,             false },   // 0x8200400
		{ "LED_PIO",                  LED_PIO_BASE,                  true  },   // 0x8200500
		{ "TX_DATA",                  TX_DATA_BASE,                  false },   // 0x8200510
		{ "RX_DATA",                  RX_DATA_BASE,                  false },   // 0x8200520
		{ "TXRX_CNFG",                TXRX_CNFG_BASE,                false },   // 0x8200530
		{ "TXRX_STAT",                TXRX_STAT_BASE,                false },   // 0x8200540
		{ "SGDMA_RX",                 SGDMA_RX_BASE,                 false },   // 0x8200600
		{ "SGDMA_TX",                 SGDMA_TX_BASE,                 false },   // 0x8200700
		{ "TSE_MAC",                  TSE_MAC_BASE,                  false },   // 0x8201000

		{ "CDCE_CNFG",                CDCE_CNFG_BASE,                true  },   // 0x8300010
		{ "CDCE_STAT",                CDCE_STAT_BASE,                true  },   // 0x8300020
		{ "GBE_STAT",                 GBE_STAT_BASE,                 false },   // 0x8300030
		{ "MAC_DEST_ADDR_MSB_0",      MAC_DEST_ADDR_MSB_0_BASE,      true  },   // 0x8300040
		{ "MAC_DEST_ADDR_LSB_0",      MAC_DEST_ADDR_LSB_0_BASE,      true  },   // 0x8300050
		{ "MAC_SRC_ADDR_MSB_0",       MAC_SRC_ADDR_MSB_0_BASE,       true  },   // 0x8300060
		{ "MAC_SRC_ADDR_LSB_0",       MAC_SRC_ADDR_LSB_0_BASE,       true  },   // 0x8300070
		{ "IP_DEST_ADDR_0",           IP_DEST_ADDR_0_BASE,           true  },   // 0x8300080
		{ "IP_SRC_ADDR_0",            IP_SRC_ADDR_0_BASE,            true  },   // 0x8300090
		{ "UDP_PORT_0",               UDP_PORT_0_BASE,               true  },   // 0x83000a0
		{ "GLOBAL_CNFG",              GLOBAL_CNFG_BASE,              true  },   // 0x83000b0
		{ "GLOBAL_STATUS",            GLOBAL_STATUS_BASE,            true  },   // 0x83000c0
		{ "DCS_START",                DCS_START_BASE,                false },   // 0x83000d0
		{ "DCS_RESET",                DCS_RESET_BASE,                false },   // 0x83000e0
		{ "PRTRIG_MSB",               PRTRIG_MSB_BASE,               false },   // 0x8300100
		{ "PRTRIG_LSB",               PRTRIG_LSB_BASE,               false },   // 0x8300110
		{ "TRIG_MSB",                 TRIG_MSB_BASE,                 false },   // 0x8300120
		{ "TRIG_LSB",                 TRIG_LSB_BASE,                 false },   // 0x8300130
		{ "RESET_CNTRL",              RESET_CNTRL_BASE,              false },   // 0x8300140
		{ "MAC_DEST_ADDR_MSB_1",      MAC_DEST_ADDR_MSB_1_BASE,      true  },   // 0x8300150
		{ "MAC_DEST_ADDR_LSB_1",      MAC_DEST_ADDR_LSB_1_BASE,      true  },   // 0x8300160
		{ "MAC_SRC_ADDR_MSB_1",       MAC_SRC_ADDR_MSB_1_BASE,       true  },   // 0x8300170
		{ "MAC_SRC_ADDR_LSB_1",       MAC_SRC_ADDR_LSB_1_BASE,       true  },   // 0x8300180
		{ "IP_DEST_ADDR_1",           IP_DEST_ADDR_1_BASE,           true  },   // 0x8300190
		{ "IP_SRC_ADDR_1",            IP_SRC_ADDR_1_BASE,            true  },   // 0x83001a0
		{ "UDP_PORT_1",               UDP_PORT_1_BASE,               true  },   // 0x83001b0

		{ "BOB_I2C_0",                BOB_I2C_0_BASE,                false },   // 0x8400000
		{ "BOB_I2C_1",                BOB_I2C_1_BASE,                false },   // 0x8400010

		{ "FIFO_TO_ASM_0_IN",         FIFO_TO_ASM_0_IN_BASE,         false },   // 0x8500000
		{ "FIFO_TO_ASM_1_IN",         FIFO_TO_ASM_1_IN_BASE,         false },   // 0x8500008
		{ "FIFO_TO_ASM_2_IN",         FIFO_TO_ASM_2_IN_BASE,         false },   // 0x8500010
		{ "FIFO_TO_ASM_3_IN",         FIFO_TO_ASM_3_IN_BASE,         false },   // 0x8500018
		{ "FIFO_TO_ASM_4_IN",         FIFO_TO_ASM_4_IN_BASE,         false },   // 0x8500020
		{ "FIFO_TO_ASM_5_IN",         FIFO_TO_ASM_5_IN_BASE,         false },   // 0x8500028
		{ "FIFO_TO_ASM_6_IN",         FIFO_TO_ASM_6_IN_BASE,         false },   // 0x8500030
		{ "FIFO_TO_ASM_7_IN",         FIFO_TO_ASM_7_IN_BASE,         false },   // 0x8500038
		{ "FIFO_TO_ASM_8_IN",         FIFO_TO_ASM_8_IN_BASE,         false },   // 0x8500040
		{ "FIFO_TO_ASM_9_IN",         FIFO_TO_ASM_9_IN_BASE,         false },   // 0x8500048
		{ "FIFO_TO_ASM_10_IN",        FIFO_TO_ASM_10_IN_BASE,        false },   // 0x8500050
		{ "FIFO_TO_ASM_11_IN",        FIFO_TO_ASM_11_IN_BASE,        false },   // 0x8500058
		{ "FIFO_FROM_ASM_0_OUT",      FIFO_FROM_ASM_0_OUT_BASE,      false },   // 0x8500068
		{ "FIFO_FROM_ASM_1_OUT",      FIFO_FROM_ASM_1_OUT_BASE,      false },   // 0x8500070
		{ "FIFO_FROM_ASM_2_OUT",      FIFO_FROM_ASM_2_OUT_BASE,      false },   // 0x8500078
		{ "FIFO_FROM_ASM_3_OUT",      FIFO_FROM_ASM_3_OUT_BASE,      false },   // 0x8500080
		{ "FIFO_FROM_ASM_4_OUT",      FIFO_FROM_ASM_4_OUT_BASE,      false },   // 0x8500088
		{ "FIFO_FROM_ASM_5_OUT",      FIFO_FROM_ASM_5_OUT_BASE,      false },   // 0x8500090
		{ "FIFO_FROM_ASM_6_OUT",      FIFO_FROM_ASM_6_OUT_BASE,      false },   // 0x8500098
		{ "FIFO_FROM_ASM_7_OUT",      FIFO_FROM_ASM_7_OUT_BASE,      false },   // 0x85000a0
		{ "FIFO_FROM_ASM_8_OUT",      FIFO_FROM_ASM_8_OUT_BASE,      false },   // 0x85000a8
		{ "FIFO_FROM_ASM_9_OUT",      FIFO_FROM_ASM_9_OUT_BASE,      false },   // 0x85000b0
		{ "FIFO_FROM_ASM_10_OUT",     FIFO_FROM_ASM_10_OUT_BASE,     false },   // 0x85000b8
		{ "FIFO_FROM_ASM_11_OUT",     FIFO_FROM_ASM_11_OUT_BASE,     false },   // 0x85000c0
		{ "FIFO_TO_ASM_0_IN_CSR",     FIFO_TO_ASM_0_IN_CSR_BASE,     false },   // 0x8500100
		{ "FIFO_TO_ASM_1_IN_CSR",     FIFO_TO_ASM_1_IN_CSR_BASE,     false },   // 0x8500120
		{ "FIFO_TO_ASM_2_IN_CSR",     FIFO_TO_ASM_2_IN_CSR_BASE,     false },   // 0x8500140
		{ "FIFO_TO_ASM_3_IN_CSR",     FIFO_TO_ASM_3_IN_CSR_BASE,     false },   // 0x8500160
		{ "FIFO_TO_ASM_4_IN_CSR",     FIFO_TO_ASM_4_IN_CSR_BASE,     false },   // 0x8500180
		{ "FIFO_TO_ASM_5_IN_CSR",     FIFO_TO_ASM_5_IN_CSR_BASE,     false },   // 0x85001a0
		{ "FIFO_TO_ASM_6_IN_CSR",     FIFO_TO_ASM_6_IN_CSR_BASE,     false },   // 0x85001c0
		{ "FIFO_TO_ASM_7_IN_CSR",     FIFO_TO_ASM_7_IN_CSR_BASE,     false },   // 0x85001e0
		{ "FIFO_TO_ASM_8_IN_CSR",     FIFO_TO_ASM_8_IN_CSR_BASE,     false },   // 0x8500200
		{ "FIFO_TO_ASM_9_IN_CSR",     FIFO_TO_ASM_9_IN_CSR_BASE,     false },   // 0x8500220
		{ "FIFO_TO_ASM_10_IN_CSR",    FIFO_TO_ASM_10_IN_CSR_BASE,    false },   // 0x8500240
		{ "FIFO_TO_ASM_11_IN_CSR",    FIFO_TO_ASM_11_IN_CSR_BASE,    false },   // 0x8500260
		{ "FIFO_FROM_ASM_0_OUT_CSR",  FIFO_FROM_ASM_0_OUT_CSR_BASE,  false },   // 0x8500300
		{ "FIFO_FROM_ASM_1_OUT_CSR",  FIFO_FROM_ASM_1_OUT_CSR_BASE,  false },   // 0x8500320
		{ "FIFO_FROM_ASM_2_OUT_CSR",  FIFO_FROM_ASM_2_OUT_CSR_BASE,  false },   // 0x8500340
		{ "FIFO_FROM_ASM_3_OUT_CSR",  FIFO_FROM_ASM_3_OUT_CSR_BASE,  false },   // 0x8500360
		{ "FIFO_FROM_ASM_4_OUT_CSR",  FIFO_FROM_ASM_4_OUT_CSR_BASE,  false },   // 0x8500380
		{ "FIFO_FROM_ASM_5_OUT_CSR",  FIFO_FROM_ASM_5_OUT_CSR_BASE,  false },   // 0x85003a0
		{ "FIFO_FROM_ASM_6_OUT_CSR",  FIFO_FROM_ASM_6_OUT_CSR_BASE,  false },   // 0x85003c0
		{ "FIFO_FROM_ASM_7_OUT_CSR",  FIFO_FROM_ASM_7_OUT_CSR_BASE,  false },   // 0x85003e0
		{ "FIFO_FROM_ASM_8_OUT_CSR",  FIFO_FROM_ASM_8_OUT_CSR_BASE,  false },   // 0x8500400
		{ "FIFO_FROM_ASM_9_OUT_CSR",  FIFO_FROM_ASM_9_OUT_CSR_BASE,  false },   // 0x8500420
		{ "FIFO_FROM_ASM_10_OUT_CSR", FIFO_FROM_ASM_10_OUT_CSR_BASE, false },   // 0x8500440
		{ "FIFO_FROM_ASM_11_OUT_CSR", FIFO_FROM_ASM_11_OUT_CSR_BASE, false },   // 0x8500460
		{ "FIFO_TO_THOR_IN",          FIFO_TO_THOR_IN_BASE,          false },   // 0x8500500
		{ "FIFO_FROM_THOR_OUT",       FIFO_FROM_THOR_OUT_BASE,       false },   // 0x8500508
		{ "FIFO_TO_THOR_IN_CSR",      FIFO_TO_THOR_IN_CSR_BASE,      false },   // 0x8500520
		{ "FIFO_FROM_THOR_OUT_CSR",   FIFO_FROM_THOR_OUT_CSR_BASE,   false },   // 0x8500540
		{ "FIFO_TO_10GBE_IN",         FIFO_TO_10GBE_IN_BASE,         false },   // 0x8500600
		{ "FIFO_FROM_10GBE_OUT",      FIFO_FROM_10GBE_OUT_BASE,      false },   // 0x8500608
		{ "FIFO_TO_10GBE_IN_CSR",     FIFO_TO_10GBE_IN_CSR_BASE,     false },   // 0x8500620
		{ "FIFO_FROM_10GBE_OUT_CSR",  FIFO_FROM_10GBE_OUT_CSR_BASE,  false },   // 0x8500640

		{ "USEMASK",                  0x00000000,                    true },
		{ "Others",                   0x00000001,                    true }
	};
};
//===============================================
//===============================================

#endif // DIAL_AMCREG_H
