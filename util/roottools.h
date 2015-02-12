#ifndef ROOTTOOLS_H
#define ROOTTOOLS_H
#include "hoststool.h"
#include <QProcess>

/**
 * RootTools class contains a set of tools for use in the development of rooted applications
 */
class RootTools
{
private:
    static RootTools* pInstance;
    bool if_root;
    QProcess shell_;
    QList<QStringList> mounts_;
    RootTools();
    ~RootTools();

public:
    static RootTools* Instance();
    static void Destroy();
    bool copyFile(QString src, QString dst, bool remountAsRw = true);
    bool check_root();
    bool remount(QString file, QString mountType);
    QList<QStringList>  getMounts();
};

#endif // ROOTTOOLS_H
