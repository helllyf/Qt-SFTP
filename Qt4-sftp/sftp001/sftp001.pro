#-------------------------------------------------
#
# Project created by QtCreator 2018-07-03T14:41:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sftp001
TEMPLATE = app


QT           += network



SOURCES += main.cpp\
        mainwindow.cpp \
    operatefile.cpp \
    tinyxml2.cpp \
    sftpclient.cpp \
    msghandlerwapper.cpp

HEADERS  += mainwindow.h \
    operatefile.h \
    tinyxml2.h \
    sftpclient.h \
    msghandlerwapper.h


FORMS    += mainwindow.ui

INCLUDEPATH += ./include/

LIBS += -L$$PWD/lib/ -llibssh2 -lzlib -llibeay32 -lssleay32

RESOURCES += \
    sftp.qrc


RC_FILE += version.rc

