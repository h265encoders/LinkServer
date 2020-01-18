QT += core
QT -= gui

TARGET = LinkServer
CONFIG += console script
CONFIG -= app_bundle

TEMPLATE = app

chip=HI3536C
include(../LinkLib/Link.pri)
include(../libmaia-master/maia.pri)

DESTDIR = bin/$$chip/

SOURCES += main.cpp \
    Server.cpp \
    Script.cpp \
    GroupRPC.cpp

HEADERS += \
    Server.h \
    Script.h \
    GroupRPC.h

DISTFILES += \
    ../3536C/fs/link/script/main.js \
    ../3536C/fs/link/script/base.js \
    ../3536C/fs/link/script/net.js \
    ../3536C/fs/link/script/file.js \
    ../3536C/fs/link/script/group.js

