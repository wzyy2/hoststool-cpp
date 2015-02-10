#include "qsubfetchupdate.h"
#include "util/commonutil.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>


QSubFetchUpdate::QSubFetchUpdate(QDialogUI *parent):
    QThread(parent)
{
    url_ = parent->mirrors_[parent->mirror_id_]["update"] + parent->filename_;
    path_ = QString("./") + parent->filename_;
    tmp_path_ = path_ + ".download";
    filesize_ = parent->update_["size"].toDouble();

    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QSubFetchUpdate::~QSubFetchUpdate()
{

}

//Start operations to retrieve the new hosts data file.
void QSubFetchUpdate::run()
{
    emit prog_trigger(0, QApplication::translate("Util", "Connecting..."));
    fetch_file();
}

//Retrieve the latest data file to a specified local path with a url.
void QSubFetchUpdate::fetch_file()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;
    QTimer timer;
    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    // the HTTP request
    QUrl u(url_);
    QNetworkRequest req(u);
    timer.start(120 * 1000); //120s
    QNetworkReply *reply = mgr.get(req);

    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                     SLOT(set_progress(qint64,qint64)));

    eventLoop.exec(); // blocks stack until "finished()" has been called

    if(timer.isActive()) {
        if (reply->error() == QNetworkReply::NoError) {
            //success
            QByteArray  ret = reply->readAll();
            QFile dstFile(tmp_path_);
            if (!dstFile.open(QIODevice::WriteOnly))
                throw QString("io");
            dstFile.write(ret);
            dstFile.close();
            replace_old();
            delete reply;
            emit finish_trigger(1, 0);
            return;
        }
        else {
            //failure
            qDebug() << "Failure" << reply->errorString();
            delete reply;
        }
    }else{
        //timeout
        disconnect(&mgr, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        reply->abort();
        delete reply;
    }

    emit finish_trigger(1, 1);
}

//Send message to the main dialog to set the progress bar.
void QSubFetchUpdate::set_progress(qint64 bytesReceived,qint64 bytesTotal)
{
    float prog = 100 * bytesReceived / bytesTotal;
    QString done = CommonUtil::convert_size(bytesReceived);
    QString total = CommonUtil::convert_size(bytesTotal);
    QString text = QApplication::translate("Util", "Downloading: ");
    text = text + done + " / " + total;
    emit prog_trigger(prog, text);
}

//Replace the old hosts data file with the new one.
void QSubFetchUpdate::replace_old()
{
    if(QFile::exists(path_)){
        QFile::remove(path_);
    }
    QFile::rename(tmp_path_, path_);
}
