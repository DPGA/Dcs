//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	MyDefs.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef MYDEFS_H
#define MYDEFS_H
//================================================
//================================================
typedef unsigned char        u8;
typedef unsigned short       u16;
typedef unsigned int         u32;
typedef unsigned long long   u64;
//================================================
//================================================
enum {
    QUIET,
    VERBOSE,
    VERYVERBOSE,
    MAXVERBOSE
};
#define TIMERDFLT            (5000)
//================================================
//================================================
#define PRJID			(0X44504741)	// DPGA
#define FIRMREVMAJOR 	(1)				// Firmware Revision
#define FIRMREVMINOR 	(2)				// Firmware Revision
#define HARDREVMAJOR 	(1)				// Hardware Revision
#define HARDREVMINOR 	(1)				// Hardware Revision
#define SN				(1)				// Serial Number

//================================================
//================================================
#define NB_ASM               (12)
#define NB_CHAN              (NB_ASM + 1)
#define THOR_CHAN            (NB_ASM)

//================================================
//================================================
#define K28_0			(0X1C)				// Acknowledge Frame
#define K28_1			(0X3C)				// Write Register Command Frame
#define K28_2			(0X5C)				// Read Register Command Frame
#define K28_3			(0X7C)				// Special Command Frame
#define K28_4			(0X9C)				// Monitoring Frame
#define K28_5			(0XBC)				// Synchronization Word
#define K28_6			(0XDC)				// IDLE Word
#define K28_7			(0XFC)				// PreTrigger Frame
#define K23_7			(0XF7)				// Trigger Frame
#define K27_7			(0XFB)
#define K29_7			(0XFD)				// Data Sub-frame
#define K30_7			(0XFE)				// Data Frame

#define BROADCAST_FE	(0X7F)				// Broadcast Front End Number

//================================================
//================================================
#define CLEAR			(0)					// Acknowledge Clear
enum ACKNOWLEDGE {
    PARITYERR,								// Acknowledge Parity Error
    LENERR,									// Acknowledge Length Error
    CRCERR,									// Acknowledge CRC Error
    ENDERR,									// Acknowledge End Frame Error
    STARTERR,								// Acknowledge Start Frame Error
    FENOERR,								// Acknowledge FrontEnd Identifier Error
    EPCSEERR,								// Acknowledge EPCS Program  Error
    PARITY,									// Acknowledge Parity Bit
    KEYERR=0x41,                                 // Error protect Key Erase
    MAX_ACK
};
//================================================
//================================================

#define ARGIN32BITS(A)			(A << 1)
//================================================
//================================================
#define CMDRST			(0x0012)			// "0000_0000_0001_0010" : Special Command System Reset
#define CMDRSTCNT		(0x0011)			// "0000_0000_0001_0001" : Special Command Counter Reset
#define CMDSTARTRUN		(0x0009)			// "0000_0000_0000_1001" : Special Command Start Run
#define CMDSTOPRUN		(0x000A)			// "0000_0000_0000_1010" : Special Command Stop Run
#define CMDCONFIG		(0x0005)			// "0000_0000_0000_0101" : Special Command Dynamic FPGA Configuration
#define CMDVETO			(0x0006)			// "0000_0000_0000_0110" : Special Command Veto
#define CMDALIVE		(0x0022)			// "0000_0000_0010_0010" : Special Command Alive Check
#define CMDCHANGE		(0x0024)			// "0000_0000_0010_0100" : Special Command Changement Strobe
#define CMDERASEEPCS	(0x0040)			// "0000_0000_0100_0000" : Special Command Erase EPCS Memory
#define CMDERASEEPCSCONF (0xA55A)			// "0000_0000_0100_0000" : Special Command Erase EPCS Memory Confirmation

//================================================
//================================================
#define UDP_10GBE_MAC_SRC_AD_0		(0xaa)
#define UDP_10GBE_MAC_SRC_AD_1		(0xbc)
#define UDP_10GBE_MAC_SRC_AD_2		(0xcd)
#define UDP_10GBE_MAC_SRC_AD_3 		(0xde)
#define UDP_10GBE_MAC_SRC_AD_4 		(0xef)
#define UDP_10GBE_MAC_SRC_AD_5 		(0xfa)

#define UDP_10GBE_MAC_DST_AD_0		(0x14)
#define UDP_10GBE_MAC_DST_AD_1		(0x18)
#define UDP_10GBE_MAC_DST_AD_2		(0x77)
#define UDP_10GBE_MAC_DST_AD_3 		(0x46)
#define UDP_10GBE_MAC_DST_AD_4 		(0x34)
#define UDP_10GBE_MAC_DST_AD_5 		(0xc6)

#define UDP_10GBE_IP_SRC_AD_O		(192)
#define UDP_10GBE_IP_SRC_AD_1		(168)
#define UDP_10GBE_IP_SRC_AD_2		(2)
#define UDP_10GBE_IP_SRC_AD_3		(50)

#define UDP_10GBE_IP_DST_AD_O		(192)
#define UDP_10GBE_IP_DST_AD_1		(168)
#define UDP_10GBE_IP_DST_AD_2		(2)
#define UDP_10GBE_IP_DST_AD_3		(1)

#define UDP_10GBE_SRC_PORT			(2016)
#define UDP_10GBE_DST_PORT			(2016)

//================================================
//================================================
#define CDCE_POWER_DOWN			(0x0)		// Power_Down_N[2..0]
#define CDCE_SYNC				(0x0)		// Sync_N[2..0]
#define CDCE_CLOCK				(0x3)		// Clock_Sel[2..0]
#define CDCE_CONFIG				((CDCE_CLOCK << 8) | (CDCE_SYNC << 4) | CDCE_POWER_DOWN)

#define NB_CDCE_REG             (9)
#define NB_CDCE                 (3)

//================================================
//================================================
#define EPCS1				(0x10)
#define EPCS4				(0x12)
#define EPCS16				(0x18)
#define EPCS64				(0x16)

//=========================================================================
//=========================================================================

//================================================
//================================================

#endif // MYDEFS_H
