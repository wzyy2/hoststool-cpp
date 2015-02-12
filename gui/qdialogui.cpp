#include "qdialogui.h"
#include "version.h"


QDialogUI::QDialogUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Util)
{
    mirror_id_ = 0;
    ipv_id_ = 0;
    plat_flag_ = true;
    writable_ = 0;
    down_flag_ = 0;
    make_path_ = "./hosts";
    infofile_ = "hostsinfo.json";
    filename_ = "hostslist.data";
    custom_ = "custom.hosts";

    //Set default UI language
    ui->setupUi(this);
    this->set_stylesheet();

    redata_ = RetrieveData::Instance();
}

QDialogUI::~QDialogUI()
{
    delete ui;
    RetrieveData::Destroy();
}


/**
 *@brief Set the style sheet of main dialog.
 */
void QDialogUI::set_stylesheet()
{
    QString qss;
    QFile qssFile(":/qss/theme/default.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen())
    {
        qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);
        qssFile.close();
    }
}

/**
 *@brief Set optional language selection items in the SelectLang widget.
 */
void QDialogUI::set_languages()
{
    ui->SelectLang->clear();
    ui->SelectLang->blockSignals(true);
    ui->SelectLang->addItem("English");
    ui->SelectLang->addItem("Chinese(simplified)");
    if(QLocale::system().name().toStdString() == "zh_CN"){
        ui->SelectLang->setCurrentIndex(1);
    }
    ui->SelectLang->blockSignals(false);
}

/**
 *@brief Set optional server list.
 */
void QDialogUI::set_mirrors()
{
    for(unsigned int i = 0; i < mirrors_.size(); i++){
        ui->SelectMirror->addItem(QString::fromUtf8(""));
        ui->SelectMirror->setItemText(
                    i, QApplication::translate("Util", mirrors_[i]["tag"].toStdString().c_str()) );
    }

}

/**
 *@brief Set the information of the label indicating current operating system platform.
 */
void QDialogUI::set_platform_label()
{
    QString color;
    if(this->plat_flag_){
        color = "GREEN";
    }else{
        color = "RED";
    }
    set_label_color(ui->labelOSStat, color);
    set_label_text(ui->labelOSStat, QString("[%1]").arg(platform_));
}

/**
 * @brief Set the `color` of a `label`.
 * @param label  Label on the main dialog.
 * @param color  Color to be set on the label.
 */
void QDialogUI::set_label_color(QLabel *label, QString color)
{
    QString rgb;
    if(color == "GREEN")
        rgb = "#37b158";
    else if(color == "RED")
        rgb = "#e27867";
    else if(color == "BLACK")
        rgb = "#b1b1b1";
    else
        rgb = "#ffffff";
    label->setStyleSheet(QString("QLabel {color: %1}").arg(rgb));
}


/**
 * @brief Set the `text` of a `label`.
 * @param label  Label on the main dialog.
 * @param text  Message to be set on the label.
 */
void QDialogUI::set_label_text(QLabel *label, QString text)
{
    label->setText(QApplication::translate("Util", text.toStdString().c_str()));
}

/**
 * @brief Set the information of connection status to the current server selected.
 */
void QDialogUI::set_conn_status(int status, int mirror_id)
{
    QString color,stat;
    if(this->mirror_id_ == mirror_id || mirror_id == -1){
        if(status == -1){
            set_label_color(ui->labelConnStat, "BLACK");
            set_label_text(ui->labelConnStat, QApplication::translate("Util", "Checking..."));
        }else if(status == 0 || status == 1){
            if(status){
                color = "GREEN";
                stat = QApplication::translate("Util", "[OK]");
            }else{
                color = "RED";
                stat = QApplication::translate("Util", "[Failed]");
            }
            set_label_color(ui->labelConnStat, color);
            set_label_text(ui->labelConnStat, stat);
        }
    }
}

/**
 * @brief Set the version.
 */
void QDialogUI::set_version()
{
    QString version = QString("v") + QString(__version__) + QString(" ") + QString(__release__);
    set_label_text(ui->VersionLabel, version.toStdString().c_str());
}

/**
 * @brief Set the information of the current local data file.
 */
void QDialogUI::set_info()
{
    std::map<QString, QString> info;
    info = redata_->get_info();
    QString ver = info["Version"];
    cur_ver_ = ver;
    set_label_text(ui->labelVersionData, ver);
    QString build = info["Buildtime"];
    build = CommonUtil::timestamp_to_date(build);
    set_label_text(ui->labelReleaseData, build);
}

/**
 * @brief `progress` position of the progress bar with a `message`.
 * @param progress  Progress position to be set on the progress bar.
 * @param message  Message to be set on the progress bar.
 */
void QDialogUI::set_down_progress(int progress, QString message)
{
    ui->Prog->setProperty("value", progress);
    set_conn_status(1);
    ui->Prog->setFormat(message);
}

/**
 * @brief Draw the function list and decide whether to load the default selection configuration or not.
 * @param new_  A flag indicating whether to load the default selection configuration or not. Default value is `0`.
 * 0    Use user config.
 * 1    Use default config.
 */
void QDialogUI::set_func_list(int new_)
{
    ui->Functionlist->clear();
    ui->FunctionsBox->setTitle(QApplication::translate("Util", "Functions"));

    //if new,the data will be load from database,or we just use data from class member
    if(new_){
        for(int ip=0; ip<2; ip++){
            QList<QList<QVariant> > choice;
            std::map<int, QList<int> > defaults;
            QList<int> slices;
            redata_->get_choice(ip?true:false, choice, defaults, slices);
            if(QFile::exists(custom_)){
                QList<QVariant> a;
                a.append(4);
                a.append(1);
                a.append(0);
                a.append("customize");
                choice.insert(0, a);
                QList<int> b;
                b.push_back(1);
                defaults[0x04] = b;
                for(int i=0; i<slices.size(); i++){
                    slices[i] += 1;
                }
                slices.insert(0, 0);
            }
            this->choice_[ip] = choice;
            this->slices_[ip] = slices;
            QList<int> funcs;
            for(int i=0; i<choice.size(); i++){
                int id = (choice[i][0].toInt());
                if(defaults[id].indexOf(choice[i][1].toInt()) >= 0){
                    funcs.append(1);
                }else{
                    funcs.append(0);
                }
            }
            funcs_[ip] = funcs;
        }
    }
}

/**
 * @brief Set a specified item to become unchecked in the function list.
 * @param item_id  Index number of a specified item in the function list.
 */
void QDialogUI::set_list_item_unchecked(int item_id)
{
    funcs_[ipv_id_][item_id] = 0;
    QListWidgetItem *item = ui->Functionlist->item(item_id);
    item->setCheckState(Qt::Unchecked);
}

/**
 * @brief Refresh the items in the function list by user settings.
 */
void QDialogUI::refresh_func_list()
{
    int ip_flag = ipv_id_;
    ui->Functionlist->clear();
    for(int f_id=0; f_id<choice_[ip_flag].size(); f_id++){
        QListWidgetItem *item = new QListWidgetItem;
        if(funcs_[ip_flag][f_id] == 1){
            item->setCheckState(Qt::Checked);
        }else{
            item->setCheckState(Qt::Unchecked);
        }
        item->setText(QApplication::translate("Util", choice_[ip_flag][f_id][3].toString().toStdString().c_str()));
        ui->Functionlist->addItem(item);
    }
}

/**
 * @brief Start operations to show progress while making hosts file.
 * @param mod_name  Tag of a specified hosts module in current progress.
 * @param mod_num  Number of current module in the operation sequence.
 */
void QDialogUI::set_make_progress(QString mod_name, int mod_num)
{
    int total_mods_num = funcs_[ipv_id_].count(1) + 1;
    int prog = 100 * mod_num / total_mods_num;
    ui->Prog->setProperty("value", prog);
    QString module = mod_name;//_translate
    QString message = QApplication::translate("Util", "Applying module: %1(%2/%3)"
                                              ).arg(module).arg(mod_num).arg(total_mods_num);
    ui->Prog->setFormat(message);
    set_make_message(message);
}

/**
 * @brief Show a message box with a `message` and a `title`.
 * @param title  Title of the message box to be displayed.
 * @param message  Message in the message box.
 */
void QDialogUI::set_message(QString title, QString message)
{
    ui->FunctionsBox->setTitle(QApplication::translate("Util", title.toStdString().c_str()));
    ui->Functionlist->clear();
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(message);
    item->setFlags(Qt::ItemIsEnabled);
    ui->Functionlist->addItem(item);
}

/**
 * @brief List message for the current operating progress while making the new
 * hosts file in function list.
 * @param message  Message to be displayed in the function list.
 * @param start  A flag indicating whether the message is the first one
 * in the making progress or not. Default value is `0`.
 */
void QDialogUI::set_make_message( QString message, int start)
{
    if(start){
        ui->FunctionsBox->setTitle(QApplication::translate("Util", "Progress"));
        ui->Functionlist->clear();
    }
    QListWidgetItem *item = new QListWidgetItem();
    QString text = QString("- ") + message;
    item->setText(text.toStdString().c_str());
    item->setFlags(Qt::ItemIsEnabled);
    ui->Functionlist->addItem(item);
}

/**
 * @brief Show permission error warning message box.
 */
void QDialogUI::warning_permission()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Progress"),
                         QApplication::translate("Util", "You do not have permissions to change the \n"
                                                         "hosts file.\n"
                                                         "Please run this program as Administrator/root\n"
                                                         "so it can modify your hosts file."));
    exit(0);
}

/**
 * @brief Show download error warning message box.
 */
void QDialogUI::warning_download()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Warning"),
                         QApplication::translate("Util", "Error retrieving data from the server.\n"
                                                         "Please try another server."));

}

/**
 * @brief Show incorrect data file warning message box.
 */
void QDialogUI::warning_incorrect_datafile()
{
    QString msg_title = "Warning";
    QString msg = QApplication::translate("Util",  "Incorrect Data file!\n"
                                                   "Please use the \"Download\" key to \n"
                                                   "fetch a new data file.");
    set_message(msg_title, msg);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
}

/**
 * @brief Show no data file warning message box.
 */
void QDialogUI::warning_no_datafile()
{
    QString msg_title = "Warning";
    QString msg = QApplication::translate("Util", "Data file not found!\n"
                                                  "Please use the \"Download\" key to \n"
                                                  "fetch a new data file.");
    set_message(msg_title, msg);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
}

/**
 * @brief Show confirm question message box before applying hosts file.
 * @return A flag indicating whether user has accepted to continue the operations or not.
 */
bool QDialogUI::question_apply()
{
    QString msg_title = QApplication::translate("Util", "Notice");
    QString msg = QApplication::translate("Util", "Are you sure you want to apply changes \n"
                                                  "to the hosts file on your system?\n\n"
                                                  "This operation could not be reverted if \n"
                                                  "you have not made a backup of your \n"
                                                  "current hosts file.");

    int choice = QMessageBox::question(this, msg_title, msg,
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No
                                       );
    if(choice == QMessageBox::Yes)
        return true;
    else
        return false;
}

/**
 * @brief Draw data file is up-to-date message box.
 */
void QDialogUI::info_uptodate()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Notice"),
                         QApplication::translate("Util", "Data file is up-to-date."));
}

/**
 * @brief Draw operation complete message box.
 */
void QDialogUI::info_complete()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Complete"),
                         QApplication::translate("Util", "Operation completed."));
}

/**
 * @brief Draw no backup message box.
 */
void QDialogUI::info_nobackup()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Notice"),
                         QApplication::translate("Util", "No backup found."));
}

/**
 * @brief Draw no backup message box.
 */
void QDialogUI::info_backup()
{
    QMessageBox::warning(this, QApplication::translate("Util", "Notice"),
                         QApplication::translate("Util", "Backup before apply changes."));
}

/**
 * @brief Set button status while making hosts operations started.
 */
void QDialogUI::set_make_start_btns()
{
    ui->Functionlist->setEnabled(false);
    ui->SelectIP->setEnabled(false);
    ui->ButtonCheck->setEnabled(false);
    ui->ButtonUpdate->setEnabled(false);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
    ui->ButtonExit->setEnabled(false);
}

/**
 * @brief Set button status while making hosts operations finished.
 */
void QDialogUI::set_make_finish_btns()
{
    ui->Functionlist->setEnabled(true);
    ui->SelectIP->setEnabled(true);
    ui->ButtonCheck->setEnabled(true);
    ui->ButtonUpdate->setEnabled(true);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
    ui->ButtonExit->setEnabled(true);
}

/**
 * @brief Set button status while `CheckUpdate` button clicked.
 */
void QDialogUI::set_update_click_btns()
{
    ui->ButtonApply->setEnabled(true);
    ui->ButtonANSI->setEnabled(true);
    ui->ButtonUTF->setEnabled(true);
}

/**
 * @brief Set button status while operations to check update of hosts data file started.
 */
void QDialogUI::set_update_start_btns()
{
    ui->SelectMirror->setEnabled(false);
    ui->ButtonCheck->setEnabled(false);
    ui->ButtonUpdate->setEnabled(false);
}

/**
 * @brief Set button status while operations to check update of hosts data file
 * finished.
 */
void QDialogUI::set_update_finish_btns()
{
    ui->SelectMirror->setEnabled(true);
    ui->ButtonCheck->setEnabled(true);
    ui->ButtonUpdate->setEnabled(true);
}

/**
 * @brief Set button status while `FetchUpdate` button clicked.
 */
void QDialogUI::set_fetch_click_btns()
{
    ui->Functionlist->setEnabled(false);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
}

/**
 * @brief Set button status while operations to retrieve hosts data file
 * started.
 */
void QDialogUI::set_fetch_start_btns()
{
    ui->SelectMirror->setEnabled(false);
    ui->ButtonCheck->setEnabled(false);
    ui->ButtonUpdate->setEnabled(false);
    ui->ButtonApply->setEnabled(false);
    ui->ButtonANSI->setEnabled(false);
    ui->ButtonUTF->setEnabled(false);
    ui->ButtonExit->setEnabled(false);
}

/**
 * @brief Set button status while operations to retrieve hosts data file
 * finished.
 * @param error A flag indicating if error occurs while retrieving hosts
 * data file from the server.
 */
void QDialogUI::set_fetch_finish_btns(int error)
{
    if(error){
        ui->ButtonApply->setEnabled(false);
        ui->ButtonANSI->setEnabled(false);
        ui->ButtonUTF->setEnabled(false);
    }else{
        ui->ButtonApply->setEnabled(true);
        ui->ButtonANSI->setEnabled(true);
        ui->ButtonUTF->setEnabled(true);
    }
    ui->Functionlist->setEnabled(true);
    ui->SelectMirror->setEnabled(true);
    ui->ButtonCheck->setEnabled(true);
    ui->ButtonUpdate->setEnabled(true);
    ui->ButtonExit->setEnabled(true);
}
