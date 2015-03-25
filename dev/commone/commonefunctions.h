#ifndef COMMONEFUNCTIONS_H
#define COMMONEFUNCTIONS_H

#include <QString>
#include <QVariant>

bool checkAppPath(const QString &pRelativePath, const QString &pBasePath=QString());
bool addAppTranslator(const QString &pTranslationBase, const QString &pTranslationDir=QString());
QVariant objectToVariant(QObject *pObject);
QObject *variantToObject(QVariant pVariant);

#endif // COMMONEFUNCTIONS_H
