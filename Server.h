#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include "maiaXmlRpcServer.h"
#include "Link.h"
#include <QMutex>
#include <iostream>
#include <QTimer>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void init();
    MaiaXmlRpcServer *rpcServer;

    void newLog(QString text);
    static void customMessageHandler(QtMsgType type,const QMessageLogContext&, const QString &msg);
    static QMutex mutexLog;
private:
    QString logString;
    QTimer timerKill;
signals:

public slots:
    QString createWithName(QString className, QString name);
    QString create(QString className);
    bool startWithData(QString name, QVariantMap data);
    bool start(QString name);
    bool setData(QString name, QVariantMap data);
    bool linkA(QVariantList nameList);
    bool linkV(QVariantList nameList);
    bool unLinkA(QVariantList nameList);
    bool unLinkV(QVariantList nameList);
    QString getState(QString name);
    QVariantMap getData(QString name);
    QVariantMap getUserData(QString name);
    QVariant invoke(QString name, QString func, QVariant value);
    bool stop(QString name);
    bool stopSync(QString name);
    bool destroy(QString name);
    QString getLog();
    bool LOG(QString str);
    bool openLog();
    bool reset();
    void onTimerKill();
};
extern Server *server;
#endif // SERVER_H
