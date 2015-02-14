#include "gui/qdialogslots.h"

#include <QApplication>
#include <QTranslator>
#include <QInputDialog>
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

#if (defined USE_MOBILE)
    q.showFullScreen();
#else
    q.showNormal();
#endif

#ifdef ROOTTOOL_NEED
#if (defined PASSWORD_NEED)
    QInputDialog passbox;
    bool isok;
    QString passwd = passbox.getText(NULL, QApplication::translate("Util", "Password"),
                                         QApplication::translate("Util", "Please input your root password,defalut alpine."),
                                         QLineEdit::Normal,
                                         "alpine",
                                     &isok);
    if(!isok)
        exit(0);
    //ios need password for su
    RootTools::Configure(passwd, true);
#endif
    RootTools::Instance();
#endif
    //Check if current session have root privileges
    q.check_writable();

    return a.exec();
}


