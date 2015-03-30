#include "commonefunctions.h"
#include <QStringList>
#include <QDir>
#include <QApplication>
#include <QTranslator>
#include <QSettings>

bool checkAppPath(const QString &pRelativePath, const QString &pBasePath){
    bool rv = true;
    qApp->setProperty("checkAppPathMessage",QVariant());
    //
    QString lRelativePath(pRelativePath.trimmed());
    QString lBasePath(pBasePath.trimmed());
    if (lBasePath.isEmpty()) lBasePath = qApp->applicationDirPath();
    lRelativePath.replace("\\","/").replace("//","/");
    QStringList dirNames = lRelativePath.split("/",QString::SkipEmptyParts);
    QDir dir = QDir(lBasePath);
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
                              ,QObject::tr("In the path: ")+pRelativePath
                              +QObject::tr("\ncan not find/create folder : ")+dirNames.at(i));
        }
    }
    return rv;
}


bool addAppTranslator(const QString &pTranslationBase, const QString &pTranslationDir){
    bool rv = false;
    QString textError;
    qApp->setProperty("addAppTranslatorMessage",QVariant());
    QString localTranslationFile(pTranslationBase);
    QString localTranslationDir(pTranslationDir);
    bool dirExists = true;
    if (localTranslationDir.isEmpty()){
        localTranslationDir = "/translations/";
        dirExists = checkAppPath(localTranslationDir);
        localTranslationDir = qApp->applicationDirPath()+localTranslationDir;
    }else{
        dirExists = QFile::exists(localTranslationDir);
    }
    if (!dirExists){
        textError = QObject::tr("Translation dir no exists: ")+localTranslationFile;
    }else{
        QSettings *appSettings = qobject_cast<QSettings *>(variantToObject(qApp->property("settings")));
        if (appSettings){
            localTranslationFile += "_" + appSettings->value("App/Locale").toString();
        }
        QTranslator * translator = new QTranslator(qApp);
        if (translator){
            if (translator->load(localTranslationFile,localTranslationDir)){
                if (qApp->installTranslator(translator)){
                    rv = true;
                }else{
                    textError = QObject::tr("Translation not loaded: ")+localTranslationFile
                            +QObject::tr("\nfrom dir: ")+localTranslationDir;
                }
            }else{
                textError = QObject::tr("Translation not found: ")+localTranslationFile
                        +QObject::tr("\nin dir: ")+localTranslationDir;
                        ;
            }
        }else{
            textError = QObject::tr("Translation object not created for: ")
                    +"\n"+localTranslationDir+localTranslationFile;
        }
    }
    if (!textError.isEmpty()){
        qApp->setProperty("addAppTranslatorMessage", textError);
    }
    return rv;
}

QVariant objectToVariant(QObject *pObject){
    QVariant rv;
    if (pObject){
        rv.setValue<QObject *>(pObject);
    }
    return rv;
}

QObject *variantToObject(QVariant pVariant){
    QObject *rv=NULL;
    if (pVariant.isValid() && pVariant.canConvert<QObject *>()){
        rv = pVariant.value<QObject *>();
    }
    return rv;
}

void updateSettings(const QVariantMap &pKeyValueMap, QSettings *pSettings, QObject *pObject, bool pForceUpdate){
    QMap<QString, QVariant>::const_iterator i;
    for (i = pKeyValueMap.constBegin(); i != pKeyValueMap.constEnd(); ++i){
        if (pForceUpdate || !pSettings->contains(i.key())) {
            pSettings->setValue(i.key(),i.value());
        }
        if (pObject){
            pObject->setProperty(i.key().toUtf8(), pSettings->value(i.key()));
        }
    }
}
