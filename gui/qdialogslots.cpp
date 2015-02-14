#include "qdialogslots.h"
#include "qsubchkconnection.h"
#include "qsubchkupdate.h"

#include <QDesktopServices>

QDialogSlots::QDialogSlots(QWidget *parent) :
    QDialogDaemon(parent)
{
    this->set_languages();

    //init item which don't need refresh after change languages
    this->set_platform();
    this->mirrors_ = CommonUtil::set_network();
    this->set_mirrors();
    this->set_version();
}

QDialogSlots::~QDialogSlots()
{

}

/**
 * @brief Close this program while the reject signal is emitted.
 */
void QDialogSlots::reject()
{
    close();
    return QDialogDaemon::reject();
}

/**
 * @brief Close this program while the close signal is emitted.
 */
bool QDialogSlots::close()
{
    return QDialogDaemon::close();
}

/**
 * @brief Change the current server selection.
 * @param mirr_id  Index number of current mirror server.
 */
void QDialogSlots::on_Mirror_changed(int mirror_id)
{
    mirror_id_ = mirror_id;
    check_connection();
}

/**
 * @brief Change the current IP version setting.
 * @param ipv_id  An flag indicating current IP version setting. The value could be 1 or 0.
 */
void QDialogSlots::on_IPVersion_changed(int ipv_id)
{
    if(ipv_id != ipv_id_){
        ipv_id_ = ipv_id;
        if(!redata_->db_exists()){
            warning_no_datafile();
        }else{
            set_func_list(0);
            refresh_func_list();
        }
    }
}

/**
 * @brief Change the current selection of modules to be applied to hosts file.
 * @param item  Row number of the item listed in Functionlist which is
        changed by user.
 */
void QDialogSlots::on_Selection_changed(QListWidgetItem *item)
{
    int ip_flag = ipv_id_;
    int func_id = ui->Functionlist->row(item);
    if(funcs_[ip_flag][func_id] == 0){
        funcs_[ip_flag][func_id] = 1;
    }else{
        funcs_[ip_flag][func_id] = 0;
    }
    QList<int> mutex = redata_->get_ids(choice_[ip_flag][func_id][2].toInt());
    for(int i=0; i<choice_[ip_flag].size(); i++){
        int c_id = i;
        QList<QVariant> c = choice_[ip_flag][i];
        if(c[0].toInt() == choice_[ip_flag][func_id][0].toInt()){
            //check mutex,like goole us and google hk.
            if(mutex.indexOf(c[1].toInt()) >= 0 && funcs_[ip_flag][c_id] == 1){
                funcs_[ip_flag][c_id] = 0;
            }
        }
    }
    refresh_func_list();
}

/**
 * @brief Change the UI language setting.
 * @param lang  The language name which is selected by user.
 */
void QDialogSlots::on_Lang_changed(QString lang)
{
    qDebug()<<"languages"<<lang;
    if(lang == "Chinese(simplified)"){
        trans_.load(":/lang/zh_CN.qm");
        QApplication::installTranslator(&trans_);
        ui->retranslateUi(this);
    }else if(lang == "English"){
        trans_.load(":/lang/en_US.qm");
        QApplication::installTranslator(&trans_);
        ui->retranslateUi(this);
    }
    refresh_main();
    check_connection();
}

/**
 * @brief Start operations to make a hosts file.
 */
void QDialogSlots::on_MakeHosts_clicked()
{
    QString filename = backfile_;
    if (!QFile::exists(filename)){
        this->info_backup();
        return;
    }

    if(!writable_){
        warning_permission();
        return;
    }
    if(question_apply()){
        make_hosts("system");
    }else{
        return;
    }
}

/**
 * @brief Export a hosts file encoded in ANSI.
 */
void QDialogSlots::on_MakeANSI_clicked()
{
    make_path_ = export_hosts();
    if(make_path_ != ""){
        make_hosts("ansi");
    }

}

/**
 * @brief Export a hosts file encoded in UTF-8.
 */
void QDialogSlots::on_MakeUTF8_clicked()
{
    make_path_ = export_hosts();
    if(make_path_ != ""){
        make_hosts("utf-8");
    }
}

/**
 * @brief Backup the hosts file of current operating system.
 */
void QDialogSlots::on_Backup_clicked()
{
    qDebug()<<111;
    QString filename = backfile_;
    if (QFile::exists(filename)){
        QFile::remove(filename);
    }
    if(QFile::copy(hosts_path_, filename))
        info_complete();

}

/**
 * @brief Restore a previously backed up hosts file.
 */
void QDialogSlots::on_Restore_clicked()
{
    if(!writable_){
        warning_permission();
        return;
    }
    QString filename = backfile_;
    if (!QFile::exists(filename)){
        this->info_nobackup();
        return;
    }
    if(CommonUtil::copyFile(filename, hosts_path_))
        info_complete();
}

/**
 * @brief Retrieve update information (metadata) of the latest data file from a
 * specified server.
 */
void QDialogSlots::on_CheckUpdate_clicked()
{
    if(choice_[0].size() != 0){
        refresh_func_list();
        set_update_click_btns();
    }
    if(update_.size() == 0 || update_["version"] == QApplication::translate("Util", "[Error]")){
        check_update();
    }
}

/**
 * @brief Retrieve the latest hosts data file.
 */
void QDialogSlots::on_FetchUpdate_clicked()
{
    set_fetch_click_btns();

    //set down flag to fetch_update_after_check
    down_flag_ = 1;

    if(update_.size() == 0 || update_["version"] == QApplication::translate("Util", "[Error]")){
        check_update();
    }else if(new_version()){
        fetch_update();
    }else{
        info_uptodate();
        finish_fetch();
    }
}

/**
 * @brief Open external link in browser.
 */
void QDialogSlots::on_LinkActivated(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}
