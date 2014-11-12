#-------------------------------------------------
#
# Project created by QtCreator 2011-08-06T19:34:40
#
#-------------------------------------------------

QT          +=  core gui \
                network

TARGET      =   HermesTrader
TEMPLATE    =   app


SOURCES     +=  main.cpp\
                WndMain.cpp \
                Plot.cpp \
                Target.cpp \
                Template.cpp \
                Predict.cpp \
                DlgChart.cpp \
                DlgAppStart.cpp \
                SMAThread.cpp \
                EMAThread.cpp \
                RMAThread.cpp \
                WMAThread.cpp \
                ChangeThread.cpp \
                TermLowThread.cpp \
                TermHighThread.cpp \
                BackTestThread.cpp \
                SimulationThread.cpp \
                IchimokuThread.cpp \
                BollingerBandThread.cpp \
                FibonacciThread.cpp

HEADERS     +=  WndMain.h \
                Plot.h \
                Target.h \
                Template.h \
                Predict.h \
                DlgChart.h \
                DlgAppStart.h \
                SMAThread.h \
                EMAThread.h \
                RMAThread.h \
                WMAThread.h \
                ChangeThread.h \
                TermLowThread.h \
                TermHighThread.h \
                BackTestThread.h \
                SimulationThread.h \
                IchimokuThread.h \
                BollingerBandThread.h \
                FibonacciThread.h

FORMS       +=  WndMain.ui \
                DlgChart.ui \
                DlgAppStart.ui

DEFINES     +=  QWT_DLL \
                QWTD_DLL

# For checking contents
message(Printing DEFINES:[$$DEFINES]...)
message(Printing CONFIG:[$$CONFIG]...)

contains(CONFIG, ReleaseBuild){
#ReleaseBuild
message(Building release...)
DEFINES     +=  QT_NO_DEBUG
LIBS        +=  qwt.lib
LIBS        +=  qwtmathml.lib
} else {
#DebugBuild
message(Building debug...)
DEFINES     +=  QT_DEBUG
LIBS        +=  qwtd.lib
LIBS        +=  qwtmathmld.lib
}




























