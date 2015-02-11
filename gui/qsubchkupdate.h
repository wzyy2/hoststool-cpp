#ifndef QSUBCHKUPDATE_H
#define QSUBCHKUPDATE_H
#include "hoststool.h"
#include "qdialogui.h"

#include <QThread>
#include <QNetworkReply>

class QSubChkUpdate : public QThread
{
    Q_OBJECT

public:
    QSubChkUpdate(QDialogUI *parent = 0);
    ~QSubChkUpdate();

protected:
    QString url_;
    void run();

signals:
    void trigger(QJsonObject update);
};

#endif // QSUBCHKUPDATE_H
