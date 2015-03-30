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
    Q_PLUGIN_METADATA(IID "PG.Soft.POS.Plugins.AnyPluginInterface/1.0")
    Q_PLUGIN_METADATA(IID "PG.Soft.POS.Plugins.PluCoreInterface/1.0")
    Q_INTERFACES(AnyPluginInterface PluCoreInterface)

public:
    explicit Plu();
    virtual ~Plu();

public: //AnyPlugin interface
    QString getUID() const; //получить уникальный айди плагина
    QString getFileName() const; //получить имя файла плагина без расширения
    QString getName() const; //получить краткое название плагина
    QString getDescription() const; //получить подробное описание плагина
    QWidget *getWidget(); //получить графическое представление плагина
    QSettings *getParameters(); //получить ссылку на параметры плагина
    //установить путь и краткое название для плагина
    void setFileName(const QString &pFileName = QString());
    void setDefaultParameters();
private:
    void initModelView();

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
    QString decodeMSRCode(const QString &text);
    void resizeChildToParent(QWidget *child, QWidget *parent);
    QString getMSRCode(const QString &comment, QWidget *parent);
    QDialog *showShadow(QWidget *parent, double pOpacity=-1.0, const QString &pBackgroundcolor=QString());
    QObjectList getPluginList(int pPluType=-1);
};
#endif
