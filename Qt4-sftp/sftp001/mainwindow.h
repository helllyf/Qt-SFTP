#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "operatefile.h"
#include "sftpclient.h"
#include <QIcon>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QFile>
#include "msghandlerwapper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void warm(QString s);
    const int checkConfig();
    void showTrayIcon();
    int checkXml();
    void beginToUpload();
    void showWorkingMessage();
    void getFilePath(const int pos,const int type);
    void loadXmlToWindow();

    void useKeyModel();
    void usePassWordModel();

private slots:
    void on_operate_click_clicked();
    void setTimeToUpload();
    void quitApp();
    void on_save_clicked();
    void slot_iconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_private_file_clicked();
    void on_scanPathBackup_file_clicked();

    void on_scanPath_file_clicked();

    void on_public_file_clicked();

    void on_use_private_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QString ip, port, user_name, pass_word, scan_path, scan_path_backup, sftp_path;
    operateFile *op;
    bool isWorking;


    int scanCount;
    QSystemTrayIcon *m_trayIcon;
    QAction *m_quitAction;
    QMenu *m_menu;
    QFile outputFile;


protected:
     void closeEvent(QCloseEvent *event);

public slots:
    void outputMessage(QtMsgType type, const QString &msg);

};

#endif // MAINWINDOW_H
