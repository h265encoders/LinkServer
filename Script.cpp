#include "Script.h"
#include <QDebug>
#include <QFile>
#include "Server.h"
#include "Json.h"
#include "unistd.h"
#include <QScriptValueIterator>
#include "GroupRPC.h"


Script::Script(QObject *parent) : QObject(parent)
{

}

void Script::init()
{
    QScriptValue self = engine.newQObject(this);
    engine.globalObject().setProperty("sys",self);
    QScriptValue link = engine.newQObject(server);
    engine.globalObject().setProperty("Link",link);
    QScriptValue group = engine.newQObject(groupRPC);
    engine.globalObject().setProperty("group",group);
    //engine.evaluate("function include(path){ sys.include(path); }");
    engine.evaluate("function setInterval(func,time){ sys.setInterval(func,time); }");
    engine.evaluate("function setTimeout(func,time){ sys.setTimeout(func,time); }");
    engine.evaluate("function clearInterval(id){ sys.stopTimer(id); }");
}

QVariant Script::loadConfig(QString path)
{
    if(!path.startsWith("/"))
        path="/link/config/"+path;
    return Json::loadFile(path);
}

bool Script::saveConfig(QVariant data, QString path)
{
    if(!path.startsWith("/"))
        path="/link/config/"+path;
    if(data.toMap().isEmpty())
    {
        QFile f(path);
        f.open(QFile::ReadWrite);
        f.resize(0);
        f.write(data.toString().toUtf8().data(),data.toString().toUtf8().length());
        f.close();
        return true;
    }
    else
        return Json::saveFile(data,path);
}

QString Script::readFile(QString path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QString str=file.readAll();
    file.close();
    return str;
}

bool Script::regist(QString name, QScriptValue func, QScriptValue obj)
{
    qDebug()<<"regist"<<name;
    methodMap[name]=func;
    thisMap[name]=obj;
    return true;
}

bool Script::connect(QString name, QScriptValue func, QScriptValue jsobj)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    regist(name, func,jsobj);
    QObject::connect(obj,SIGNAL(newEvent(QString,QVariant)),this,SLOT(onNewEvent(QString,QVariant)));
    return true;
}

void Script::onNewEvent(QString type, QVariant args)
{
    LinkObject *obj=(LinkObject *)sender();
    QVariantList list;
    list<<type;
    list<<args;
    call(obj->name(),list);
}


QVariant Script::call(QString method, QVariantList args)
{
    QScriptValueList list;
    foreach(QVariant v,args)
        list<<engine.toScriptValue(v);
    if(!methodMap.contains(method))
        return false;

    QScriptValue ret=methodMap[method].call(thisMap[method],list);
    if(engine.hasUncaughtException()){
        qDebug()<<engine.uncaughtException().toString();
    }
//    if(ret.isObject())
//    {
//        QVariantMap map;
//        QScriptValueIterator it(ret);
//        while (it.hasNext()) {
//            it.next();
//            qDebug() << it.name() << ": " << it.value().toVariant();
//            map[it.name()]=it.value().toVariant();
//        }
//        return map;

//    }
//    else
        return ret.toVariant();
}

int Script::setTimeout(QString func, int ms)
{
    return setTimer(func, ms, true);
}

int Script::setInterval(QString func, int ms)
{
    return setTimer(func, ms, false);
}

int Script::setTimer(QString func, int ms, bool singleshot)
{
    _timerData tmp;

        tmp.func = func;
        tmp.singleshot = singleshot;
        tmp.ms = ms;
        tmp.id = startTimer(ms);

        if ( tmp.id == 0 ) {
            qDebug() << "can not start timer!";
        }
    //    qDebug("setTimer:%d,%s",tmp.id,tmp.func.toLatin1().data());
        timerData.insert(tmp.id, tmp);
        return tmp.id;
}

void Script::stopTimer(int id)
{
    if(!timerData.contains(id))
            return;
    //    qDebug("stopTimer:%d",id);
        killTimer(id);
        timerData.remove(id);
}

void Script::sleep(int ms)
{
    usleep(ms*1000);
}

void Script::exec(QString cmd)
{
    system(cmd.toLatin1().data());
}


void Script::include(QString path)
{
    if(!path.startsWith("/"))
        path="/link/script/"+path;

    QFile file(path);
    file.open(QFile::ReadOnly);
    QString line;
    while(true)
    {
        line=file.readLine();
        if(line.startsWith("include"))
        {
//            include("func.js");
//            qDebug()<<line;
            int a=line.indexOf("\"");
            int b=line.indexOf("\"",a+1);
            QString path=line.mid(a+1,b-a-1);
            include(path);
            //engine.evaluate(line);
        }
        else
            break;
    }
    QString js=line+file.readAll();
    file.close();
    engine.evaluate(js);
    if(engine.hasUncaughtException()){
        qDebug()<<engine.uncaughtException().toString();
    }
}

void Script::timerEvent(QTimerEvent *e)
{
    _timerData timer = timerData.value(e->timerId());
       QScriptValue res;
       if ( timer.id != 0 ) {
//           qDebug() << QString("Timeout, evaluating: ") + timer.func;
           jsLock.lock();
           res = engine.evaluate(timer.func);
           jsLock.unlock();
           if ( res.isError() ) {
               qDebug() << res.toString();
           }
           if ( timer.singleshot ) {
               stopTimer(timer.id);
           }
       }
}


