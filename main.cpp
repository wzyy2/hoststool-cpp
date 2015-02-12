#include "gui/qdialogslots.h"

#include <QApplication>
#include <QTranslator>
#include <QDebug>

#include "util/roottools.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDialogSlots q;

    //languages can only be set after set QApplication
    if(QLocale::system().name().toStdString() == "zh_CN"){
        q.trans_.load(":/lang/zh_CN.qm");
        QApplication::installTranslator(&(q.trans_));
        q.ui->retranslateUi(&q);
    }else{
        q.trans_.load(":/lang/en_US.qm");
        QApplication::installTranslator(&(q.trans_));
        q.ui->retranslateUi(&q);
    }
    q.refresh_main();
    //Check if current session have root privileges
    q.check_writable();

    RootTools::Instance();

#if (defined USE_MOBILE)
    q.showMaximized();
#else
    q.showNormal();
#endif


    return a.exec();
}


