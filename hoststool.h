#ifndef GLOBAL_H
#define GLOBAL_H

#include <QStringList>
#include <QString>
#include <QDebug>


#include <map>
#include <vector>

#if (defined Q_OS_IOS) || (defined Q_OS_ANDROID)
    #define USE_MOBILE
#endif

//#define TRAN(a,b)  QApplication::translate((a), (b))

#endif // GLOBAL_H

