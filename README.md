# Qt-SFTP
Qt Based SFTP,Contained Qt4 and Qt5 version

# 基本功能：
1.功能实现了指定文件后缀名定时上传功能。   
2.sftp实现通过密码和秘钥方式上传。   
3.有完善的xml配置功能。 程序开启默认读取配置文件实现上传功能。若配置文件读取失败或者连接服务器失败，则弹出配置窗口来配置。  

# 安装环境：
window版本：win10  
编译器：    mingw 4.8以上  


# 程序比较简单，在libssh2上完成的功能  
1.msghandlerwapper用于存储log  
2.operatefile用于操作文件  
3.tinyxml2是解析xml的库  



# ToDo：
1.下载功能。  


# 声明：
任何人均可以使用，可以用于任何用途。但是程序中用到其他的开源库，使用规则遵循库本身的要求。
