#include "qsubchkupdate.h"
#include "util/commonutil.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>

QSubChkUpdate::QSubChkUpdate(QDialogUI *parent):
    QThread(parent)
{
    url_ = parent->mirrors_[parent->mirror_id_]["update"] + parent->infofile_;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QSubChkUpdate::~QSubChkUpdate()
{

}

// Start operations to retrieve the metadata of the latest hosts data file.
void QSubChkUpdate::run()
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
    timer.start(5000);
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if(timer.isActive()) {
        if (reply->error() == QNetworkReply::NoError) {
            //success
            QString ret = reply->readAll();

            QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(ret.toUtf8(), &json_error);
            QJsonObject update_json;
            if(json_error.error == QJsonParseError::NoError)
            {
                update_json = parse_doucment.object();
            }else{
                //throw QString("recieved json data wrong!");
                update_json.insert(QString("version"), QApplication::translate("Util", "[Error]"));
            }

            emit trigger(update_json);
            delete reply;
            return;
        }
        else {
            //failure
            qDebug() << "Failure" <<reply->errorString();
            delete reply;
        }
    }else{
        //timeout
        disconnect(&mgr, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        reply->abort();
        delete reply;
    }
    QJsonObject json;
    json.insert(QString("version"), QApplication::translate("Util", "[Error]"));

    emit trigger(json);
}
