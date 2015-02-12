#ifndef COMMONUTIL_H
#define COMMONUTIL_H
#include "hoststool.h"


/**
 * CommonUtil contains a set of basic tools for Hosts Setup Utility to use.
 */
namespace CommonUtil {

int check_connection(QString link);

typedef struct{
    QString system;
    QString hostname;
    QString path;
    QString encode;
    int flag;
}platform_struct;

platform_struct check_platform();

void check_privileges(QString &username, bool &flag);

std::vector<std::map<QString, QString> > set_network(QString file_path = ":/conf/network.conf");

QString timestamp_to_date(QString timestamp);

QString convert_size(int bufferbytes);

QStringList cut_message(QString msg, int cut);

bool copyFile(QString src, QString dist, bool remountAsRw = true);
}

#endif // COMMONUTIL_H
