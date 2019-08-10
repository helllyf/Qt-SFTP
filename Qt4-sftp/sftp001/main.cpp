#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//情况1

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    MainWindow w;
    w.showTrayIcon();
    if(w.checkXml() == 1) {
        w.showWorkingMessage();
        w.beginToUpload();
    }else{
        w.show();
    }
    return a.exec();
}
