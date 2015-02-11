#ifndef QDIALOGDAEMON_H
#define QDIALOGDAEMON_H
#include "hoststool.h"
#include "qdialogui.h"

/**
 * QDialogDaemon class contains methods used to manage the operations while
 * modifying the hosts file of current operating system. Including methods
 * to manage operations to update data file, download data file, configure
 * hosts, make hosts file, backup hosts file, and restore backup.
 */
class QDialogDaemon : public QDialogUI
{
    Q_OBJECT

public:
    QDialogDaemon(QWidget *parent = 0);
    ~QDialogDaemon();

public slots:
    void set_platform();
    void check_writable();
    void check_connection();
    void check_update();
    void fetch_update();
    void fetch_update_after_check();
    QString export_hosts();
    void make_hosts(QString mode=  "system");
    void move_hosts();
    void set_config_bytes(QString mode);
    void refresh_info(int refresh=0);
    void finish_make(QString time, int count);
    void finish_update(QJsonObject update);
    void finish_fetch(int refresh = 1, int error = 0);
    int new_version();
    void refresh_main();

protected:


};

#endif // QDIALOGDAEMON_H
