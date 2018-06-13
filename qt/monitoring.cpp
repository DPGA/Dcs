#include "monitoring.h"
#include <QListWidget>
#include <QDockWidget>
#include <QGroupBox>
#include <QLCDNumber>
#include <QPushButton>

monitoring::monitoring(QWidget *parent) :
    DecodeFrame()
{
//    ui->setupUi(this);

    lMyPlotsQwt.clear();
    // Opening SHM
    //===============================================
    ShdMem = new ShmRingBuffer<SharedMemory>(CAPACITY,false,SHM_ASM_DATA);
    //      ShdNet = new ShmRingBuffer<sStatFrame>(CAPACITY,true,SHM_NETWORK);
    //===============================================
    DisplayAsm = new MyWindow();
    DisplayAsm->setWindowTitle("Asm" + QString::number(0));


    QWidget *wid = new QWidget();
    QGridLayout* gBox = new QGridLayout;
    bClear = new QPushButton("Clear");
    gBox->addWidget(bClear,0,0);
    gBox->addWidget(new QPushButton("b1"),0,1);
    for (int row=0;row<6;row++) {
        for (int col=0;col<4;col++) {
            lMyPlotsQwt << new MyPlotsQwt("Ch" + QString::number(row*4+col),false);
            gBox->addWidget(lMyPlotsQwt.back(),row+1,col);
        }
        QLCDNumber *temp= new QLCDNumber();
        temp->setGeometry(50,50,100,100);  // Marche pas à voir
        gBox->addWidget(temp,row+1,4);

        lNumber << temp;
    }

    wid->setLayout(gBox);
 //   DisplayAsm->setFocusPolicy(Qt::ClickFocus);
    DisplayAsm->setCentralWidget(wid);
    DisplayAsm->show();
 //   connect(DisplayAsm,SIGNAL(Actived()),this,SLOT(setModal()));
//    connect(DisplayAsm,SIGNAL(Desactived()),this,SLOT(unsetModal()));

}

monitoring::~monitoring()
{

}

void monitoring::ClearGraph()
{
    for (int Ch=0;Ch<24;Ch++)
        lMyPlotsQwt.at(Ch)->clearCurve();
}

void monitoring::ReadShmData()
//===============================================
//===============================================
{
   struct S_HeaderFrame *Header;
   uint16_t *buf;



   while (ShdMem->begin() != ShdMem->end()) {
 //       qInfo() << "Traitement Fragment";
        SharedMemory Packet = ShdMem->dump_front();
        SetPacket((uint16_t *) &Packet.raw[0]);
        Header = (struct S_HeaderFrame *) &Packet.raw[0];

        int FeID = GetFeId(); //((ntohs(Header->FeIdK30) & 0x7f00) >> 8) - 0x10;
        if (FeID >= 0x10) FeID -= 0x10;

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
        QVector <double> x,y;

 //       if (ui->FrontEnd->value() == FeID)
        {
            for (int j=0;j<4;++j) {
                x.clear();y.clear();
                buf = GetChannel(j);
                unsigned short Ch = GetCh();
                for (int k=0;k<nbSamples;k++) {
                    x.push_back(k);
                    double temp=(double)(ntohs(buf[2+k]));
                    y.push_back((double) (ntohs(buf[2+k]) & 0xfff));

 /*                   if ((k<10) && (temp > 2000)){
                        if (k==0) qDebug() << "channel " << Ch;
                        qDebug() << ntohs(buf[2+k]) << " " << y.back() << " " << temp;
                    }
  */              }


                if ((Ch >=0) && (Ch < 24)) {
  //               MyPlotsQwt *mp = lMyPlotsQwt.at(Ch);
  //                if (!ui->allCurves->isChecked()) lMyPlotsQwt.at(Ch)->clearCurve();
                  lMyPlotsQwt.at(Ch)->setData(x,y);
                  if (j==0) lNumber.at(Ch/4)->display(lNumber.at(Ch/4)->value()+1);
                }
             }

        }
    }
   usleep(1000);
}


void monitoring::setModal()
{
    DisplayAsm->hide();
    DisplayAsm->setWindowModality(Qt::WindowModal);
    DisplayAsm->show();
   //setWindowModality(Qt::WindowModal);
}

void monitoring::unsetModal()
{
    DisplayAsm->hide();
    DisplayAsm->setWindowModality(Qt::NonModal);
    DisplayAsm->show();
   //setWindowModality(Qt::NonModal);
}

void monitoring::process()
{
    while(true)
        ReadShmData();

}
