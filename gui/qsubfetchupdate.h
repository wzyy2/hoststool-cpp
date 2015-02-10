#ifndef QSUBFETCHUPDATE_H
#define QSUBFETCHUPDATE_H

#include "hoststool.h"
#include <QThread>
#include <QNetworkReply>
#include "qdialogui.h"

//This class contains methods to retrieve the latest hosts data file from a server.
class QSubFetchUpdate : public QThread
{
    Q_OBJECT

public:
    QSubFetchUpdate(QDialogUI *parent = 0);
    ~QSubFetchUpdate();

protected:
    QString url_;
    QString path_;
    QString tmp_path_;
    QString filesize_;
    void run();
    void fetch_file();
    void set_progress();
    void replace_old();

signals:
    void prog_trigger(int progress, QString update);
    void finish_trigger(int refresh, int error);

public slots:
    void set_progress(qint64 bytesReceived,qint64 bytesTotal);

};
#endif // QSUBFETCHUPDATE_H
