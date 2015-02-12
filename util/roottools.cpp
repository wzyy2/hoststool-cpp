#include "roottools.h"
#include <QProcess>
#include <QDir>

RootTools* RootTools::pInstance = NULL;

RootTools::RootTools()
{
    shell_.start("su");
    shell_.waitForStarted();
    //echo 1 and receive to check if su works well
    QString cmd = "echo 1\n";
    shell_.write(cmd.toStdString().c_str(), cmd.size());
    shell_.waitForReadyRead();
    if(shell_.readAll()[0] == '1')
        if_root = true;
    else
        if_root = false;
    if(if_root){
        mounts_ = getMounts();
    }
}

RootTools::~RootTools()
{

}

RootTools* RootTools::Instance()
{
    if(pInstance == NULL)
    {
        pInstance = new RootTools();
    }
    return pInstance;
}

void RootTools::Destroy()
{
    delete pInstance;
    pInstance = NULL;
}

/**
 * @brief Check if it's root success
 */
bool RootTools::check_root()
{
    return if_root;
}

/**
 * @brief Copy file.
 */
bool RootTools::copyFile(QString src, QString dst, bool remountAsRW)
{
    QFileInfo s(src);
    QFileInfo d(dst);
    src = s.absoluteFilePath();
    dst = d.absoluteFilePath();

    if(remountAsRW){
        remount(dst, "rw");
    }

    QString cmd = "cat " + src + " > " + dst + "\n";
    shell_.write(cmd.toStdString().c_str(), cmd.size());

    if(remountAsRW){
        remount(dst, "ro");
    }

    qDebug()<<"copy_file "<<cmd;

    return true;
}

/**
 * @brief Remount the file path
 * @param file the path of file.
 * @param mountType  "rw" or "ro"
 */
bool RootTools::remount(QString file, QString mountType)
{
    bool foundMount = false;
    int i = 0;
    QDir dir = QFileInfo(file).dir();

    while(!foundMount){
        for(i = 0; i < mounts_.size(); i++){
            if(dir.absolutePath() == mounts_[i][1]){
                foundMount = true;
                break;
            }
        }
        dir.cdUp();
        if(dir.isRoot())
            break;
    }
    if(!foundMount)
        return false;

    QString cmd = "mount -o " + mountType + ",remount " + mounts_[i][0] + " " + mounts_[i][1] + "\n";
    qDebug()<<"mount"<<cmd;
    shell_.write(cmd.toStdString().c_str(), cmd.size());
}

/**
 * @brief get mounts points info
 * device,mountPoint,fstype,flags
 */
QList<QStringList> RootTools::getMounts()
{
    QList<QStringList> mounts;

    QString cmd = "cat /proc/mounts\n";
    shell_.write(cmd.toStdString().c_str(), cmd.size());
    shell_.waitForReadyRead();

    QByteArray rr = shell_.readAll();
    QString ss(rr);
    QStringList info = ss.split("\n");

    for(int i = 0; i < info.size(); i++){
        QStringList m = info[i].split(" ");
        if(m.size() < 4){
            continue;
        }
        mounts.push_back(m);
    }

    return mounts;
}
