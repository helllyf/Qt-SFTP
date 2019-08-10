/*
  (C) 2011 dbzhang800#gmail.com
*/

#include "msghandlerwapper.h"
#include <QtCore/QMetaType>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>
/*
 * qt4
void static msgHandlerFunction(QtMsgType type, const char *msg)
{
    QMetaObject::invokeMethod(MsgHandlerWapper::instance(), "message"
                        , Q_ARG(QtMsgType, type)
                        , Q_ARG(QString, QString::fromLocal8Bit(msg)));
}
*/
//qt5 写法
void static msgHandlerFunction(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QMetaObject::invokeMethod(MsgHandlerWapper::instance(), "message"
                        , Q_ARG(QtMsgType, type)
                        , Q_ARG(QString, msg));
}
//采用单例模式
MsgHandlerWapper * MsgHandlerWapper::m_instance = 0;

MsgHandlerWapper * MsgHandlerWapper::instance()
{
    static QMutex mutex;
    if (!m_instance) {
        QMutexLocker locker(&mutex);//简化互斥锁,当多个线程开启debug，互斥访问,当m_instance不存在，需要等待其他debug new以后再尝试。
        if (!m_instance)
            m_instance = new MsgHandlerWapper;
    }

    return m_instance;
}

MsgHandlerWapper::MsgHandlerWapper()
    :QObject(qApp)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");//跨线程中使用
    //qInstallMsgHandler(msgHandlerFunction); //qt4
    qInstallMessageHandler(msgHandlerFunction);//qt5
}
