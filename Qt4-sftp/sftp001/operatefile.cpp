#include "operatefile.h"
#include <QtDebug>

#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QTimer>
// 解决中文乱码的问题.
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif





operateFile::operateFile():
    xml_path("./sftpconfig.xml")
{
}

void operateFile::scanFile()
{
    QDir dir(file_path);
    QStringList filters;
    filters << "*.fsn" ;//<<"*cpp"
    dir.setNameFilters(filters);
    QStringList file_list = dir.entryList();

    QStringList::iterator i;
    for(i = file_list.begin();i != file_list.end();i++) {
        qDebug()<<  "file : " <<*i <<" wait to upload.";
        QString file_path_temp = file_path + *i ;
//        QString sftp_path_temp = sftp_path + *i;
        //const char *slocalfile = temp.toStdString().c_str();
        int rc = uploadFile(*i);
        switch(rc) {
            case 1:
                qDebug()<< file_path_temp<<" upload success.";
                remove_and_backupFile(file_path_temp);
            break;
            case -5:
                qDebug()<<" sftp init failed ,sftp reopen!";
            break;
            case  -1:
                qDebug()<<file_path_temp << " file open failed.";
            break;
            case -8:
                qDebug() <<file_path_temp <<"upload failed ,please check network, sftp server and restart.";
            break;
            case -9:
                qDebug()<< file_path_temp << "fstp open failed,check ip, port, username, password! please restart! ";
            break;
            default:
            break;
        }
    }
}




void operateFile::checkPath()
{
    file_path.replace(QRegExp("\\"), "/");
    file_path_backup.replace(QRegExp("\\"), "/");
    sftp_path.replace(QRegExp("\\"), "/");
    if(!file_path.endsWith("/"))
        file_path += "/";
    if(!file_path_backup.endsWith("/"))
        file_path_backup += "/";
    if(!sftp_path.endsWith("/"))
        sftp_path += "/";
}

const char* operateFile::readXmlValue(XMLElement* root,const char* element) {
    if(root->FirstChildElement(element) == NULL){
        qDebug()<<"not find:"<< element;
        return "";
    }else{
        return root->FirstChildElement(element)->GetText();
    }

    // (root->FirstChildElement(element))->Attribute("value");
}

const int operateFile::readFromXml()
{
    XMLDocument doc;
    int res = doc.LoadFile(xml_path.toStdString().c_str());
    if(res != 0)
    {
        qDebug()<<"load xml file failed";
        return -1;
    }
    XMLElement* root=doc.RootElement();
    host_ip = QString(readXmlValue(root,"ip"));
    port = QString(readXmlValue(root,"port"));
    user_name = QString(readXmlValue(root,"username"));
    pass_word = QString(readXmlValue(root,"password"));
    file_path = QString(readXmlValue(root,"scanPath"));
    file_path_backup = QString(readXmlValue(root,"scanPathBackup"));
    sftp_path = QString(readXmlValue(root,"sftpPath"));

    user_auth = QString(readXmlValue(root,"userAuth"));
    public_file_path = QString(readXmlValue(root,"publicKeyPath"));
    private_file_path =  QString(readXmlValue(root,"privateKeyPath"));
    pass_phrase = QString(readXmlValue(root,"passPhrase"));
    circle_time = QString(readXmlValue(root,"circleTime"));
    checkPath();
    return 1;
}

const int operateFile::getCircleTime() {
    int time_temp = 2;
    if(circle_time.toInt()!= 0)
        time_temp = circle_time.toInt();
    return 60 * 1000 * time_temp;
}

void operateFile::loadConfFromW(
        QString host_ip_,
        QString port_,
        QString user_name_,
        QString pass_word_,
        QString file_path_,
        QString file_path_backup_,
        QString sftp_path_,
        QString user_auth_,
        QString public_file_path_,
        QString private_file_path_,
        QString pass_phrase_,
        QString circle_time_) {

    host_ip = host_ip_;
    port = port_;
    user_name = user_name_;
    pass_word = pass_word_;
    file_path = file_path_ ;
    file_path_backup = file_path_backup_ ;
    sftp_path = sftp_path_;
    user_auth = user_auth_;
    public_file_path = public_file_path_;
    private_file_path = private_file_path_;
    pass_phrase = pass_phrase_;
    circle_time = circle_time_;
    checkPath();
    saveAsSftpConf();
    qDebug()<<"saved to xml:"<<host_ip<<" "<<port<< " "<<user_name<<pass_word<<file_path<<file_path_backup<<sftp_path \
              <<user_auth<<public_file_path<<private_file_path<<pass_phrase;
}
/**
 * 保存xml文件
 * @brief operateFile::saveAsSftpConf
 * @return
 */

void operateFile::saveXmlOneByOne(XMLDocument *doc ,XMLElement* root,const char* element,QString s) {
    qDebug()<<s;
    if(s == NULL || s == ""){
        return;
    }
    XMLElement* el = doc->NewElement(element);
    el->InsertEndChild(doc->NewText(s.toStdString().c_str()));
    root->InsertEndChild(el);
}

int operateFile::saveAsSftpConf() {
    XMLDocument doc;
    XMLElement* root=doc.NewElement("sftp");
    saveXmlOneByOne(&doc,root,"ip",host_ip);
    saveXmlOneByOne(&doc,root,"port",port);
    saveXmlOneByOne(&doc,root,"username",user_name);
    saveXmlOneByOne(&doc,root,"password",pass_word);
    saveXmlOneByOne(&doc,root,"scanPath",file_path);
    saveXmlOneByOne(&doc,root,"scanPathBackup",file_path_backup);
    saveXmlOneByOne(&doc,root,"sftpPath",sftp_path);

    //秘钥
    saveXmlOneByOne(&doc,root,"userAuth",user_auth);
    saveXmlOneByOne(&doc,root,"publicKeyPath",public_file_path);
    saveXmlOneByOne(&doc,root,"privateKeyPath",private_file_path);
    saveXmlOneByOne(&doc,root,"passPhrase",pass_phrase);
    saveXmlOneByOne(&doc,root,"circleTime",circle_time);
    doc.InsertEndChild(root);
    doc.SaveFile(xml_path.toStdString().c_str());
    return 0;
}

const int operateFile::sftpOpen()
{
    if(initSession() != 1){
        qDebug()<<"Init session failed!";
        return -1;
    }else{
        QDir dir;
        if (!dir.exists(file_path))
        {
            bool res = dir.mkpath(file_path);
            qDebug()<<"i do not found scanPath,mkdir scanPath:"<< res;
        }
        if (!dir.exists(file_path_backup))
        {
            bool res = dir.mkpath(file_path_backup);
            qDebug()<<"i do not found scanPathBackup,mkdir scanPathBackup:"<< res;
        }
        qDebug()<<"Init session successed!";
        return 1;
    }
}

//void operateFile::sftpReopen() {
//    delete sftpc;
//    sftpc = NuLL;
//    sftpOpen();
//}

const int operateFile::remove_and_backupFile(QString slocalfile)
{
    QString old_name(slocalfile);
    QString new_name(slocalfile);
    new_name = new_name.split('/').last();
    new_name.append(".bak");
    new_name.prepend(file_path_backup);
    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(file_path_backup))
    {
        bool res = dir.mkpath(file_path_backup);
        qDebug()<<"i do not found scanPathBackup,mkdir scanPathBackup:"<< res;
    }

    //移动旧文件到备份文件夹
    if(QFile::rename(old_name,new_name)){
        return 1;
    }else{
        //备份文件夹中文件已存在，则覆盖旧文件
        qDebug()<<new_name<<" exists";
        if(QFile::remove(new_name)){
            qDebug()<<new_name<<" remove success";
        }else{
            //同名文件无删除权限，则文件名后加bak;
            qDebug()<<new_name<<"same file remove fail,add bak to new file";
            new_name.append(".bak");
        }
        QFile::rename(old_name,new_name);
        return 1;
    }
}

//QString host_ip;
//QString port;
//QString user_name;
//QString pass_word;
//QString sftp_path;
QString operateFile::getIp() {
    return host_ip;
}
QString operateFile::getPort() {
    return port;
}
QString operateFile::getUserName() {
    return user_name;
}
QString operateFile::getPassWord() {
    return pass_word;
}
QString operateFile::getSftpPath() {
    return sftp_path;
}

//QString file_path;//上传文件目录
//QString file_path_backup;//备份文件目录
//QString user_auth;//0表示密码 1表示公钥
//QString public_file_path;
QString operateFile::getFilePath() {
    return file_path;
}
QString operateFile::getFilePathBackup() {
    return file_path_backup;
}
QString operateFile::getUserAuth() {
    return user_auth;
}
QString operateFile::getPublicFilePath() {
    return public_file_path;
}

//QString private_file_path;
//QString pass_phrase;
//QString circle_time;//扫描循环时间,单位为分钟
QString operateFile::getPrivateFilePath() {
    return private_file_path;
}
QString operateFile::getPassPhrase() {
    return pass_phrase;
}
QString operateFile::getCircleTimeS() {
    return circle_time;
}
