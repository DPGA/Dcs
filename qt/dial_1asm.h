#ifndef DIAL_1ASM_H
#define DIAL_1ASM_H

#include <Asm.h>
#include <QDialog>
#include "ipc.h"
#include "HvClient.h"

namespace Ui {
class dial_1Asm;
}

class dial_1Asm : public QDialog, public ipc
{
    Q_OBJECT

public:
    explicit dial_1Asm(CAsm *asmm, CHvClient *hv, u32 use, u16 *frontend, QString path, QWidget *parent = 0);
    ~dial_1Asm();

private slots:
    void on_AsmModeDaq_activated(int index);

    void on_AsmchoiceLogic_clicked();

    void on_AsmdelayTrig_editingFinished();

    void on_AsmFreqPied_editingFinished();

    void on_AsmConfig_clicked();

    void on_AsmStart_clicked();

    void on_AsmStop_clicked();

    void on_AsmCalib_clicked();

    void on_AsmFreqPied_valueChanged(int arg1);

    void on_MonitoringValue_editingFinished();

    void on_SaveFile_clicked();

    void on_SaveTest_clicked();

    void on_Hv_clicked();

    void on_NbSamples_editingFinished();

    void on_PhaseDrsAdc_activated(int index);

    void on_cfgRegDrs_editingFinished();

    void on_ResetDrs_clicked();

private:
    Ui::dial_1Asm *ui;
    void decodeStatus();
    void setTipPllStat ();
    void writerXml(QString dir);
    void Refresh();

    CAsm *p_asm;
    u32 m_mask;
    u32 m_chan;
    u32 m_use;
    u16 *p_fe;
    CHvClient *p_hv;
    u16 ModeDaq;
    QString m_filename;
    QString Path;
    const uint16_t t_phase[32] = {0x0,0x1,0x2,0x3,0x8,0x9,0xa,0xb,0x10,0x11,0x12,0x13,0x18,0x19,0x1a,0x1b,0x20,0x21,0x22,0x23,0x28,0x29,0x2a,0x2b,
                          0x30,0x31,0x32,0x33,0x38,0x39,0x3a,0x3b};


};

#endif // DIAL_1ASM_H
