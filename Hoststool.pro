#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T02:30:49
#
#-------------------------------------------------

QT       += core gui network sql
android: QT  += androidextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hoststool
TEMPLATE = app


SOURCES += main.cpp\
    gui/qdialogui.cpp \
    gui/qdialogdaemon.cpp \
    util/commonutil.cpp \
    util/makehosts.cpp \
    util/retrievedata.cpp \
    gui/qsubchkupdate.cpp \
    gui/qsubchkconnection.cpp \
    gui/qdialogslots.cpp \
    gui/qsubfetchupdate.cpp \
    gui/qsubmakehosts.cpp \
    lib/quazip/zip.c \
    lib/quazip/unzip.c \
    lib/quazip/quazipnewinfo.cpp \
    lib/quazip/quazipfileinfo.cpp \
    lib/quazip/quazipfile.cpp \
    lib/quazip/quazipdir.cpp \
    lib/quazip/quazip.cpp \
    lib/quazip/quaziodevice.cpp \
    lib/quazip/quagzipfile.cpp \
    lib/quazip/quacrc32.cpp \
    lib/quazip/quaadler32.cpp \
    lib/quazip/qioapi.cpp \
    lib/quazip/JlCompress.cpp \
    util/roottools.cpp

HEADERS  += \
    gui/qdialogui.h \
    gui/qdialogdaemon.h \
    version.h \
    util/commonutil.h \
    util/makehosts.h \
    util/retrievedata.h \
    gui/qsubchkupdate.h \
    gui/qsubchkconnection.h \
    gui/qdialogslots.h \
    gui/qsubfetchupdate.h \
    gui/qsubmakehosts.h \
    lib/quazip/zip.h \
    lib/quazip/unzip.h \
    lib/quazip/quazipnewinfo.h \
    lib/quazip/quazipfileinfo.h \
    lib/quazip/quazipfile.h \
    lib/quazip/quazipdir.h \
    lib/quazip/quazip_global.h \
    lib/quazip/quazip.h \
    lib/quazip/quaziodevice.h \
    lib/quazip/quagzipfile.h \
    lib/quazip/quacrc32.h \
    lib/quazip/quachecksum32.h \
    lib/quazip/quaadler32.h \
    lib/quazip/JlCompress.h \
    lib/quazip/ioapi.h \
    lib/quazip/crypt.h \
    hoststool.h \
    util/roottools.h

FORMS    += \
    util_ui.ui

CONFIG += mobility
MOBILITY = 

RESOURCES += \
    res/util.qrc \
    res/style.qrc \
    res/conf.qrc \
    res/lang.qrc

DISTFILES += \
    android/res/values/libs.xml \
    android/build.gradle \
    android/AndroidManifest.xml \
    android/src/org/qtproject/RootHelper.java

TRANSLATIONS += en_US.ts \
    zh_CN.ts

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

INCLUDEPATH += $$PWD/

DEFINES += QUAZIP_STATIC#HOSTS_TEST
!android:  DEFINES += HOSTS_TEST

#Lib
!win32 : LIBS += -lz

win32:{
    DEFINES += WIN_ZLIB
}
