#ifndef __ANYPLUGININTERFACE_H__
#define __ANYPLUGININTERFACE_H__

#include <QString>
#include <QSettings>
#include <QList>
#include "enums.h"


class AnyPluginInterface
{
protected:
    QString pluLoadPath; //путь, где обнаружен плагин
    QSettings * appParameters; //параметры приложения
    QSettings * pluParameters; // параметры плагина
    QList<QObject *> * pluList; //список всех обнаруженных плагинов
    QString pluFileName; //имя файла плагина, как он обнаружен на диске
public:
    virtual ~AnyPluginInterface() {}
    //
public:
    virtual int getPluType() = 0; //получить тип плагина
    virtual int getPluSortPosition() = 0; //получить позицию для сортировки в списке плагинов
    virtual QString getPluUID() const = 0; //получить уникальный айди плагина
    virtual QString getPluName() const = 0; //получить краткое название плагина
    virtual QString getPluDescription() const = 0; //получить подробное описание плагина
    virtual QWidget * getPluWidget() = 0; //получить графическое представление плагина
    virtual QString getPluLoadPath() const = 0; //получить путь, где обнаружен плагин
    virtual QSettings *getPluParameters() = 0; //получить ссылку на параметры плагина
    virtual QSettings *getAppParameters() = 0; //получить ссылку на параметры приложения
    //установить путь и краткое название для плагина
    virtual void setPluLoadPath(const QString &pPath, const QString &pPluName = QString()) = 0;
    //установить ссылку на параметры приложения
    virtual void setAppParameters(QSettings *pSettings) = 0;
    //установить список всех обнаруженных плагинов
    virtual void setPluList(QList<QObject *> *pList) = 0;
    //выполнить целевое действие плагина
    virtual QVariant performPluAction(const QVariant &parameter=QVariant()) = 0;
};

Q_DECLARE_INTERFACE(AnyPluginInterface, "AZ_Soft.Plugins.AnyPluginInterface/3.0")

#endif // __ANYPLUGININTERFACE_H__

