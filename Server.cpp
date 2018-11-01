#include "Server.h"
#include "Script.h"
QMutex Server::mutexLog;
Server::Server(QObject *parent) : QObject(parent)
{

}

void Server::init()
{
    rpcServer = new MaiaXmlRpcServer(8080,this);
    rpcServer->addMethod("Link.create", this, "create");
    rpcServer->addMethod("Link.createWithName", this, "createWithName");
    rpcServer->addMethod("Link.start", this, "start");
    rpcServer->addMethod("Link.startWithData", this, "startWithData");
    rpcServer->addMethod("Link.stop", this, "stop");
    rpcServer->addMethod("Link.stopSync", this, "stopSync");
    rpcServer->addMethod("Link.getState", this, "getState");
    rpcServer->addMethod("Link.setData", this, "setData");
    rpcServer->addMethod("Link.getData", this, "getData");
    rpcServer->addMethod("Link.getUserData", this, "getUserData");
    rpcServer->addMethod("Link.linkA", this, "linkA");
    rpcServer->addMethod("Link.linkV", this, "linkV");
    rpcServer->addMethod("Link.unlinkA", this, "unlinkA");
    rpcServer->addMethod("Link.unlinkV", this, "unlinkV");
    rpcServer->addMethod("Link.destroy", this, "destroy");
    rpcServer->addMethod("Link.openLog", this, "openLog");
    rpcServer->addMethod("Link.getLog", this, "getLog");
    rpcServer->addMethod("Link.LOG", this, "LOG");
    rpcServer->addMethod("Link.reset", this, "reset");
    rpcServer->addMethod("Link.invoke", this, "invoke");
    rpcServer->addMethod("Script.call", script, "call");

    timerKill.setSingleShot(true);
    connect(&timerKill,SIGNAL(timeout()),this,SLOT(onTimerKill()));

}

void Server::newLog(QString text)
{
    logString.append(text+"\n");
}

void Server::customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QMutexLocker locker(&mutexLog);
    QString txt;
    QDateTime tm;
    tm=QDateTime::currentDateTime();
    txt=tm.toString("hh:mm:ss ");
    if(type==QtDebugMsg)
        txt += QString("Debug: %1").arg(msg);
    else if(type==QtWarningMsg)
        txt += QString("Warning: %1").arg(msg);
    else if(type==QtCriticalMsg)
        txt += QString("Critical: %1").arg(msg);
    else if(type==QtFatalMsg)
        txt += QString("Fatal: %1").arg(msg);

    std::cout << txt.toStdString().data() << std::endl;
    server->newLog(txt);
}

QString Server::createWithName(QString className, QString name)
{
    return Link::create(className,name)->name();
}

QString Server::create(QString className)
{
    return Link::create(className)->name();
}

bool Server::startWithData(QString name, QVariantMap data)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    //qDebug()<<data;
    return obj->start(data);
}

bool Server::start(QString name)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    return obj->start();
}

bool Server::setData(QString name, QVariantMap data)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    return obj->setData(data);
}

bool Server::linkA(QVariantList nameList)
{
    if(nameList.count()<2)
        return false;
    LinkObject *lastObj=NULL;
    for(int i=0;i<nameList.count();i++)
    {
        LinkObject *obj=Link::find(nameList[i].toString());
        if(obj==NULL)
            return false;
        if(lastObj!=NULL)
            lastObj->linkA(obj);
        lastObj=obj;
    }
    return true;
}

bool Server::linkV(QVariantList nameList)
{
    if(nameList.count()<2)
        return false;
    LinkObject *lastObj=NULL;
    for(int i=0;i<nameList.count();i++)
    {
        LinkObject *obj=Link::find(nameList[i].toString());
        if(obj==NULL)
            return false;
        if(lastObj!=NULL)
            lastObj->linkV(obj);
        lastObj=obj;
    }
    return true;
}

bool Server::unLinkA(QVariantList nameList)
{
    if(nameList.count()<2)
        return false;
    LinkObject *lastObj=NULL;
    for(int i=0;i<nameList.count();i++)
    {
        LinkObject *obj=Link::find(nameList[i].toString());
        if(obj==NULL)
            return false;
        if(lastObj!=NULL)
            lastObj->unLinkA(obj);
        lastObj=obj;
    }
    return true;
}

bool Server::unLinkV(QVariantList nameList)
{
    if(nameList.count()<2)
        return false;
    LinkObject *lastObj=NULL;
    for(int i=0;i<nameList.count();i++)
    {
        LinkObject *obj=Link::find(nameList[i].toString());
        if(obj==NULL)
            return false;
        if(lastObj!=NULL)
            lastObj->unLinkV(obj);
        lastObj=obj;
    }
    return true;
}

QString Server::getState(QString name)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return "null";
    return obj->getState();
}

QVariantMap Server::getData(QString name)
{
    QVariantMap ret;
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return ret;
    ret=obj->getData();
    return ret;
}

QVariantMap Server::getUserData(QString name)
{
    QVariantMap ret;
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return ret;
    ret=obj->getUserData();
    return ret;
}

QVariant Server::invoke(QString name, QString func, QVariant value)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    //qDebug()<<"invok"<<name<<func<<value;


    return obj->invoke(func,value);
}

bool Server::stop(QString name)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    return obj->stop();
}

bool Server::stopSync(QString name)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    return obj->stop(true);
}

bool Server::destroy(QString name)
{
    LinkObject *obj=Link::find(name);
    if(obj==NULL)
        return false;
    obj->destroy();
    return true;
}

QString Server::getLog()
{
    QString ret=logString;
    logString="";
    return ret;
}

bool Server::LOG(QString str)
{
    qDebug()<<str;
    return true;
}

bool Server::openLog()
{
    qInstallMessageHandler(Server::customMessageHandler);
}

bool Server::reset()
{
    timerKill.start(500);
    return true;
}

void Server::onTimerKill()
{
    exit(0);
}

