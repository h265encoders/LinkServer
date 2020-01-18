#ifndef SCRIPT_H
#define SCRIPT_H

#include <QObject>
#include <QScriptEngine>
#include <QMap>
#include <QMutex>
struct _timerData
{
    int id;
    int ms;
    bool singleshot;
    QString func;
};

class Script : public QObject
{
    Q_OBJECT
public:
    explicit Script(QObject *parent = 0);
    void init();
    void include(QString path);
    QScriptEngine engine;
    void timerEvent(QTimerEvent *e);
private:
    QMap<QString,QScriptValue> methodMap;
    QMap<QString,QScriptValue> thisMap;
    QMap<int, _timerData> timerData;
    QMutex jsLock;
signals:

public slots:
    QVariant loadConfig(QString path);
    bool saveConfig(QVariant data, QString path);
    QString readFile(QString path);
    bool regist(QString name,QScriptValue func, QScriptValue obj);
    bool connect(QString name, QScriptValue func, QScriptValue jsobj);
    void onNewEvent(QString type, QVariant args);
    QVariant call(QString method, QVariantList args);
    int setTimeout(QString func, int ms);
    int setInterval(QString func, int ms);
    int setTimer(QString func, int ms, bool singleshot=true);
    void stopTimer(int id);
    void sleep(int ms);
    void exec(QString cmd);

};
extern Script *script;
#endif // SCRIPT_H
