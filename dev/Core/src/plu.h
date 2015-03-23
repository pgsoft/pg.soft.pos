#ifndef PLUCORE_H
#define PLUCORE_H

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "pluCoreInterface.h"
#include "anyPluginInterface.h"
//
class Plu : public QObject, public PluCoreInterface, public AnyPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "AZ_Soft.Plugins.AnyPluginInterface/3.0")
    Q_INTERFACES(AnyPluginInterface PluCoreInterface)

public:
    explicit Plu();
    virtual ~Plu();

public: //AnyPlugin interface
    int getPluType(); //получить тип плагина
    int getPluSortPosition(); //получить позицию для сортировки в списке плагинов
    QString getPluUID() const; //получить уникальный айди плагина
    QString getPluName() const; //получить краткое название плагина
    QString getPluDescription() const; //получить подробное описание плагина
    QWidget * getPluWidget(); //получить графическое представление плагина
    QString getPluLoadPath() const; //получить путь, где обнаружен плагин
    QSettings *getPluParameters(); //получить ссылку на параметры плагина
    QSettings *getAppParameters(); //получить ссылку на параметры приложения
    //установить путь и краткое название для плагина
    void setPluLoadPath(const QString &pPath, const QString &pPluName = QString());
    //установить ссылку на параметры приложения
    void setAppParameters(QSettings *pSettings);
    //установить список всех обнаруженных плагинов
    void setPluList(QList<QObject *> *pList);
    //выполнить целевое действие плагина
    QVariant performPluAction(const QVariant &parameter=QVariant());

public: //PluCore interface
    //вставить запись в таблицу БД
    int insertData(const QString &pTableName
                   , const QSqlRecord &pRecord
                   , QSqlDatabase * = 0);
    //обновить запись в таблице БД
    int updateData(const QString &pTableName
                          , const QSqlRecord &pRecord
                          , const QString &pIDFieldName
                          , QSqlDatabase * = 0);
    //удалить запись в таблице БД
    bool deleteData(const QString &pTableName
                    , const QSqlRecord &pRecord
                    , const QString &pIDFieldName
                    , QSqlDatabase * = 0);
    //получить структуру таблицы БД
    QSqlRecord getStructureOfTable(const QString &pTableName
                                   , QSqlDatabase * pDB = 0);

    //выполнение любого скл-запроса
    bool execSQL(QSqlQuery *q, QWidget *w = NULL, const QString &aboutQuery = QString());
};
#endif
