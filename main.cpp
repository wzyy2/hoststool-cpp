#include "gui/qdialogslots.h"
#include <QApplication>
#include <QTranslator>
#include "util/commonutil.h"
#include "util/retrievedata.h"
#include <QDebug>
#include <QCoreApplication>
#include <QByteArray>
#include <QBitArray>
#include <QString>
#include <QDebug>
#include <QFile>

#include "lib/quazip/quazip.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDialogSlots *q = new QDialogSlots();

    //languages can only be set after set QApplication
    if(QLocale::system().name().toStdString() == "zh_CN"){
        q->trans_.load(":/lang/zh_CN.qm");
        QApplication::installTranslator(&(q->trans_));
        q->ui->retranslateUi(q);
    }else{
        q->trans_.load(":/lang/en_US.qm");
        QApplication::installTranslator(&(q->trans_));
        q->ui->retranslateUi(q);
    }
    q->init_main();
    //Check if current session have root privileges
    q->check_writable();

#if (defined Q_OS_IOS) || (defined Q_OS_ANDROID)
    q->showMaximized();
#else
    q->show();
#endif

    return a.exec();
}


