#include "qdialogdaemon.h"
#include "qsubchkconnection.h"
#include "qsubchkupdate.h"
#include "qsubfetchupdate.h"
#include "qsubmakehosts.h"
#include "util/commonutil.h"
#include "util/retrievedata.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFileDialog>

QDialogDaemon::QDialogDaemon(QWidget *parent) :
    QDialogUI(parent)
{

}

QDialogDaemon::~QDialogDaemon()
{

}

/**
 * @brief Check if current session is ran with root privileges.
 */
void QDialogDaemon::check_writable()
{
    QString username;
    bool flag;
    CommonUtil::check_privileges(username, flag);
    writable_ = flag?1:0;
    if(!writable_)
        warning_permission();
}

/**
 * @brief Operations to check the connection to current server.
 */
void QDialogDaemon::check_connection()
{
    QSubChkConnection *thread = new QSubChkConnection(this);
    connect(thread, SIGNAL(trigger(int, int)), this, SLOT(set_conn_status(int, int)) );
    thread->start();
}

/**
 * @brief Retrieve the metadata of the latest data file from a server.
 */
void QDialogDaemon::check_update()
{
    set_update_start_btns();
    set_label_text(ui->labelLatestData, QApplication::translate("Util", "Checking..."));
    QSubChkUpdate *thread = new QSubChkUpdate(this);
    connect(thread, SIGNAL(trigger(QJsonObject)), this, SLOT(finish_update(QJsonObject)) );
    thread->start();
}

/**
 * @brief Retrieve a new hosts data file from a server.
 */
void QDialogDaemon::fetch_update()
{
    set_fetch_start_btns();
    QSubFetchUpdate *thread = new QSubFetchUpdate(this);
    connect(thread, SIGNAL(prog_trigger(int, QString)), this, SLOT(set_down_progress(int, QString)) );
    connect(thread, SIGNAL(finish_trigger(int, int)), this, SLOT(finish_fetch(int, int)) );
    thread->start();
}

/**
 * @brief Decide whether to retrieve a new data file from server or not after
 * checking update information from a mirror.
 */
void QDialogDaemon::fetch_update_after_check()
{
    if(update_["version"] == QApplication::translate("Util", "[Error]")){
        finish_fetch(1, 0);
    }else if(new_version()){
        fetch_update();
    }else{
        info_uptodate();
        finish_fetch(1, 0);
    }
}

/**
 * @brief Display the export dialog and get the path to save the exported hosts
 * file.
 * @return Path to export a hosts file.
 */
QString QDialogDaemon::export_hosts()
{
    QString filename = "hosts";
    if(platform_ == "OS X"){
        //filename = "/Users/" + filename;
    }
    QString filepath = QFileDialog::getSaveFileName(this, QApplication::translate("Util", "Export hosts"),
                                                    filename, QApplication::translate("Util", "hosts File"));
    return filepath;
}

/**
 * @brief Make a new hosts file for current system.
 * @param mode   Operation mode for making hosts file. The valid value
 * could be one of `system`, `ansi`, and `utf-8`.
 * Default by `system`.
 */
void QDialogDaemon::make_hosts(QString mode)
{
    set_make_start_btns();
    set_make_message(QApplication::translate("Util", "Building hosts file..."), 1);
    //Avoid conflict while making hosts file
    RetrieveData::disconnect_db();
    make_mode_ = mode;
    set_config_bytes(mode);

    QSubMakeHosts *thread = new QSubMakeHosts(this);
    connect(thread, SIGNAL(info_trigger(QString, int)), this, SLOT(set_make_progress(QString, int)) );
    connect(thread, SIGNAL(fina_trigger(QString, int)), this, SLOT(finish_make(QString, int)) );
    connect(thread, SIGNAL(move_trigger()), this, SLOT(move_hosts()) );
    thread->start();
}

/**
 * @brief Move hosts file to the system path after making.c
 */
void QDialogDaemon::move_hosts()
{
    QString filepath = "hosts";
    QString msg = QApplication::translate("Util", "Copying new hosts file to\n %1").arg(hosts_path_);
    set_make_message(msg);
    if (QFile::exists(hosts_path_)){
        QFile::remove(hosts_path_);
    }
    if(!QFile::copy(filepath, hosts_path_)){
        warning_permission();
    }
    QFile::remove(filepath);
    msg = QApplication::translate("Util", "Operation completed");
    set_make_message(msg);
    info_complete();
}

/**
 * @brief Generate the module configuration byte words by the selection from
 * function list on the main dialog.
 * @param mode  Operation mode for making hosts file. The valid value
 * could be one of `system`, `ansi`, and `utf-8`.
 */
void QDialogDaemon::set_config_bytes(QString mode)
{
    //which part to select
    int ip_flag = ipv_id_;
    std::map<int, int> selection;
    int localhost_word;
    if(mode == "system"){
        std::map<QString, int> a;
        a["Windows"] = 0x0001;
        a["Linux"] = 0x0002;
        a["Unix"] = 0x0002;
        a["Android"] = 0x0002;
        a["OS X"] = 0x0004;
        a["IOS"] = 0x0004;
        localhost_word = a[platform_];
    }else{
        localhost_word = 0x0008;
    }
    //localhost
    selection[0x02] = localhost_word;
    QList<int> ch_parts;
    //hosts share
    ch_parts.push_back(0x08);
    //ipvx
    ch_parts.push_back(ip_flag?0x20:0x10);
    //hosts adblock
    ch_parts.push_back(0x40);
    //Set customized module if exists
    if(QFile::exists(custom_)){
        ch_parts.insert(0, 0x04);
    }
    for(int i=0; i<ch_parts.size(); i++){
        QList<int> part_cfg = funcs_[ip_flag].mid(
                    slices_[ip_flag][i], slices_[ip_flag][i+1] - slices_[ip_flag][i]);
        int part_word = 0;
        for(int j=0; j<part_cfg.size(); j++){
            part_word += part_cfg[j] << j;
        }
        selection[ch_parts[i]] = part_word;
        make_cfg_ = selection;
    }
}


/**
 * @brief Reload the data file information and show them on the main dialog. The
 * information here includes both metadata and hosts module info from the
 * data file.
 * @param refresh   A flag indicating whether the information on main
 * dialog needs to be reloaded or not. The value could be `0` or `1`.
 */
void QDialogDaemon::refresh_info(int refresh)
{
    //refresh_info
    if(refresh){
        RetrieveData::clear();
        try{
            RetrieveData::unpack();
            RetrieveData::connect_db();
            this->set_func_list(1);
            this->refresh_func_list();
            this->set_info();
        }catch(QString &a){
            if(a == "io"){
                this->warning_no_datafile();
            }else{
                this->warning_incorrect_datafile();
            }
        }
    }
}

/**
 * @brief Start operations after making new hosts file.
 * @param time  Total time uesd while generating the new hosts file.
 * @param count  Total number of hosts entries inserted into the new hosts file.
 */
void QDialogDaemon::finish_make(QString time, int count)
{
    set_make_finish_btns();
    //due to disconnect to avoid conflict
    RetrieveData::connect_db();
    set_make_message(QApplication::translate("Util", "Notice: %1 hosts entries has \n"
                                             "been applied in %2 secs.").arg(count).arg(time));
    set_down_progress(100,  QApplication::translate("Util","Operation Completed Successfully!"));

}

/**
 * @brief Start operations after checking update.
 * @param update  Metadata of the latest hosts data file on the server.
 */
void QDialogDaemon::finish_update(QJsonObject update)
{
    update_ = update;
    set_label_text(ui->labelLatestData, update["version"].toString());
    if(update["version"].toString() == QApplication::translate("Util", "[Error]")){
        set_conn_status(0);
    }else{
        set_conn_status(1);
    }
    if(down_flag_){
        fetch_update_after_check();
    }else{
        set_update_finish_btns();
    }
}

/**
 * @brief Start operations after downloading data file.
 * @param refresh  An flag indicating whether the downloading progress is
 * successfully finished or not. Default by 1.
 * @param error  An flag indicating whether the downloading
 * progress is successfully finished or not. Default by 0.
 */
void QDialogDaemon::finish_fetch(int refresh, int error)
{
    down_flag_ = 0;
    if(error){
        //Error occurred while downloading
        set_down_progress(0, QApplication::translate("Util", "Error"));
        QFile::remove(filename_);
        warning_download();
        QString msg_title("Warning");
        QString msg = QApplication::translate("Util", "Incorrect Data file!\n"
                                      "Please use the \"Download\" key to \n"
                                      "fetch a new data file.");
        set_message(msg_title, msg);
        set_conn_status(0);
    }else{
        //Data file retrieved successfully
        set_down_progress(100, QApplication::translate("Util", "Download Complete"));
        refresh_info(refresh);
    }
    set_fetch_finish_btns(error);
}

/**
 * @brief Compare version of local data file to the version from the server.
 * @return  A flag indicating whether the local data file is up-to-date or not.
 */
int QDialogDaemon::new_version( )
{
    QString local_ver = cur_ver_;
    QString server_ver = update_["version"].toString();

    QStringList local_tmp = local_ver.split('.');
    QStringList server_tmp = server_ver.split('.');
    for(int i=0; i<local_tmp.size(); i++){
        if(server_ver[i] > local_ver[i])
            return 1;
    }
    return 0;
}

/**
 * @brief Set the information of current operating system platform.
 */
void QDialogDaemon::set_platform()
{
    CommonUtil::platform_struct p = CommonUtil::check_platform();
    platform_ = p.system;
    hostname_ = p.hostname;
    hosts_path_ = p.path;
    plat_flag_ = p.flag;
    if(p.encode == "win_ansi")
        sys_eol_ = "\r\n";
    else
        sys_eol_ = "\n";
}

/**
 * @brief Set up the elements on the main dialog.
 */
void QDialogDaemon::init_main()
{
    this->set_version();
    this->mirrors_ = CommonUtil::set_network();
    this->set_mirrors();
    //Read data file and set function list
    try{
        RetrieveData::unpack();
        RetrieveData::connect_db();
        this->set_func_list(1);
        this->refresh_func_list();
        this->set_info();
    }catch(QString &a){
        if(a == "io"){
            this->warning_no_datafile();
        }else{
            this->warning_incorrect_datafile();
        }
    }
}
