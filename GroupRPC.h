#ifndef GROUPRPC_H
#define GROUPRPC_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QMap>
#include <QList>
#include <QScriptValue>

class GroupRequest : public QObject
{
    Q_OBJECT
public:
    friend class GroupRPC;
    explicit GroupRequest(QObject *parent = 0);
    uint seq;
    QVariant ret;
private:
    static uint nextSeq;
signals:
    void respond();
public slots:
};

class GroupRPC : public QObject
{
    Q_OBJECT
public:
    explicit GroupRPC(QObject *parent = 0);
    bool init(int p=5432);
private:
    QVariant callbak(QVariantMap msg);
    void send(QVariantMap &msg);
private:
    QMap<QString,QScriptValue> thisMap;
    QMap<QString,QScriptValue> methodMap;
    QList<GroupRequest*> requestList;
    QString localMac;
    QHostAddress localIp;
    QUdpSocket socket;
    QHostAddress fromIp;
    QString fromMac;
    int timeout;
    int port;
    int groupId;
signals:

public slots:
    void onRead();
    bool regist(QString name, QScriptValue method,  QScriptValue obj);
    QVariant call(QString method,QString to,QVariant data=QVariant(),bool noRespond=false);
    void updateNet();
    void setGroupId(int gid);
    QString getFromIp();
    QString getLocalIp();
    QString getFromMac();
    QString getLoaclMac();
};
extern GroupRPC *groupRPC;
#endif // GROUPRPC_H
