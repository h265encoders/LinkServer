#include <QCoreApplication>
#include <QTextCodec>
#include "Server.h"
#include "Script.h"
#include "Link.h"
#include "GroupRPC.h"

Server *server;
Script *script;
GroupRPC *groupRPC;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    Link::init();
    server=new Server();
    script=new Script();
    groupRPC=new GroupRPC();
    groupRPC->init();
    script->init();
    server->init();
//    server->openLog();

    for(int i=1;i<argc;i++)
    {
        QString path=argv[i];
        script->include(path);
    }

    return a.exec();
}

