#ifndef QDIALOGUI_H
#define QDIALOGUI_H
#include "hoststool.h"
#include "ui_util_ui.h"

#include <QListWidgetItem>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QJsonObject>


namespace Ui {
class Util;
}

/**
 * CursesUI class contains methods to draw the Graphical User Interface (GUI)
 */
class QDialogUI : public QDialog
{
    Q_OBJECT

public:
    explicit QDialogUI(QWidget *parent = 0);
    virtual ~QDialogUI();

public:
    void set_stylesheet();
    void set_languages();
    void set_mirrors();
    void set_platform_label();
    void set_label_color(QLabel *label, QString color);
    void set_label_text(QLabel *label, QString text);

    void set_version();
    void set_info();

    void set_func_list(int new_ = 0);
    void set_list_item_unchecked(int item_id);
    void refresh_func_list();

    void set_message(QString title, QString message);
    void set_make_message(QString message, int start = 0);
    void warning_permission();
    void warning_download();
    void warning_incorrect_datafile();
    void warning_no_datafile();
    bool question_apply();
    void info_uptodate();
    void info_complete();
    void info_nobackup();
    void info_backup();
    void set_make_start_btns();
    void set_make_finish_btns();
    void set_update_click_btns();
    void set_update_start_btns();
    void set_update_finish_btns();
    void set_fetch_click_btns();
    void set_fetch_start_btns();
    void set_fetch_finish_btns(int error = 0);


    QString infofile_;
    QString filename_;
    /**
     * File name of User Customized Hosts File. Customized
     * hosts would be able to select if this file exists. The default file
     * name is ``custom.hosts``
     */
    QString custom_;
    /**
     * Dictionaries containing `tag`, `test url`, and `update url` of mirror servers.
     */
    std::vector<std::map<QString, QString> >  mirrors_;
    /**
     * Index number of current selected server from the
     */
    int mirror_id_;
    /**
     * Platform of current operating system.
     */
    QString platform_;
    /**
     * A flag indicating whether current operating system is supported or not.
     */
    bool plat_flag_;
    /**
     * An flag indicating current IP version setting. The value could be 1 or 0: 1 ipv4 0 ipv4;
     */
    int ipv_id_;
    /**
     * Current version of local hosts data file.
     */
    QString cur_ver_;

    /**
     * Two lists with the selection of functions both for IPv4 and IPv6 environment.
     */
    QList<QList<QVariant> > choice_[2];
    /**
     * Two lists with integers indicating the number of function items
     * from different parts listed in the function list.
     */
    QList<int> slices_[2];
    /**
     * Two lists with the information of function list both for IPv4 and IPv6 environment.
     */
    QList<int> funcs_[2];

    /**
     * Update information of the current data file on server.
     */
    QJsonObject update_;

    /**
     * The hostname of current operating system.
     */
    QString hostname_;
    /**
     * The absolute path to the hosts file on current operating system.
     */
    QString hosts_path_;
    /**
     * The End-Of-Line marker. This maker could typically be one of `CR`, `LF`, or `CRLF`.
     */
    QString sys_eol_;
    /**
     * Indicating whether the program is run with admin/root privileges.
     */
    int writable_;
    /**
     * An flag indicating the downloading status of current session.
     * 1 represents data file is being downloaded.
     */
    int down_flag_;
    /**
     * Operation mode for making hosts file. The valid value could be one of `system`, `ansi`, and `utf-8`.
     */
    QString make_mode_;
    /**
     * A set of module selection control bytes used to
     * control whether a specified method is used or not while generate a
     * hosts file.
     */
    std::map<int, int> make_cfg_;
    /**
     * Temporary path to store generated hosts file. The
     * default value of `make_path` is "`./hosts`".
     */
    QString make_path_;
    /**
     * languages
     */
    QTranslator trans_;

    Ui::Util *ui;

protected:


//slot
public slots:
    void set_conn_status(int status, int mirror_id = -1);
    void set_down_progress(int progress, QString message);
    void set_make_progress(QString mod_name, int mod_num);

    virtual void on_Mirror_changed(int mirr_id) = 0;
    virtual void on_IPVersion_changed(int ipv_id) = 0;
    virtual void on_Selection_changed(QListWidgetItem *item) = 0;
    virtual void on_Lang_changed(QString lang) = 0;
    virtual void on_MakeHosts_clicked() = 0;
    virtual void on_MakeANSI_clicked() = 0;
    virtual void on_MakeUTF8_clicked() = 0;
    virtual void on_Backup_clicked() = 0;
    virtual void on_Restore_clicked() = 0;
    virtual void on_CheckUpdate_clicked() = 0;
    virtual void on_FetchUpdate_clicked() = 0;
    virtual void on_LinkActivated(QString url) = 0;
};

#endif // QDIALOGUI_H
