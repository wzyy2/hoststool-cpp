#ifndef RETRIEVEDATA_H
#define RETRIEVEDATA_H
#include "hoststool.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>

#define DATAFILE (QString(PATH_PREFIX) + "hostslist.data")
#define DATABASE (QString(PATH_PREFIX) + "hostslist.s3db")

/**
 * RetrieveData class contains a set of tools to retrieve information from the hosts data file.
 */
class RetrieveData
{
private:

    /**
     * Connection of sql.
     */
    QSqlDatabase conn;

    /**
     * Filename of a SQLite database file.
     */
    QString _database;

    bool connected;

    static RetrieveData* pInstance;

    RetrieveData();
    ~RetrieveData();

public:

    static RetrieveData* Instance();
    static void Destroy();

    bool db_exists(QString database = DATABASE);

    void connect_db(QString database = DATABASE);

    void disconnect_db();

    std::map<QString, QString>  get_info();

    QStringList get_head();

    QList<int> get_ids(int id_cfg);

    void get_host(int part_id, int mod_id, QString &mod_name, QList<std::map<QString, QString> > &hosts);

    void get_choice(bool flag_v6, QList<QList<QVariant> > &modules,
                    std::map<int, QList<int> > &defaults,
                    QList<int> &slices);

    bool chk_mutex(int part_id, int mod_cfg);

    void unpack(QString datafile = DATAFILE, QString database = DATABASE);

    void clear();
};

#endif // RETRIEVEDATA_H
