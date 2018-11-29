#ifndef DIAL_CALIB_H
#define DIAL_CALIB_H

#include <QDialog>
#include "Asm.h"
#include "Amc.h"
#include "Thor.h"
//#include "shdmem.h"
//#include "ringshm.h"
//#include "decodeframe.h"
//#include "monitoring.h"

namespace Ui {
class Dial_calib;
}


class Dial_calib : public QDialog //public DecodeFrame
{
   Q_OBJECT

public:
    explicit Dial_calib(CAmc *amc, CAsm* asmm, CThor *thor, u32 use, u16 *frontend, u32 usemask, QWidget *parent = 0);
    ~Dial_calib();

private slots:
	void on_FrontEnd_editingFinished();
	void on_StartCalib_clicked();
    void on_StepClk_clicked();

    void on_StopCalib_clicked();

    void on_PhaseAdc_clicked();

    void on_PhaseDrsAdc_activated(int index);


    void on_CalibVco_clicked();

    void on_CopyToEeprom_clicked();

//    void on_ActiveClock_clicked();

    void on_SelectClock_clicked();

    void on_ShiftClockPllFpga_activated(int index);

    void on_DWriteDenable_currentIndexChanged(int index);

    void on_StartDaq_clicked();

    void on_Monitoring_clicked();

    void ReadShmData(unsigned int NoBoard);


    void on_AndOrQuartet_activated(int index);

    void on_ModeDaq_activated(int index);

    void on_FreqPied_editingFinished();

    void on_CalculFreq_clicked();
    void ReadFrequency();

    void on_Autocalib_clicked();

private:
    Ui::Dial_calib *ui;
    void setTipPllStat    ();
    void Message(int ret, QString message, const QString endmessage="");
    void MeanStDev(QVector<double> Y,double *Mean,double *Stdev);
    void decodeStatus();
    void CalibAuto();
    void Error();
    bool m_useThor;
    bool m_useAmc;

    CAsm *p_asm;
    CAmc *p_amc;
    CThor *p_thor;
    u32 m_mask;
    u32 m_chan;
    u32 m_use;
    u16 *p_fe;
    u16 Pattern;
    QList <QCheckBox *> Ch;
    void setup();
    u16 getFrontEnd(u32 chan);
    void Run1sec();
    const uint16_t t_phase[50] = {0x0,0x1,0x2,0x3,0x4,0x8,0x9,0xa,0xb,0xc,0x10,0x11,0x12,0x13,0x14,0x18,0x19,0x1a,0x1b,0x1c,0x20,0x21,0x22,0x23,0x24,
                          0x40,0x41,0x42,0x43,0x44,0x48,0x49,0x4a,0x4b,0x4c,0x50,0x51,0x52,0x53,0x54,0x58,0x59,0x5a,0x5b,0x5c,0x60,0x61,0x62,0x63,0x64};
  //  ShmRingBuffer<SharedMemory> *ShdMem;
    double CptTrame = 0;
    QList <class MyPlotsQwt *> lMyPlotsQwt;
    u16 ModeDaq;
    u32 m_usemask;
    bool m_send;
 //   monitoring *Monitor;
    double Mean[24];
    double Stdev[24];
    int nbstep;
};

#endif // DIAL_CALIB_H
