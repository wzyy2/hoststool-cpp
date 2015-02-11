#ifndef QSUBMAKEHOSTS_H
#define QSUBMAKEHOSTS_H

#include "hoststool.h"
#include "qdialogui.h"
#include "util/makehosts.h"

#include <QThread>
#include <QNetworkReply>

/**
 * This class contains methods to make a
 * new hosts file for client.
 */
class QSubMakeHosts : public QThread
{
    Q_OBJECT

public:
    explicit QSubMakeHosts(QDialogUI *parent = 0);
    ~QSubMakeHosts();

signals:
    void info_trigger(QString, int);
    void fina_trigger(QString, int);
    void move_trigger();

protected:
    void run();
    MakeHosts *make_host_;
};

#endif // QSUBMAKEHOSTS_H
