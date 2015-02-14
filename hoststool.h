#ifndef GLOBAL_H
#define GLOBAL_H

#include <QStringList>
#include <QString>
#include <QDebug>
#include <QStandardPaths>

#include <map>
#include <vector>

#if (defined Q_OS_IOS) || (defined Q_OS_ANDROID)
#define USE_MOBILE
#define ROOTTOOL_NEED
#endif

#if (defined Q_OS_IOS)
#define PASSWORD_NEED
#define PATH_PREFIX (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/")
#else
#define PATH_PREFIX "./"
#endif



//#define TRAN(a,b)  QApplication::translate((a), (b))

#endif // GLOBAL_H

