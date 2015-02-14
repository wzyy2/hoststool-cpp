#include "commonutil.h"
#include "roottools.h"

#include <math.h>
#include <unistd.h>

#include <QHostInfo>
#include <QString>
#include <QProcess>
#include <QtNetwork>
#include <QSettings>


/**
 * @brief Check connect to a specified server by :attr:`link`.
 * @param link  The link to a specified server. This string could be a
 * domain name or the IP address of a server.
 * @return A flag indicating whether the connection status is good or not.
 */
int CommonUtil::check_connection(QString link)
{
    QTcpSocket *client;
    // create custom temporary event loop on stack
    client = new QTcpSocket();
    client->connectToHost(link, 80);
    bool ret;
    ret = client->waitForConnected();
    //int ret = client->write(data);
    delete client;
    return ret?1:0;
}

/**
 * @brief Check information about current operating system.
 * @return system(`str`): Operating system of current session.
 * @return hostname(`str`): Hostname of current machine.
 * @return path(`str`): Path to hosts on current operating system.
 * @return encode(`str`): Default encoding of current OS.
 * @return flag(`int`): A flag indicating whether the current OS is
 * supported or not.
 *
 */
CommonUtil::platform_struct CommonUtil::check_platform()
{
    CommonUtil::platform_struct ret;

    ret.hostname = QHostInfo::localHostName();
    qDebug("hostname:%s", ret.hostname.toStdString().c_str());

#ifdef Q_OS_LINUX
    ret.system = "Linux";
    ret.encode = "unix_utf8";
    ret.path = "/etc/hosts";
#endif
#ifdef Q_OS_ANDROID
    ret.system = "Android";
    ret.path = "/system/etc/hosts";
#endif
#ifdef Q_OS_IOS
    ret.system = "IOS";
    ret.encode = "unix_utf8";
    ret.path = "/etc/hosts";
    //Remove the ".local" suffix
    ret.hostname = ret.hostname.mid(0, ret.hostname.length() - 6);
#endif
#ifdef Q_OS_OSX
    ret.system = "OS X";
    ret.encode = "unix_utf8";
    ret.path = "/etc/hosts";
    //Remove the ".local" suffix
    ret.hostname = ret.hostname.mid(0, ret.hostname.length() - 6);
#endif
#ifdef Q_OS_WIN
    ret.system = "Windows";
    ret.encode = "win_ansi";
    QStringList env_list(QProcess::systemEnvironment());
    int idx = env_list.indexOf(QRegExp("^WINDIR=.*", Qt::CaseInsensitive));
    QStringList windir = env_list[idx].split('=');
    ret.path = windir[1] + QString("\\System32\\drivers\\etc\\hosts");
    qDebug("windir:%s", ret.path.toStdString().c_str());
#endif
#ifdef HOSTS_TEST
    ret.path = "./test/hosts";
#endif
    ret.flag = 1;
    return ret;
}

/**
 * @brief Check whether the current session has privileges to change the hosts
 * file of current operating system.
 * @param username  Username of the user running current session.
 * @param flag   A flag indicating whether the current session has
 * write privileges to the hosts file or not.
 */
void CommonUtil::check_privileges(QString &username, bool &flag)
{
    CommonUtil::platform_struct p = CommonUtil::check_platform();
    QStringList env_list(QProcess::systemEnvironment());
    if(p.system == "windows"){
        int idx = env_list.indexOf(QRegExp("^USERNAME=.*", Qt::CaseInsensitive));
        QStringList username_ = env_list[idx].split('=');
        username = username_[1];
    }else if(p.system == "Linux"){
        int idx = env_list.indexOf(QRegExp("^USER=.*", Qt::CaseInsensitive));
        QStringList username_ = env_list[idx].split('=');
        username = username_[1];
    }else{
        username = "unKnown";
    }
    qDebug()<<"username:"<<username;
    
#ifdef ROOTTOOL_NEED
    if(!RootTools::Instance()->check_root()){
        flag = false;
        return;
    }else{
        flag = true;
        return;
    }
#endif
    
    QFile file(p.path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug()<<"Can't open the file!"<<endl;
        flag = false;
    }else{
        flag = true;
        file.close();
    }
}

/**
 * @brief Get configurations for mirrors to connect to.
 * @param file_path  Path to a configuration file containing which
 * contains the server list.
 * @return `tag`, `test url`, `label`, and `update url` of the servers listed in
 * the configuration file.
 */
std::vector<std::map<QString, QString> > CommonUtil::set_network(QString file_path)
{
    std::vector<std::map<QString, QString> > ret;
    QSettings *config = new QSettings(file_path, QSettings::IniFormat);
    QStringList group = config->childGroups();
    qDebug()<<"group size:"<<group.size();
    //std::vector<std::map<QString, QString> >
    for(int i = 0; i < group.size(); i++)
    {
        std::map<QString, QString> a;
        a["tag"] = group[i];
        a["label"] = config->value(group[i] + "/label").toString();
        a["test_url"] = config->value(group[i] + "/server").toString();
        a["update"] = config->value(group[i] + "/update").toString();
        ret.push_back(a);
    }
    delete config;

    return ret;
}

/**
 * @brief Transform unix `timestamp` to a data string in ISO format.
 * @param param timestamp: A unix timestamp indicating a specified time.
 * @return Date in ISO format, which is `YY-mm-dd` in specific.
 */
QString CommonUtil::timestamp_to_date(QString time)
{
    QDateTime timestamp;
    timestamp.setTime_t(time.toInt());
    QString ret = timestamp.toString(Qt::ISODate);
    ret =ret.left(ret.indexOf('T'));
    return ret;
}

/**
 * @brief Convert byte size :attr:`bufferbytes` of a file into a size string.
 * @param bufferbytes  The size of a file counted in bytes.
 * @return A readable size string.
 */
QString CommonUtil::convert_size(int bufferbytes)
{
    if(bufferbytes == 0)
        return "0 B";
    unsigned long l_unit = log(bufferbytes)/log(0x400);

    QString units[] = {"B", "KB", "MB", "GB", "TB", "PB"};
    QString formats[] = {"%.2f", "%.1f", "%.0f"};
    float f = pow(0x400, l_unit);
    float size = bufferbytes / f;
    unsigned long l_num = log(size)/log(10);

    if(l_unit >= sizeof(units))
        l_unit = sizeof(units) - 1;
    if(l_num >= sizeof(formats))
        l_num = sizeof(formats) - 1;

    QString a = formats[l_num] + " " + units[l_unit];

    QString out;
    out.sprintf(a.toStdString().c_str(), size);

    return out;
}

/**
 * @brief Cut english message msg into lines with specified length cut.
 * @param msg  The message to be cut.
 * @param cut  The length for each line of the message.
 * @return Lines cut from the message.
 */
QStringList CommonUtil::cut_message(QString msg, int cut)
{
    QStringList msgs;
    while(msg.length() >= cut){
        QString line;
        //        QStringList words = str.split("\n");
        if(msg.mid(0, cut - 1).indexOf("\n") >= 0){
            line = msg.left(msg.indexOf("\n"));
            msg = msg.mid(msg.indexOf("\n" + 1)); //\n
        }else{
            int cut_len;
            QString hyphen;
            if(msg[cut - 1] != ' ' && msg[cut - 1] != '\n'
                    && msg[cut] != ' ' && msg[cut] != '\n'){
                cut_len = cut -1;
                if(msg[cut - 2] != ' ' && msg[cut - 2] != '\n'){
                    hyphen = ' ';
                }else{
                    hyphen = '-';
                }
            }else{
                cut_len = cut;
                hyphen = ' ';
            }
            line = msg.mid(0, cut_len) + hyphen;
            msg = msg.mid(cut_len);

        }
        msgs.append(line);
    }
    msgs.append(msg);
    return msgs;
}

/**
 * @brief Copys a file to a destination.
 * @param src source
 * @param dst destination
 * @param remountAsRw remounts the destination as read/write before writing to it
 */

bool CommonUtil::copyFile(QString src, QString dst)
{
#ifdef ROOTTOOL_NEED
#ifdef Q_OS_ANDROID
    //android /system filesystem usually is mount as readonly
    return RootTools::Instance()->copyFile(src, dst, true);
#endif
    return RootTools::Instance()->copyFile(src, dst, false);
#else
    if (QFile::exists(dst)){
        QFile::remove(dst);
    }
    return QFile::copy(src, dst);
#endif
}
