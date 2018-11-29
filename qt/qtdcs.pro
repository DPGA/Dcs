#-------------------------------------------------
#
# Project created by QtCreator 2018-02-09T10:01:54
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtdcs
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


include ( /usr/local/qwt/features/qwt.prf )

SOURCES += main.cpp\
        mainwindow.cpp \
    Amc.cpp \
    Asm.cpp \
    dial_about.cpp \
    dial_alldaq.cpp \
    dial_amcdcdce.cpp \
    dial_amcid.cpp \
    dial_amcreg.cpp \
    dial_amcreset.cpp \
    dial_asmconfig.cpp \
    dial_asmdaq.cpp \
    dial_asmprog.cpp \
    dial_asmreg.cpp \
    dial_calib.cpp \
    dial_confirm.cpp \
    dial_finder.cpp \
    dial_hvconfig.cpp \
    dial_hvonoff.cpp \
    dial_thorconfig.cpp \
    dial_thordaq.cpp \
    dial_thorprog.cpp \
    dial_thorreg.cpp \
    dial_verbose.cpp \
    HvClient.cpp \
    Tcp.cpp \
    Thor.cpp \
    Tools.cpp \
    myplotsqwt.cpp \
    histogramme.cpp \
    mywindow.cpp \
    dial_1asm.cpp \
#  monitoring.cpp \
    ipc.cpp \
    dial_mezza.cpp

HEADERS  += \
    Amc.h \
    dial_amcid.h \
    dial_amcdcdce.h \
    dial_alldaq.h \
    dial_about.h \
    command_error.h \
    Asm.h \
    AmcData.h \
    dial_amcreg.h \
    dial_amcreset.h \
    dial_asmconfig.h \
    dial_asmdaq.h \
    dial_asmprog.h \
    dial_asmreg.h \
    dial_calib.h \
    dial_confirm.h \
    dial_finder.h \
    dial_hvconfig.h \
    dial_hvonoff.h \
    dial_thorconfig.h \
    dial_thordaq.h \
    dial_thorprog.h \
    dial_thorreg.h \
    dial_verbose.h \
    hexspinbox.h \
    HvClient.h \
    MyDefs.h \
    Nios_system.h \
    Tcp.h \
    Thor.h \
    Tools.h \
    myplotsqwt.h \
    histogramme.h \
    zoomer.h \
    mywindow.h \
    dial_1asm.h \
    version.h \
#    monitoring.h \
    ipc.h \
    mainwindow.h \
    dial_mezza.h

FORMS += \
    mainwindow.ui \
    dial_about.ui \
    dial_alldaq.ui \
    dial_amcdcdce.ui \
    dial_amcid.ui \
    dial_amcreg.ui \
    dial_amcreset.ui \
    dial_asmconfig.ui \
    dial_asmdaq.ui \
    dial_asmprog.ui \
    dial_asmreg.ui \
    dial_calib.ui \
    dial_confirm.ui \
    dial_finder.ui \
    dial_hvconfig.ui \
    dial_hvonoff.ui \
    dial_thorconfig.ui \
    dial_thordaq.ui \
    dial_thorprog.ui \
    dial_thorreg.ui \
    dial_verbose.ui \
    dial_1asm.ui \
    dial_mezza.ui


INCLUDEPATH += ../../Lib/libshm/include/
INCLUDEPATH += ../../Lib/libframe/include/
INCLUDEPATH += ../../common/include/


INCLUDEPATH += $$PWD/../../../ServeurUdp/build/lib
DEPENDPATH += $$PWD/../../../ServeurUdp//build/lib

unix:!macx: LIBS += -L$$PWD/../../build/lib/ -lringshm

INCLUDEPATH += $$PWD/../../../ServeurUdp/build
DEPENDPATH += $$PWD/../../../ServeurUdp/build

unix:!macx: LIBS += -L$$PWD/../../build/lib/ -lframe

INCLUDEPATH += $$PWD/../../../ServeurUdp/build
DEPENDPATH += $$PWD/../../../ServeurUdp/build

unix:!macx: PRE_TARGETDEPS += $$PWD/../../build/lib/libframe.a
