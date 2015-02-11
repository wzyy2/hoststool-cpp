#include "makehosts.h"
#include "retrievedata.h"

MakeHosts::MakeHosts(std::map<int, int> make_cfg_, QString hostname_, QString custom_,
                     QString make_path_, QString make_mode_,
                     QString sys_eol_)
{
    count = 0;
    mod_num = 0;

    make_cfg = make_cfg_;
    hostname = hostname_;
    custom = custom_;

    make_mode = make_mode_;

    if(make_mode == "system"){
        eol = sys_eol_;
        hosts_file = new QFile("hosts");

    }else if(make_mode == "ansi"){
        eol = "\r\n";
        hosts_file = new QFile(make_path_);

    }else if(make_mode == "utf-8"){
        eol = "\n";
        hosts_file = new QFile(make_path_);

    }
}

MakeHosts::~MakeHosts()
{
    if(hosts_file->isOpen())
        hosts_file->close();
    delete hosts_file;
}

/**
 * @brief Start operations to retrieve data from the data file and make the new
 * hosts file for the current operating system.
 */
void MakeHosts::make()
{
    RetrieveData::connect_db();
    make_time = QDateTime::currentDateTime();
    hosts_file->open(QIODevice::ReadWrite);
    write_head();
    write_info();
    get_hosts(make_cfg);
    hosts_file->close();
    RetrieveData::disconnect_db();
}

/**
 * @brief Make the new hosts file by the configuration defined by `make_cfg`
 * from function list on the main dialog.
 * @param make_cfg  Module settings in byte word format.
 */
void MakeHosts::get_hosts(std::map<int, int> &make_cfg)  //make_cfg
{
     for(std::map<int, int>::iterator it=make_cfg.begin(); it!=make_cfg.end();++it){
        int part_id = it->first;
        int mod_cfg = make_cfg[part_id];
        if(!RetrieveData::chk_mutex(part_id, mod_cfg))
            return;
        QList<int> mods = RetrieveData::get_ids(mod_cfg);
        for(int i=0; i<mods.size(); i++){
            int mod_id = mods[i];
            mod_num += 1;
            QList<std::map<QString, QString> > hosts;
            QString mod_name;
            RetrieveData::get_host(part_id, mod_id, mod_name, hosts);
            //info_trigger.emit(mod_name, self.mod_num)
            if(part_id == 0x02){
                qDebug()<<"localhost mod_id: "<<mod_id;
                write_localhost_mod(hosts);
            }else if(part_id == 0x04){
                write_customized();
            }else{
                write_common_mod(hosts, mod_name);
            }
        }
    }
}

/**
 * @brief Write the head part into the new hosts file.
 */
void MakeHosts::write_head()
{
    QStringList ret = RetrieveData::get_head();
    for(int i=0; i<ret.size(); i++){
        QString head_str = ret[i];
        hosts_file->write((head_str + eol).toLocal8Bit());
    }
}

/**
 * @brief Write the information part into the new hosts file.
 */
void MakeHosts::write_info()
{
    std::map<QString, QString> info = RetrieveData::get_info();
    QStringList info_lines;
    info_lines.push_back("#");
    info_lines.push_back(QString("# ") + "Version" + ": " + info["Version"]);
    info_lines.push_back(QString("# ") + "BuildTime" + ": " + info["Buildtime"]);
    QString a;
    a.QString::sprintf("%d", make_time.toTime_t());
    info_lines.push_back(QString("# ") + "ApplyTime" + ": " + a);
    info_lines.push_back("#");
    for(int i=0; i<info_lines.size(); i++){
        QString line = info_lines[i];
        hosts_file->write((line + eol).toLocal8Bit());
    }

}

/**
 * @brief Write user customized hosts list into the hosts file if the customized
 * hosts file exists.
 */
void MakeHosts::write_customized()
{
    qDebug()<<"custom in";
    if(QFile::exists(custom)){
        QFile custom_file(custom);
        custom_file.open(QIODevice::ReadWrite);
        hosts_file->write((eol + "# Section Customized: " + eol).toLocal8Bit());
        char a[250];
        while(!custom_file.atEnd()){
            custom_file.readLine(a, 250);
            QString line(a);
            if(line.startsWith("#")){
                hosts_file->write((line + eol).toLocal8Bit());
            }
            int a = line.indexOf(" ");
            if(a >= 0){
                QString ip = line.left(a);
                QString host = line.mid(a);
                if(ip.length() < 16){
                    ip = ip.leftJustified(16);
                }
                hosts_file->write((ip + " " +  host + eol).toLocal8Bit());
            }
        }
        hosts_file->write((eol + "# Section End: Customized: " + eol).toLocal8Bit());
        custom_file.close();
    }
    qDebug()<<"custom out";
}


/**
 * @brief Write hosts entries :attr:`hosts` from a module named `hosts` in the
 * hosts data file.
 * @param hosts  Hosts entries from a part in the data file.
 * @param mod_name  Name of a module from the data file.
 */
void MakeHosts::write_common_mod(QList<std::map<QString, QString> > &hosts, QString mod_name)
{
    hosts_file->write((eol + "# Section Start: " + mod_name + eol).toLocal8Bit());
    for(int i=0; i<hosts.size(); i++){
        std::map<QString, QString> host = hosts[i];
        QString ip = host["ip"];
        if(ip.length() < 16){
            ip = ip.leftJustified(16);
        }
        hosts_file->write((ip + " " +  host["host"] + eol).toLocal8Bit());
        count += 1;
    }
    hosts_file->write(("# Section End: " + mod_name + eol).toLocal8Bit());
}

/**
 * @brief Write hosts entries `hosts` from a module named `hosts` in the
 * hosts data file.
 * @param hosts  Hosts entries from a part in the data file.
 */
void MakeHosts::write_localhost_mod(QList<std::map<QString, QString> > &hosts)
{
    hosts_file->write((eol + "# Section Start: Localhost" + eol).toLocal8Bit());
    for(int i=0; i<hosts.size(); i++){
        std::map<QString, QString> host = hosts[i];
        QString h = host["host"];
        if(h.indexOf("#Replace") >= 0 ){
            host["host"] = hostname;
        }
        QString ip = host["ip"];
        if(ip.length() < 16){
            ip = ip.leftJustified(16);
        }
        hosts_file->write((ip + " " + host["host"] + eol).toLocal8Bit());
        count += 1;
    }
    hosts_file->write(("# Section End: Localhost" + eol).toLocal8Bit());
}

