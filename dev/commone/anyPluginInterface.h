#ifndef __ANYPLUGININTERFACE_H__
#define __ANYPLUGININTERFACE_H__

#include <QString>
#include <QSettings>
#include <QList>

class AnyPluginInterface
{
protected:
    QSettings * pluParameters; // параметры плагина
    QString pluUID; //внутренний идентификатор плагина
public:
    virtual ~AnyPluginInterface() {}
    //
public:
    virtual QString getUID() const = 0; //получить уникальный айди плагина
    virtual QString getFileName() const = 0; //получить имя файла плагина без расширения
    virtual QString getName() const = 0; //получить краткое название плагина
    virtual QString getDescription() const = 0; //получить подробное описание плагина
    virtual QWidget * getWidget() = 0; //получить графическое представление плагина
    virtual QSettings *getParameters() = 0; //получить ссылку на параметры плагина
    //
    virtual void setFileName(const QString &) = 0; //получить ссылку на параметры плагина
    //выполнить целевое действие плагина
    virtual QVariant launchAction(const QVariant &parameter=QVariant()) = 0;
};

Q_DECLARE_INTERFACE(AnyPluginInterface, "PG.Soft.Plugins..AnyPluginInterface/1.0")

#endif // __ANYPLUGININTERFACE_H__

