#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QMessageBox>
#include <QIcon>
#include <QTimer>
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scanCount(1),
    isWorking(false)
{
    ui->setupUi(this);
    ui->operate_click->setText("启动");

    //打开日志文件
    outputFile.setFileName("sftpMsg.log");
    outputFile.open(QIODevice::Text |QIODevice::WriteOnly);


   // setWindowIcon(QIcon(":/prefix/favicon.ico"));
    op = new operateFile();
    connect(MsgHandlerWapper::instance(),
                   SIGNAL(message(QtMsgType,QString)),
                   SLOT(outputMessage(QtMsgType,QString)));
}

MainWindow::~MainWindow()
{
    delete m_trayIcon;
    delete ui;
}


void MainWindow::outputMessage(QtMsgType type, const QString &msg)
{
    QString txtMessage;

     switch (type)
     {
         case QtDebugMsg:    //调试信息提示
             txtMessage = QString("Debug: %1").arg(msg);
             break;

         case QtWarningMsg:    //一般的warning提示
             txtMessage = QString("Warning: %1").arg(msg);
             break;

         case QtCriticalMsg:    //严重错误提示
             txtMessage = QString("Critical: %1").arg(msg);
             break;

         case QtFatalMsg:    //致命错误提示
             txtMessage = QString("Fatal: %1").arg(msg);
             abort();
     }


    if(ui->debug_output->blockCount()>100){
        ui->debug_output->setPlainText("");
    }
    ui->debug_output->appendPlainText(txtMessage);
    QTextStream textStream(&outputFile);
    textStream << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) << ": " << txtMessage <<endl;
}

/**
 * @brief MainWindow::closeEvent
 * @param event
 * 监听关闭事件，删除托盘图标操作
 */

void MainWindow::closeEvent(QCloseEvent *event)
{
    //TODO: 在退出窗口之前，实现希望做的操作
    QMessageBox::StandardButton button;
        button=QMessageBox::question(this,QString("退出"),QString(QString("是否退出SFTP工具?")),QMessageBox::Yes|QMessageBox::No);
        if(button==QMessageBox::No)
        {
            this->hide();
            event->ignore(); // 忽略退出信号，程序继续进行
        }
        else if(button==QMessageBox::Yes)
        {
            m_trayIcon->hide();
            this->close();
            event->accept(); // 接受退出信号，程序退出
        }

}

/**
 * @brief MainWindow::beginToUpload
 *  1 second * 60 * 5 = 5 min
 */
void MainWindow::beginToUpload() {

    isWorking = true;
    qDebug()<<"scan file :"<<scanCount<<" times";
    if(op != NULL) {
        op->scanFile();
        scanCount++;
        QTimer::singleShot(op->getCircleTime(), this, SLOT(setTimeToUpload()));
        //QTimer::singleShot(1000, this, SLOT(setTimeToUpload()));
    }
//    connect(timer,SIGNAL(timeout()),this,SLOT(setTimeToUpload()));
//    timer->start(5000);
}

void MainWindow::setTimeToUpload() {
    beginToUpload();
}


void MainWindow::showWorkingMessage() {
    m_trayIcon->showMessage(QString("sftp"),QString("SFTP 正在运行"));
}

void MainWindow::quitApp() {
    m_trayIcon->hide();
    QApplication::exit( 0 );
}


int MainWindow::checkXml() {
    if(op->readFromXml() == 1) {
        loadXmlToWindow();
        if(op->sftpOpen() == 1)
            return 1;
    }
    return -1;
}


void MainWindow::loadXmlToWindow() {
    ui->ip->setText(op->getIp());
    ui->port->setText(op->getPort());
    ui->user_name->setText(op->getUserName());
    ui->pass_word->setText(op->getPassWord());
    ui->scan_path->setText(op->getFilePath());
    ui->scan_path_backup->setText(op->getFilePathBackup());
    ui->sftp_path->setText(op->getSftpPath());
    ui->circle_time->setText(op->getCircleTimeS());
    ui->public_key->setText(op->getPublicFilePath());
    ui->private_key->setText(op->getPrivateFilePath());
    ui->pass_phrase->setText(op->getPassPhrase());
    if(op->getUserAuth()=="1"){
        ui->use_private->setChecked(true);
        useKeyModel();
    }else{
        ui->use_private->setChecked(false);
        usePassWordModel();
    }
}


void MainWindow::warm(QString s) {
    QMessageBox::warning(this, QString("warning"),s);
}

void MainWindow::showTrayIcon() {
    m_menu = new QMenu();
    m_quitAction =  m_menu->addAction("退出");
    connect(m_quitAction, SIGNAL(triggered(bool)), this, SLOT(quitApp()));

    m_trayIcon= new QSystemTrayIcon(QIcon(":/prefix/favicon.ico"));
   // m_trayIcon.setIcon(QIcon(":/prefix/favicon.ico"));
    m_trayIcon->setToolTip(QString("SFTP 正在运行"));    //设置鼠标放上去显示的信息
    m_trayIcon->setContextMenu(m_menu);
    m_trayIcon->show();
    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
                SLOT(slot_iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::slot_iconActivated(QSystemTrayIcon::ActivationReason reason)
{

    switch (reason) {
    case QSystemTrayIcon::Unknown:
        break;
    case QSystemTrayIcon::Context:
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::Trigger:
        if(isWorking == true){
            ui->operate_click->setText("退出");
        }else{
            ui->operate_click->setText(QObject::tr("启动"));
        }
        this->show();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        break;
    }
}

const int MainWindow::checkConfig() {
    if(ui->ip->text() == ""){
        warm(QString("ip is needed"));
        return -1;
    }else{
        ip = ui->ip->text();
    }
    if(ui->port->text() == ""){
        warm(QString("port is needed"));
        return -1;
    }else{
        port = ui->port->text();
    }
    user_name = ui->user_name->text();
    pass_word = ui->pass_word->text();
    scan_path = ui->scan_path->text();
    scan_path_backup = ui->scan_path_backup->text();
    sftp_path = ui->sftp_path->text();
    return 1;
}




/**
 * @brief slot
 */

/**
 * @brief MainWindow::on_operate_click_clicked
 */
// const char *ip, *port, *user_name, *pass_word, *scan_path, *scan_path_backup, *sftp_path;
void MainWindow::on_operate_click_clicked()
{
    if( isWorking == false){
//        if(checkConfig()!= 1){
//            return;
//        }
        //op->loadConfFromW(ip,port,user_name,pass_word,scan_path,scan_path_backup,sftp_path);
//        op->saveAsSftpConf();
        on_save_clicked();
        if(op->sftpOpen()!=1 ){
            qDebug("connect error");
            return;
        }
        ui->operate_click->setText("退出");
        this->hide();
        showWorkingMessage();
        beginToUpload();
    }else{
        quitApp();
    }
}


void MainWindow::on_save_clicked()
{
//    if(checkConfig()!= 1){
//        return;
//    }
    op->loadConfFromW(ui->ip->text(),
                      ui->port->text(),
                      ui->user_name->text(),
                      ui->pass_word->text(),
                      ui->scan_path->text(),
                      ui->scan_path_backup->text(),
                      ui->sftp_path->text(),
                      ui->use_private->isChecked()?QString("1"):QString("0"),
                      ui->public_key->text(),
                      ui->private_key->text(),
                      ui->pass_phrase->text(),
                      ui->circle_time->text()
                        );
//    op->saveAsSftpConf();
}




void MainWindow::getFilePath(const int pos,const int type) {
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    QString title;
    QLineEdit * line_output;
    switch(pos) {
        case 0:
            title = "open scanPath directory";
            line_output = ui->scan_path;
        break;
        case 1:
            title = "open scanPathBackup directory";
            line_output = ui->scan_path_backup;
        break;
        case 2:
            title = "open public key file";
            line_output =  ui->public_key;
        break;
        case 3:
            title = "open private key file";
            line_output =  ui->private_key;
        break;
    }
    fileDialog->setWindowTitle(title);
    switch(type){
        case 0:
            fileDialog->setFileMode(QFileDialog::DirectoryOnly);
        break;
        case 1:
            //fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
            //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
            fileDialog->setFileMode(QFileDialog::ExistingFiles);
        break;
    }
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        qDebug()<<fileNames[0]<<endl;
        line_output->setText(fileNames[0]);
    }
    delete fileDialog;
    fileDialog = NULL;
}



void MainWindow::on_scanPathBackup_file_clicked()
{
    getFilePath(1,0);
}

void MainWindow::on_scanPath_file_clicked()
{
    getFilePath(0,0);
}

void MainWindow::on_public_file_clicked()
{
    getFilePath(2,1);
}
void MainWindow::on_private_file_clicked()
{
    getFilePath(3,1);
}
void MainWindow::usePassWordModel() {
    ui->public_key->setEnabled(false);
    ui->public_file->setEnabled(false);
    ui->private_file->setEnabled(false);
    ui->private_key->setEnabled(false);
    ui->public_key->setText("");
    ui->private_key->setText("");
    ui->pass_phrase->setText("");
    ui->pass_word->setEnabled(true);
}

void MainWindow::useKeyModel() {
    ui->public_key->setEnabled(true);
    ui->public_file->setEnabled(true);
    ui->private_file->setEnabled(true);
    ui->private_key->setEnabled(true);
   // ui->pass_phrase->setEnabled(true);
    ui->pass_word->setText("");
    ui->pass_word->setEnabled(false);
}

void MainWindow::on_use_private_stateChanged(int arg1)
{
    //use password
    if(arg1 == 0){
        usePassWordModel();
    }else if(arg1 == 2) {
        //use public key
        useKeyModel();
    }
}


