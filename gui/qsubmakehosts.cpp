#include "qsubmakehosts.h"


/**
 * Initialize a new instance of this class. Retrieve configuration from
 * the main dialog to make a new hosts file.
 */
QSubMakeHosts::QSubMakeHosts(QDialogUI *parent) : QThread(parent)
{
//    QtCore.QThread.__init__(self, parent)
    make_host_ = new MakeHosts(parent->make_cfg_, parent->hostname_,
                               parent->custom_, parent->make_path_,
                               parent->make_mode_, parent->sys_eol_);
}

QSubMakeHosts::~QSubMakeHosts()
{
    delete make_host_;
}

/**
 * @brief Start operations to retrieve data from the data file and generate new
 * hosts file.
 */
void QSubMakeHosts::run()
{
    QDateTime start_time = QDateTime::currentDateTime();

    //make hosts
    make_host_->make();

    QDateTime end_time = QDateTime::currentDateTime();
    //secs
    float total_time = start_time.msecsTo(end_time);
    total_time /= 1000;

    char a[100];
    sprintf(a, "%.4f", total_time);
    QString time = a;
    emit fina_trigger(time, make_host_->count);
    if(make_host_->make_mode == "system")
        emit move_trigger();
}
