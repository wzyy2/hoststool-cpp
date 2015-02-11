#ifndef QDIALOGSLOTS_H
#define QDIALOGSLOTS_H
#include "hoststool.h"
#include "qdialogdaemon.h"

/**
 * QDialogSlots class provides `Qt slots` to deal with the `Qt signals`
 * emitted by the widgets on the main dialog operated by users.
 */
class QDialogSlots : public QDialogDaemon
{
    Q_OBJECT

public:
    QDialogSlots(QWidget *parent = 0);
    ~QDialogSlots();

    void reject();
    bool close();

public slots:
    void on_Mirror_changed(int mirr_id);
    void on_IPVersion_changed(int ipv_id);
    void on_Selection_changed(QListWidgetItem *item);
    void on_Lang_changed(QString lang);
    void on_MakeHosts_clicked();
    void on_MakeANSI_clicked();
    void on_MakeUTF8_clicked();
    void on_Backup_clicked();
    void on_Restore_clicked();
    void on_CheckUpdate_clicked();
    void on_FetchUpdate_clicked();
    void on_LinkActivated(QString url);


};

#endif // QDIALOGSLOTS_H
