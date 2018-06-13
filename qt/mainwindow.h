//===============================================
//	Projet		:	QtDpgaDcs
//	Tittle		:	mainwindow.h
//	Author		:	Bob
//	Creation		:	13-Sept-2016 
// LastUpdate	:
//===============================================
//===============================================

//===============================================
//===============================================
#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QTime>
#include <QProcess>

#include "Tcp.h"
#include "Amc.h"
#include "Asm.h"
#include "Thor.h"
#include "HvClient.h"
#include "monitoring.h"

//===============================================
//===============================================
namespace Ui {
class MainWindow;
}
//===============================================
//===============================================
//enum  {TABAMC, TABASM, TABTHOR, TABHV, TABMISC};

//===============================================
//===============================================
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void SetVerbose (int verbose);

private slots:
	void TimerDisplay(void);

    void on_actionQuit_triggered();
	void on_actionAbout_triggered();

	void on_actionReload_Config_triggered();

	void on_pushButton_MiscTab_clicked();

	void on_tabWidget_currentChanged(int index);

	void on_actionIdentification_triggered();

	void on_pushButton_AmcTab_Do_clicked();

	void on_pushButton_AsmTab_Do_clicked();

	void on_spinBox_AsmTab_Sel_valueChanged(const QString &arg1);

	void on_spinBox_AsmTab_Sel_editingFinished();

	void on__ThorTab_Do_clicked();

	void on_actionReboot_triggered();

	void on_actionReset_triggered();

	void on_actionNios_Verbose_triggered();

	void on_actionVerbose_triggered();

	void on_actionStart_Amc_triggered();

	void on_actionStop_Amc_triggered();

   void on_actionRead_Write_Registers_triggered();

	void on_actionCDCE_Registers_triggered();

	void on_actionConfig_Amc_triggered();

	void on_actionStart_Asm_triggered();

	void on_actionStop_Asm_triggered();

	void on_actionRead_Write_Register_triggered();

	void on_actionUpdate_Change_triggered();

	void on_actionConfig_Asm_triggered();

	void on_actionReset_Fpga_triggered();

	void on_actionStart_Thor_triggered();

	void on_actionStop_Thor_triggered();

	void on_actionRead_Write_Register_2_triggered();

	void on_actionUpdate_Change_2_triggered();

	void on_actionReset_Fpga_2_triggered();

	void on_actionConfig_Thor_triggered();

//	void on_actionStart_All_triggered();

//	void on_actionStop_All_triggered();

	void on_actionProgammation_2_triggered();

	void on_actionProgrammation_triggered();

	void on_checkBox_MisTab_clicked();

	void on_spinBox_MiscTab_editingFinished();

	void on_spinBox_MiscTab_valueChanged(int arg1);

	void on_pushButton_Hv_Refresh_clicked();

    void on_radioButton_Hv_Order_clicked();

    void on_radioButton_Hv_Val_clicked();

    void on_actionLoad_From_file_triggered();

    void on_actionHV_On_Off_triggered();

    void on_actionStop_Server_triggered();

    void on_actionFinder_triggered();

    void on_actionHelp_triggered();

    void on_actionDaq_Run_triggered();

  //  void on_actionCalib_triggered();

    void on_SelectCalib_clicked();


    void on_pushButton_clicked();

    void on_actionMonitoring_triggered();

private:
	Ui::MainWindow *ui;

	CTCP tcp;
	CAmc amc;
	CAsm asmm;
	CThor thor;
	CHvClient hv;

	AMCDATA * p_amcdt;
	ASMDATA * p_asmdt;
	THORDATA * p_thordt;
	HVDATA * p_hvdt;

	u32 m_verbose;
	u32 m_mode;
	u32 m_samples;
	u32 m_niosverbose;
	u16 m_frontend[NB_CHAN];
	u32 m_usemask;
	u16 m_daqsequence;
	int m_interv;
	Qt::CheckState m_auto;
	QTimer *p_timer;


	void XmlParser();
	void checkid(int chan, QString sid, u32 use);
	void AmcDisplay(void);
	void AsmDisplay(u32 channel);
	void ThorDisplay(void);
	void MiscDisplay(void);
	void HvDisplay(void);
 //   void TimerDisplay(void);
    void AmcTip(void);
    void AsmTip(void);
    void ThorTip(void);
    void MiscTip(void);
    void HvTip(void);
    void closeEvent(QCloseEvent *event);
    QString ThorMask(int Asm, u8 Mask);
    monitoring *monitor;
    QString PathMonitor;
    QString PathDir;
    bool OnStartMonitor;
    QProcess *myProcess;
 };
//===============================================
//===============================================

#endif // MAINWINDOW_H
