#ifndef SFTPCLIENT_H
#define SFTPCLIENT_H
#include <QTcpSocket>
#include <libssh2.h>
#include <libssh2_sftp.h>
#define MAXPATH 256
class sftpClient
{
public:
    sftpClient();
    ~sftpClient();
    int initSession();
    int sftpClose();
    int uploadFile(QString file_name);
    int SFTPDirExist(LIBSSH2_SFTP *sftp_session, const char *sftppath);
    int makeSFTPDir(LIBSSH2_SFTP *sftp_session, const char *sftppath);
    const char* PathFindFileName(const char *slocalfile);
    void closeChannel(LIBSSH2_CHANNEL *channel);
    QString createSftpPath(QString file_name);

private:
    QTcpSocket *socket_;
    //ssh session
    LIBSSH2_SESSION *session_;
    LIBSSH2_SFTP* sftp_session;



    const char *fingerprint;
    char *userauthlist;
    int auth_pw;
    int iStatus;

protected:

    QString host_ip;
    QString port;
    QString user_name;
    QString pass_word;
    QString sftp_path;

    QString file_path;//上传文件目录
    QString file_path_backup;//备份文件目录

    QString user_auth;//0表示密码 1表示公钥
    QString public_file_path;
    QString private_file_path;
    QString pass_phrase;
    QString circle_time;//扫描循环时间,单位为分钟
};

#endif // SFTPCLIENT_H
