#ifndef QSUBCHKCONNECTION_H
#define QSUBCHKCONNECTION_H
#include "hoststool.h"
#include <QThread>
#include "qdialogui.h"

//QSubChkConnection is a subclass of :class:`PyQt4.QtCore.QThread`. This
//class contains methods to retrieve the metadata of the latest hosts data
//file.
class QSubChkConnection : public QThread
{
    Q_OBJECT

public:
    QSubChkConnection(QDialogUI *parent = 0);
    ~QSubChkConnection();

protected:
    QString link_;
    int mirror_id_;
    void run();

signals:
    void trigger(int status, int mirror_id);
};

#endif // QSUBCHKCONNECTION_H
