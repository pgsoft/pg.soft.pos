#ifndef COMMONEFUNCTIONS_H
#define COMMONEFUNCTIONS_H

#include <QString>
#include <QVariant>
#include <QSettings>

bool checkAppPath(const QString &pRelativePath, const QString &pBasePath=QString());
bool addAppTranslator(const QString &pTranslationBase, const QString &pTranslationDir=QString());
QVariant objectToVariant(QObject *pObject);
QObject *variantToObject(QVariant pVariant);
void updateSettings(const QVariantMap &pKeyValueMap
                    , QSettings *pSettings
                    , QObject *pObject=0
                    , bool pForceUpdate=false);

#endif // COMMONEFUNCTIONS_H
