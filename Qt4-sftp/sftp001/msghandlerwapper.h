/*
  (C) 2011 dbzhang800#gmail.com
*/
#ifndef MSGHANDLERWAPPER_H
#define MSGHANDLERWAPPER_H
#include <QtCore/QObject>

class MsgHandlerWapper:public QObject
{
    Q_OBJECT
public:
    static MsgHandlerWapper * instance();

signals:
    void message(QtMsgType type, const QString &msg);

private:
    MsgHandlerWapper();
    static MsgHandlerWapper * m_instance;
};

#endif // MSGHANDLERWAPPER_Hs
