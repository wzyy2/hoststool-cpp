#include "retrievedata.h"

#include <QFile>
#include <QDebug>

#include <lib/quazip/quazipfile.h>
#include <lib/quazip/quazip.h>


using namespace::RetrieveData;

QSqlDatabase RetrieveData::conn;
QString RetrieveData::_database;

static bool first = true;
/**
 * @brief Check whether the `database` file exists or not.
 * @param database  Path to a SQLite database file.
 * `./hostslist.s3db` by default.
 * @return A flag indicating whether the database file exists or not.
 */
bool RetrieveData::db_exists(QString database)
{
    return QFile::exists(database);
}

/**
 * @brief Set up connection with a SQLite :attr:`database`.
 * @param database  Path to a SQLite database file.
 * `./hostslist.s3db` by default.
 */
void RetrieveData::connect_db(QString database)
{
    _database = database;
    if(first){
        conn = QSqlDatabase::addDatabase("QSQLITE");
        conn.setDatabaseName(database);
        first = false;
    }

    if(conn.isOpen() == false)
    {
        conn.open();
    }
}

/**
 * @brief Close the connection with a SQLite database.
 */
void RetrieveData::disconnect_db()
{
    if(conn.isOpen() == true)
    {
        conn.close();
    }
}

/**
 * @brief Retrieve the metadata of current data file.
 * @return Metadata of current data file. The metadata here is a
 * dictionary while the `Keys` are made of `Section Name` and
 * `Values` are made of `Information` defined in the hosts data file.
 */
std::map<QString, QString>  RetrieveData::get_info()
{
    std::map<QString, QString>  info;
    QSqlQuery query(conn);
    query.exec("SELECT sect, info FROM info;");
    while(query.next()){
        info[query.value(0).toString()] = query.value(1).toString();
    }
    return info;
}

/**
 * @brief Retrieve the head information from hosts data file.
 * @param head Lines of hosts head information.
 */
QStringList RetrieveData::get_head()
{
    QStringList head;
    QSqlQuery query(conn);
    query.exec("SELECT str FROM hosts_head ORDER BY ln;");
    while(query.next()){
        //        qDebug()<<query.value(0).toString();
        head.append(query.value(0).toString());
    }
    return head;
}

/**
 * @brief Calculate the id numbers covered by config word `id_cfg`.
 * @param id_cfg  A hexadecimal config word of id selections.
 * @param ids ID numbers covered by config word.
 */
QList<int> RetrieveData::get_ids(int id_cfg)
{
    QList<int> ids;
    char a[100];
    //to_binary
    memset(a, 0, 100);
    for(int i = 0; id_cfg && i < 64 ; ++i, id_cfg /= 2){
        a[i] = "01"[id_cfg % 2];
    }

    //set bit to a list.....
    QString cfg(a);
    for(int i = 0 ; i < cfg.length(); i++){
        if(cfg[i] != '0'){
            ids.push_back(1 << (i));
        }
    }
    return ids;
}


/**
 * @brief Retrieve the hosts module specified by `mod_id` from a part
 * specified by `part_id` in the data file.
 * @param part_id ID number of a specified part from the hosts data file.
 * @param mod_id ID number of a specified module from a specified part.
 * @param mod_name  Name of a specified module.
 * @param Hosts entries from a specified module.

 */
void RetrieveData::get_host(int part_id, int mod_id, QString &mod_name, QList<std::map<QString, QString> > &hosts)
{
    QString part_name;
    if(part_id == 0x04){
        mod_name = "customize";
        return;
    }

    QSqlQuery query(conn);
    char a[100];
    sprintf(a, "SELECT part_name FROM parts "
               "WHERE part_id=%d;", part_id);
    query.exec(a);
    if(query.first()){
        part_name = query.value(0).toString();
    }else{
        return;
    }

    sprintf(a, "SELECT ip, host FROM %s "
               "WHERE cate=%d;", part_name.toStdString().c_str(), mod_id);
    query.exec(a);
    while(query.next()){
        std::map<QString, QString> a;
        //        qDebug()<<query.value(0).toString();
        //        qDebug()<<query.value(1).toString();
        a["ip"] = query.value(0).toString();
        a["host"] = query.value(1).toString();
        hosts.push_back(a);
    }

    sprintf(a, "SELECT mod_name FROM modules "
               "WHERE part_id=%d AND mod_id=%d;", part_id, mod_id);
    query.exec(a);
    if(query.first()){
        mod_name = query.value(0).toString();
    }

    return;
}

/**
 * @brief Retrieve module selection items from the hosts data file with default
 * selection for users.
 * @param flag_v6 A flag indicating whether to receive IPv6 hosts
 * entries or the IPv4 ones. Default by `False`.
 * @return modules(`list`): Information of modules for users to select.
 * defaults(`dict`): Default selection config for selected parts.
 * slices(`list`): Numbers of modules in each part.
 */
void RetrieveData::get_choice(bool flag_v6, QList<QList<QVariant> > &modules,
                              std::map<int, QList<int> > &defaults,
                              QList<int> &slices)
{
    char ch_parts[3];
    char a[100];
    //ch_parts decide which part to read
    ch_parts[0] = 0x08;
    ch_parts[1] = flag_v6?0x20:0x10;
    ch_parts[2] = 0x40;

    QSqlQuery query(conn);
    sprintf(a, "SELECT * FROM modules "
               "WHERE part_id IN (%d, %d, %d);", ch_parts[0], ch_parts[1], ch_parts[2]);
    query.exec(a);
    while(query.next()){
        QList<QVariant> a;
        a.append(query.value(0));
        a.append(query.value(1));
        a.append(query.value(2));
        a.append(query.value(3));
        modules.push_back(a);
    }

    sprintf(a, "SELECT part_id, part_default FROM parts "
               "WHERE part_id IN (%d, %d, %d);", ch_parts[0], ch_parts[1], ch_parts[2]);
    query.exec(a);
    while(query.next()){
        defaults[query.value(0).toInt()] = get_ids(query.value(1).toInt());
    }
    slices.push_back(0);
    for(int i=0; i<3; i++){
        sprintf(a, "SELECT COUNT(mod_id) FROM modules "
                   "WHERE part_id=%d;", ch_parts[i]);
        query.exec(a);
        if(query.first()){
            slices.push_back(query.value(0).toInt());
        }
    }
    for(int s=1; s<slices.size(); s++){
        slices[s] += slices[s - 1];
    }
}

/**
 * @brief Check if there is conflict in user selections `mod_cfg` from a
 * part specified by `part_id` in the data file.
 * @param part_id  ID number of a specified part from the hosts data file.
 * @param mod_cfg  A 16-bit config word indicating module selections of a
 * specified part.
 * @return A flag indicating whether there is a conflict or not.
 */
bool RetrieveData::chk_mutex(int part_id, int mod_cfg)
{
    char a[100];
    if(part_id == 0x04){
        return true;
    }
    QSqlQuery query(conn);
    sprintf(a, "SELECT mod_id, mutex FROM modules"
               "WHERE part_id=%d;", part_id);
    query.exec(a);
    std::map<int, int> mutex_tuple;
    while(query.next()){
        mutex_tuple[query.value(0).toInt()] = query.value(1).toInt();
    }
    QList<int> mutex_info;
    QList<int> mod_info = get_ids(mod_cfg);
    for(int i=0; i<mod_info.size(); i++){
        mutex_info = get_ids(mutex_tuple[mod_info[i]]);
    }
    for(int i=0; i<mod_info.size(); i++){
        for(int j=0; j<mutex_info.size(); j++){
            if(mod_info[i] == mutex_info[j])
                return false;
        }
    }

    return true;
}

/**
 * @brief Unzip the archived `datafile` to a SQLite database file `database`.
 * @param datafile  Path to the zipped data file. `./hostslist.data` by
 * default.
 * @param database  Path to a SQLite database file. `./hostslist.s3db` by
 * default.
 */
void RetrieveData::unpack(QString datafile, QString database)
{
    QuaZip archive(datafile);
    if (!archive.open(QuaZip::mdUnzip))
        throw QString("io");
    if(!archive.goToFirstFile()){
        throw QString("BadZip");
    }
    for( bool f = archive.goToFirstFile(); f; f = archive.goToNextFile() )
    {
        QString filePath = archive.getCurrentFileName();
        QuaZipFile zFile(archive.getZipName(), filePath);
        zFile.open(QIODevice::ReadOnly );
        QByteArray ba = zFile.readAll();
        zFile.close();

        QFile dstFile(database);
        if (!dstFile.open(QIODevice::WriteOnly))
            throw QString("io");
        dstFile.write(ba);
        dstFile.close();
    }
    return;
}

/**
 * @brief Close connection to the database and delete the database file.
 */
void RetrieveData::clear()
{
    if(conn.isOpen() == true)
    {
        conn.close();
    }
    QSqlDatabase::removeDatabase(_database);
    first = true;

    if(db_exists(_database))
        QFile::remove(_database);
}
