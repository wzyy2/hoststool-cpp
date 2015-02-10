#include "qsubchkconnection.h"
#include "util/commonutil.h"

QSubChkConnection::QSubChkConnection(QDialogUI *parent):
    QThread(parent)
{
    mirror_id_ = parent->mirror_id_;
    link_ = parent->mirrors_[parent->mirror_id_]["test_url"];
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QSubChkConnection::~QSubChkConnection()
{

}

//Start operations to check the network connection with a specified server.
void QSubChkConnection::run()
{
    emit trigger(-1, mirror_id_);
    int status = CommonUtil::check_connection(link_);
    emit trigger(status, mirror_id_);
}
