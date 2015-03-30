#ifndef PLU_H
#define PLU_H

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "PluUserModuleInterface.h"
#include "anyPluginInterface.h"
//
class Plu : public QObject, public PluUserModuleInterface, public AnyPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PG.Soft.Plugins.AnyPluginInterface/1.0")
    Q_PLUGIN_METADATA(IID "PG.Soft.Plugins.PluUserModuleInterface/1.0")
    Q_INTERFACES(AnyPluginInterface PluUserModuleInterface)

public:
    explicit Plu();
    virtual ~Plu();

public: //AnyPlugin interface
    QString getUID() const; //получить уникальный айди плагина
    QString getFileName() const; //получить имя файла плагина без расширения
    QString getName() const; //получить краткое название плагина
    QString getDescription() const; //получить подробное описание плагина
    QSettings *getParameters(); //получить ссылку на параметры плагина
    //установить путь и краткое название для плагина
    void setFileName(const QString &pFileName = QString());
    void setDefaultParameters();
private:
    void initModelView();

public: //PluUserModuleInterface
    QWidget *getWidget(); //получить графическое представление плагина
    int getSortPosition(); //получить позицию для сортировки в списке плагинов
    void setSortPosition(int);
    int getState();
    bool show(const QVariant &parameter=QVariant());

};
#endif //PLU_H
