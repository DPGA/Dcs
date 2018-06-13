//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	AmcData.h
//	Author		:	Bob
//	Creation		:	0è-Mar-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================

#ifndef AMCDATA_H_
#define AMCDATA_H_
//-----------------------------------------------
//-----------------------------------------------
#include "MyDefs.h"
//-----------------------------------------------
//-----------------------------------------------
typedef union _MACAD
{
	u8 m[8];
	u64  ll;
	struct
	{
		u32 lsb;
		u32 msb;
	};
}MACAD;
//-------------------------
typedef struct UDP UDP;
struct UDP
{
	MACAD  Src_Mac_Addr;			// Source MAC Address
	MACAD  Dst_Mac_Addr;			// Destination MAC Address
	u32    Src_Ip_Addr;			// Source IP Address
	u32    Dst_Ip_Addr;			// Destination IP Address
	union {
		u32  ulPort;
		struct {
			u16 Src;					// Source Port
			u16 Dst;					// Destination Port
		};
	} Port;
}__attribute__((__packed__));
//-----------------------------------------------
//-----------------------------------------------
//#define NB_CDCE_REG (9)
//#define NB_CDCE     (3)
typedef struct CDCE CDCE;

struct CDCE
{
	struct _CI
	{
		u32 reg[NB_CDCE_REG];	// CDCE62005 Configuration
	} Chip[NB_CDCE];
	u32 status;						// CDCE Status
	u32 cnfg;						// CDCE Configuration
}__attribute__((__packed__));

//-----------------------------------------------
//-----------------------------------------------
typedef union _CNFG
{
	u32 global;
	struct {
		u32 mode:8;
		u32 nbsamples:11;
		u32 daq:1;
		u32 leds:3;
	};
}CNFG;
//-----------------------------------------------
//-----------------------------------------------
typedef union _STAT
{
	u32 global;
	struct {
		u32 UpdStat:12;
		u32 AsmReconfig:2;
		u32 AsmPllLock:1;
		u32 AsmSynchred:12;
		u32 ThorReconfig:1;
		u32 ThorPllLock:1;
		u32 ThorSynchred:1;
		u32 Reserved:1;
		u32 DaqPause:1;
	};
} STAT;
//-----------------------------------------------
//-----------------------------------------------
#define NB_UDP 		(2)		// Number of 10Gbe UDP Link

typedef struct AMCDATA AMCDATA;
struct AMCDATA
{
	u32 prjid;						// Project Identifier
	u32 firmrev;					// Firmware Revision
	u32 hardrev;					// Hardware Revision
	u32 sn;							// Serial Number
	u32 sysid;						// Nios Project Identifier
	u32 sysstamp;					// Nios System Time Stamp
	u8 btime[12];					// Build Time
	u8 bdate[12];					// Build Date

	u32 UseMask;					// Used Channel

	CNFG cnfg;						// Global Configuration
	STAT status;					// Global Status
	u32 signal;						// Signal Detected

	CDCE	cdce;						// CDCE62005 Parameters
	UDP	udp[NB_UDP];			// UDP parameters

	u64 PrTrigCount;				// Pre_Trigger Counter
	u64 TrigCount;					// Trigger Counter

    u64 FrmCount[12];					// Frames Counters

	u16 local_temperature;		// Chip Temperature
	u16 fpga_temperature;		// FPGA Temperature

	u32 jtaguart_verbose;		// Jtag Uart Verbose Level

}__attribute__((__packed__));

//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
#endif /* AMCDATA_H_ */
