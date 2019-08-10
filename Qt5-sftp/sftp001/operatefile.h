#ifndef OPERATEFILE_H
#define OPERATEFILE_H
#include <QString>
#include "sftpclient.h"
#include "tinyxml2.h"
using namespace tinyxml2;
class operateFile: public sftpClient
{

public:


    operateFile();
    ~operateFile();
    /**
     * @brief scanFile
     *  scan for fsn files
     */
    void scanFile();
    /**
     * @brief readFromXml
     *  read sftp config
     */
    const int readFromXml();
    /**
     * @brief sftpOpen
     *  connect to sftp server
     */

    const int sftpOpen();

    /**
     * @brief remove_and_backupFile
     * @param slocalfile
     *  remove and backup fsn after upload
     */
    const int remove_and_backupFile(QString slocalfile);
    void checkPath();

    const char* readXmlValue(tinyxml2::XMLElement* root,const char* element);
    void loadConfFromW(
            QString host_ip_,
            QString port_,
            QString user_name_,
            QString pass_word_,
            QString file_path_,
            QString file_path_backup_,
            QString sftp_path_,
            QString user_auth,
            QString public_file_path,
            QString private_file_path,
            QString pass_phrase,
            QString circle_time);

    const int getCircleTime();
    void saveXmlOneByOne(tinyxml2::XMLDocument *doc ,tinyxml2::XMLElement* root,const char* element,QString s);
    int saveAsSftpConf();


private :
      QString xml_path;

public :

      QString getIp();
      QString getPort();
      QString getUserName();
      QString getPassWord();
      QString getSftpPath();
      QString getFilePath();
      QString getFilePathBackup();
      QString getUserAuth();
      QString getPublicFilePath();
      QString getPrivateFilePath();
      QString getPassPhrase();
      QString getCircleTimeS();
};

#endif // OPERATEFILE_H
