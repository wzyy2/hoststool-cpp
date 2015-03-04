#include "roottools.h"
#include <QProcess>
#include <QDir>

RootTools* RootTools::pInstance = NULL;
QString RootTools::password_ = "alpine";
bool RootTools::need_password_ = false;

RootTools::RootTools()
{
    QString cmd;

    shell_.start("su");
    shell_.waitForStarted();
    shell_.waitForReadyRead(500);

    if(need_password_){
        cmd = password_ + "\n";
        shell_.write(cmd.toStdString().c_str(), cmd.size());
        shell_.waitForReadyRead(300);
    }

    //echo 1 and receive it to check if su works well
    cmd = "echo 1\n";
    shell_.write(cmd.toStdString().c_str(), cmd.size());
    if(need_password_){
        shell_.waitForReadyRead(500);
    }else{
        //wait for allowed
        shell_.waitForReadyRead(15000);
    }
    if(shell_.readAll()[0] == '1')
        if_root = true;
    else
        if_root = false;

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
 * @brief Configure password for roottools.
 * @param password  password used for su.
 * @param need_password  A flag decide if it need password for su
 */
void RootTools::Configure(QString password, bool need_password)
{
   password_ = password;
   need_password_ = need_password;
}

/**
 * @brief Check if it's root success
 */
bool RootTools::check_root()
{
    return if_root;
}

/**
 * @brief Use su shell to copy file.
 * @param src the path of source.
 * @param dst the path of destination.
 * @param remountAsRW  if it need be remount.
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
    shell_.waitForBytesWritten(100);

    if(remountAsRW){
        remount(dst, "ro");
    }

    qDebug()<<"copy_file "<<cmd;

    return true;
}

/**
 * @brief Remount the file path,the mount point will be found automatically.
 * @param file the path of file.
 * @param mountType  "rw" or "ro"
 */
bool RootTools::remount(QString file, QString mountType)
{
    if(mounts_.size() == 0){
        mounts_ = getMounts();
    }

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
    shell_.waitForBytesWritten(100);

    return true;
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
    shell_.waitForReadyRead(1000);

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
