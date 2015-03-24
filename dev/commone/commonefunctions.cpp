#include "commonefunctions.h"
#include <QStringList>
#include <QDir>
#include <QApplication>

bool checkAppPath(const QString &pTextPath){
    bool rv = true;
    qApp->setProperty("checkAppPathMessage",QVariant());
    //
    QString path(pTextPath.trimmed());
    path.replace("\\","/").replace("//","/");
    QStringList dirNames = path.split("/",QString::SkipEmptyParts);
    QDir dir = QDir(qApp->applicationDirPath());
    int leftAttempts;
    for (int i=0;i<dirNames.size();++i){
        leftAttempts=2;
        while (!dir.cd(dirNames.at(i)) && leftAttempts){
            dir.mkdir(dirNames.at(i));
            leftAttempts--;
        }
        if (!leftAttempts){
            rv = false;
            qApp->setProperty("checkAppPathMessage"
                              ,QObject::tr("Ошибка проверки пути: ")+pTextPath
                              +QObject::tr("\nНе смогли создать поддиректорию: ")+dirNames.at(i));
        }
    }
    return rv;
}


