#include "sftpclient.h"

#include <QtDebug>
#include <QTcpSocket>
#include <libssh2_config.h>
#include <iostream>
#include <stdio.h>
#include <QFile>
#include <QStringList>
using namespace std;

sftpClient::sftpClient():
    socket_(0),
    auth_pw(0),
    iStatus(-1),
    sftp_session(0)
{
}


/**
 * @brief sftpClient::initSession
 *
 * @return
 */
int sftpClient::initSession() {
    int host_port = port.toInt();
//    const char * user_name_c = user_name.toAscii().constData();//"mey";
//    const char * pass_word_c = pass_word.toAscii().constData();
//    qDebug() <<user_name<< public_file_path << private_file_path << pass_phrase;
//    const char *keyfile1 =  public_file_path.toAscii().constData();// "d:/id_ssh.pub";
//    const char *keyfile2 = private_file_path.toAscii().constData();//"d:/id_ssh";
//    const char *passphrase =  pass_phrase.toAscii().constData();//"";
//    qDebug()<<"use rsa to connect :"<<user_name_c << keyfile1 << keyfile2;
    int ret = 0;
    int rc;
    iStatus |= 1;
    rc = libssh2_init(0);
    if (rc != 0) {
        return -2;
    }

    iStatus |= 2;
    socket_=new QTcpSocket();
    socket_->connectToHost(host_ip, host_port);
    if(!socket_->waitForConnected())
    {
        qDebug("Error connecting to host %s", host_ip.toLocal8Bit().constData());
        return -1;
    }
    iStatus |= 4;
    /* Create a session instance
     */
    session_ = libssh2_session_init();

    if(!session_)
    {
        return -4;
    }

    /* Since we have set non-blocking, tell libssh2 we are blocking */
    libssh2_session_set_blocking(session_, 1);

    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(session_, socket_->socketDescriptor())) == LIBSSH2_ERROR_EAGAIN);
    if (rc) {
        return -5;
    }
    iStatus |= 8;

    /* At this point we havn't yet authenticated.  The first thing to do
     * is check the hostkey's fingerprint against our known hosts Your app
     * may have it hard coded, may go to a file, may present it to the
     * user, that's your call
     */
    fingerprint = libssh2_hostkey_hash(session_, LIBSSH2_HOSTKEY_HASH_SHA1);
    //for(i = 0; i < 20; i++) {
    //   fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    //}
    //fprintf(stderr, "\n");


    /* check what authentication methods are available */
//    string str_user_name = user_name.toStdString();
//    const char* user_name_c = str_user_name.c_str();
//    string str_pass_word = pass_word.toStdString();
//    const char* pass_word_c = str_pass_word.c_str();


    userauthlist = libssh2_userauth_list(session_, user_name.toLocal8Bit().constData(), \
                                         user_name.toLocal8Bit().length());

    if (strstr(userauthlist, "password") != NULL) {
        auth_pw |= 1;
    }
    if (strstr(userauthlist, "keyboard-interactive") != NULL) {
        auth_pw |= 2;
    }
    if (strstr(userauthlist, "publickey") != NULL) {
        auth_pw |= 4;
    }


    if(auth_pw & 1 && "0" == user_auth)
    {
        qDebug()<<"use pw to connect :"<< user_name ;
        while ((rc = libssh2_userauth_password( session_,
                                                user_name.toLocal8Bit().constData(),
                                                pass_word.toLocal8Bit().constData())) == LIBSSH2_ERROR_EAGAIN);
    }else if(auth_pw & 2)
    {
      //  while ((rc = libssh2_userauth_keyboard_interactive(session_, user_name.toLocal8Bit().constData(), &kbd_sign_callback)) == LIBSSH2_ERROR_EAGAIN);
        //kdb sign
    }else if(auth_pw & 4 && "1" == user_auth)
    {
       qDebug()<<"use rsa to connect :"<<user_name << public_file_path << private_file_path;

       while ((rc = libssh2_userauth_publickey_fromfile(session_, user_name.toLocal8Bit().constData(),
                                                        public_file_path.toLocal8Bit().constData(),
                                                        private_file_path.toLocal8Bit().constData(),
                                                        pass_phrase.toLocal8Bit().constData())) == LIBSSH2_ERROR_EAGAIN);
    }
    qDebug() <<"log rc:" << rc;
    switch(rc) {
        case 0:
            do
            {
                sftp_session = libssh2_sftp_init(session_);
                if(!sftp_session)
                {
                    if(libssh2_session_last_errno(session_) ==
                       LIBSSH2_ERROR_EAGAIN)
                    {
                        qDebug()<<"non-blocking init,wait for read\n";
                        socket_->waitForReadyRead();
                    }
                    else
                    {
                        qDebug()<<"Unable to init SFTP session\n";
                        return -5;
                    }
                }
            }while(!sftp_session);

            return 1;
        break;
        case -18:
            qDebug()<<"authentication failed. check config.";
        break;
        case -16:
            qDebug()<<"file error,check private key";
        break;
    }

    return -1;

}

/**
 * @brief sftpClient::sftpClose
 * @return
 */
int sftpClient::sftpClose() {
    if (iStatus&8)
    {
        libssh2_sftp_shutdown(sftp_session);
        libssh2_session_disconnect(session_, "Normal Shutdown");
        libssh2_session_free(session_);
    }

    if (iStatus&4)
    {
        socket_->close();
    }

    if (iStatus&2)
    {
        libssh2_exit();
    }
    iStatus = 0;
    return 0;
}

/**
* 函数名：SFTPDirExist
* 说明  ：判断SFTP目录是否存在
* 参数  ：1.sftp_session
*           LIBSSH2_SFTP会话指针
*       ：2.sftppath
*           SFTP目录
* 返回值：1指定目录存在；0指定目录不存在
*/
int sftpClient::SFTPDirExist(LIBSSH2_SFTP *sftp_session, const char *sftppath)
{
    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_opendir(sftp_session, sftppath);

    if(sftp_handle)
    {
        libssh2_sftp_closedir(sftp_handle);
    }
    return sftp_handle>0;
}

/**
* 函数名：SFTPFileExist
* 说明  ：判断SFTP文件是否存在
* 参数  ：1.sftp_session
*           LIBSSH2_SFTP会话指针
*       ：2.sftppath
*           SFTP目录
* 返回值：1指定文件存在；0指定文件不存在
*/
int sftpClient::SFTPFileExist(LIBSSH2_SFTP *sftp_session, const char *sftpfilepath)
{
    LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_open(sftp_session, sftpfilepath,\
                                                         LIBSSH2_FXF_READ, \
                                                         0666);
    if(sftp_handle)
    {
        libssh2_sftp_close(sftp_handle);
    }
    return sftp_handle>0;
}


/**
* 函数名：makeSFTPDir
* 说明  ：创建SFTP目录
* 参数  ：1.sftp_session
*           LIBSSH2_SFTP会话指针
*       ：2.sftppath
*           SFTP目录
* 返回值：0成功；-1失败
*/
int sftpClient::makeSFTPDir(LIBSSH2_SFTP *sftp_session, const char *sftppath)
{
    char tmppath[MAXPATH],tmp[MAXPATH];
    const char *seps = "/";
    char *token;
    memset(tmppath,0,MAXPATH);memset(tmp,0,MAXPATH);
    strncpy(tmp,sftppath,MAXPATH-1);
    token = strtok( tmp, seps );
    while( token != NULL )
    {
        snprintf(tmppath,MAXPATH,"%s/%s",tmppath,token);
        if(!SFTPDirExist(sftp_session, tmppath)){
            qDebug()<<tmppath;
            if(libssh2_sftp_mkdir(sftp_session, tmppath,0777))
                return -1;
        }

        token = strtok( NULL, seps );
    }
    return 0;
}

const char* sftpClient::PathFindFileName(const char *slocalfile) {
    //string path="D:/osge/data/world.shp";
    string path(slocalfile);

    int pos=path.find_last_of('/');
    string s(path.substr(pos+1));
    return s.c_str();
}

void sftpClient::closeChannel(LIBSSH2_CHANNEL *channel)
{
    if(channel)
    {
        //libssh2_channel_wait_closed(channel);
        libssh2_channel_close(channel);
        libssh2_channel_free(channel); // free will auto close the channel
    }
}
/**
 * @brief sftpClient::createPath
 *      上传目录管理
 * @param file_name
 * @return
 */
QString sftpClient::createSftpPath(QString file_name){
    //CNY15_20130604102234_00050001XX_UEB00002_HM.FSN
    QStringList file_path_temp = file_name.split('_');
    if(file_path_temp.size() < 3){
        return "";
    }
    QString time_path = file_path_temp[1].mid(0,8);
    QString machine_path = file_path_temp[3];
    return time_path + '/' + machine_path +'/';
}

///* Read, write, execute/search by owner */
//#define LIBSSH2_SFTP_S_IRWXU        0000700     /* RWX mask for owner */
//#define LIBSSH2_SFTP_S_IRUSR        0000400     /* R for owner */
//#define LIBSSH2_SFTP_S_IWUSR        0000200     /* W for owner */
//#define LIBSSH2_SFTP_S_IXUSR        0000100     /* X for owner */
///* Read, write, execute/search by group */
//#define LIBSSH2_SFTP_S_IRWXG        0000070     /* RWX mask for group */
//#define LIBSSH2_SFTP_S_IRGRP        0000040     /* R for group */
//#define LIBSSH2_SFTP_S_IWGRP        0000020     /* W for group */
//#define LIBSSH2_SFTP_S_IXGRP        0000010     /* X for group */
///* Read, write, execute/search by others */
//#define LIBSSH2_SFTP_S_IRWXO        0000007     /* RWX mask for other */
//#define LIBSSH2_SFTP_S_IROTH        0000004     /* R for other */
//#define LIBSSH2_SFTP_S_IWOTH        0000002     /* W for other */
//#define LIBSSH2_SFTP_S_IXOTH        0000001     /* X for other */
int sftpClient::uploadFile(QString file_name){

//    QFile a_file(file_path + file_name);
//    if(a_file.exists() && !a_file.open(QIODevice::ReadOnly|QIODevice::Text))
//    {
//        qCritical()<<"Can't open the file:"<< a_file.fileName();
//        return -1;
//    }

    FILE *hlocalfile;
    hlocalfile = fopen(QString(file_path + file_name).toLocal8Bit().data(), "rb");
    if (!hlocalfile) {
        hlocalfile = NULL;
        return -1;
    }


    QString sftp_path_2 = createSftpPath(file_name);

    if(SFTPFileExist(sftp_session,\
                     (QString(sftp_path+sftp_path_2+file_name)).toLocal8Bit().data())){
        //a_file.close();
        fclose(hlocalfile);
        hlocalfile = NULL;
        qDebug()<<QString(sftp_path + sftp_path_2 +file_name) <<" already exists";
        return 2;
    }

    qDebug()<<"upload dir:"<<QString(sftp_path + sftp_path_2);
    if(!SFTPDirExist(sftp_session, (QString(sftp_path + sftp_path_2)).toLocal8Bit().data())){
        if(makeSFTPDir(sftp_session,(QString(sftp_path + sftp_path_2)).toLocal8Bit().data()) != 0){
        //    a_file.close();
            fclose(hlocalfile);
            hlocalfile = NULL;
            qDebug()<<"mkdir in sftp server failed";
            return -1;
        }
    }



    LIBSSH2_SFTP_HANDLE* sftp_handle;
    do{
        sftp_handle = libssh2_sftp_open(sftp_session,(QString(sftp_path+sftp_path_2+file_name + ".temp")).toLocal8Bit().data(),
                                        LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                                          0666);
        if(!sftp_handle)
        {
            if(libssh2_session_last_errno(session_) != LIBSSH2_ERROR_EAGAIN)
            {
                qDebug()<<"Unable to open file with SFTP,error code:"<< libssh2_session_last_errno(session_);
                libssh2_sftp_close(sftp_handle);
                return -1;
            }
            else
            {
                qDebug()<<"non-blocking open";
                socket_->waitForReadyRead();
            }
        }
    }while(!sftp_handle);
    int rc;
//    QByteArray byte_array;
//    byte_array.resize(4096);
//    char* buffer=byte_array.data();
//    char* ptr=0;
//    int buffer_size=byte_array.size();
//    qint64 nread;
    size_t nread;
    char mem[1024*10];
    char *ptr;
    do
    {
        //nread = fread(mem, 1, sizeof(mem), local);
//        nread = a_file.read(buffer,buffer_size);
//        if (nread <= 0) {
//            /* end of file */
//            break;
//        }
//        ptr = buffer;
            nread = fread(mem, 1, sizeof(mem), hlocalfile);
            if (nread <= 0) {
                /* end of file */
                break;
            }
            ptr = mem;

        do {
            /* write the same data over and over, until error or completion */
            rc = libssh2_sftp_write(sftp_handle, ptr, nread);
            //qDebug()<<rc<<":"<<nread<<" :"<<ptr<<endl;
            if (rc < 0) {
                qDebug()<<"ERROR "<<rc;
                break;
            }
            else {
                /* rc indicates how many bytes were written this time */
                ptr += rc;
                nread -= rc;
            }
        } while (nread);

    } while (1);
    libssh2_sftp_close(sftp_handle);
    //a_file.close();
    fclose(hlocalfile);
    hlocalfile = NULL;
    int retRename = libssh2_sftp_rename(sftp_session,(QString(sftp_path+sftp_path_2+file_name + ".temp")).toLocal8Bit().data(), \
                        (QString(sftp_path+sftp_path_2+file_name )).toLocal8Bit().data());
    if(retRename != 0) {
        qDebug()<<"rename failed:"<<retRename;
    }
    return 1;
}



//int sftpClient::uploadFile(
//        const char *sftppath,
//        const char *slocalfile,
//        const char *sftpfilename)
//{
//    int ret = 0;
//    int rc;
//    char mem[1024*10];
//    size_t nread;
//    char *ptr;
//    FILE *hlocalfile;
//    qDebug()<<sftppath<<slocalfile<<sftpfilename;
//    //sftp session
//    LIBSSH2_SFTP *sftp_session;
//    LIBSSH2_SFTP_HANDLE *sftp_handle;

//    char sftp_filefullpath[MAX_PATH];

//    hlocalfile = fopen(slocalfile, "rb");
//    if (!hlocalfile) {
//        return -1;
//    }

//    do
//    {
//        sftp_session = libssh2_sftp_init(session_);
//        if(!sftp_session)
//        {
//            if(libssh2_session_last_errno(session_) ==
//               LIBSSH2_ERROR_EAGAIN)
//            {
//                qDebug()<<"non-blocking init,wait for read\n";
//                socket_->waitForReadyRead();
//            }
//            else
//            {
//                qDebug()<<"Unable to init SFTP session,error code:" << libssh2_session_last_errno(session_);
//                return -5;
//            }
//        }
//    }while(!sftp_session);

//    /* 上传文件 */
//    rc = 0;
//    if(!SFTPDirExist(sftp_session, sftppath))
//        rc = makeSFTPDir(sftp_session, sftppath);
//    if(rc != 0){
//        fclose(hlocalfile);
//        return -8;
//    }
//    /* FTP文件全路径，包括文件名 */
//    snprintf(sftp_filefullpath,MAX_PATH,
//        sftppath[strlen(sftppath)-1]=='/'?"%s%s":"%s/%s",
//        sftppath,
//        sftpfilename==NULL?PathFindFileName(slocalfile):sftpfilename);

//    sftp_handle = libssh2_sftp_open(sftp_session, sftp_filefullpath,
//                        LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
//                        LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
//                        LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
//    qDebug()<<"handle"<<sftp_handle;
//    if (!sftp_handle) {
//        fclose(hlocalfile);
//        return -9;
//    }

//    do {
//        nread = fread(mem, 1, sizeof(mem), hlocalfile);
//        if (nread <= 0) {
//            /* end of file */
//            break;
//        }
//        ptr = mem;

//        do {
//            /* write data in a loop until we block */
//            rc = libssh2_sftp_write(sftp_handle, ptr, nread);
//            if(rc < 0)
//                break;
//            ptr += rc;
//            nread -= rc;
//        } while (nread);

//    } while (rc > 0);
//    fclose(hlocalfile);
//    libssh2_sftp_close(sftp_handle);
//    libssh2_sftp_shutdown(sftp_session);
//    return 1;
//}

sftpClient::~sftpClient() {
    if(socket_ && session_) {
        sftpClose();
    }
}
