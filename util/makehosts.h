#ifndef MAKEHOSTS_H
#define MAKEHOSTS_H
#include "hoststool.h"
#include <QFile>
#include <QStringList>
#include <QDateTime>

//MakeHosts class contains methods to make a hosts file with host entries \
//from a single data file.
class MakeHosts
{
public:
    explicit MakeHosts(std::map<int, int> make_cfg, QString hostname, QString custom,
                       QString make_path, QString make_mode,
                       QString sys_eol);
    ~MakeHosts();
    void make();
    void get_hosts(std::map<int, int> &make_cfg);
    void write_head();
    void write_info();
    void write_common_mod(QList<std::map<QString, QString> > &hosts, QString mod_name);
    void write_localhost_mod(QList<std::map<QString, QString> > &hosts);
    void write_customized();

    //Operation mode for making hosts file. The valid value could be one of `system`, `ansi`, and `utf-8`.
    QString make_mode;

    //File name of User Customized Hosts file.
    QString custom;

    //File Name of hosts file.
    QString hostname;

    //The hosts file to write hosts to.
    QFile *hosts_file;

    //Configuration to make a new hosts file.
    std::map<int, int>  make_cfg;

    //Total number of modules written to hosts file.
    int mod_num;

    //Number of the module being processed currently in the operation sequence.
    int count;

    //End-of-Line used by the hosts file created.
    QString eol;

    //timestamp of making hosts file.
    QDateTime  make_time;
};

#endif // MAKEHOSTS_H
