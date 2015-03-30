#ifndef __PLUCOREINTERFACE_H__
#define __PLUCOREINTERFACE_H__

#include <QSqlDatabase>
#include <QString>
#include <QDialog>

class PluCoreInterface
{
public:
    virtual ~PluCoreInterface() {}
    //
public: //функции
    //обновить компоненты программы

    //вставить запись в таблицу БД
    virtual int insertData(const QString &pTableName
                                  , const QSqlRecord &pRecord
                                  , QSqlDatabase * pDB = 0) = 0;
    //обновить запись в таблице БД
    virtual int updateData(const QString &pTableName
                                  , const QSqlRecord &pRecord
                                  , const QString &pIDFieldName
                                  , QSqlDatabase * pDB = 0) = 0;
    //удалить запись в таблице БД
    virtual bool deleteData(const QString &pTableName
                                  , const QSqlRecord &pRecord
                                  , const QString &pIDFieldName
                                  , QSqlDatabase * pDB = 0) =0;
    //получить структуру таблицы БД
    virtual QSqlRecord getStructureOfTable(const QString &pTableName
                                           , QSqlDatabase * pDB = 0) = 0;

    //получить произвольный скл-запрос
    virtual bool execSQL(QSqlQuery *q, QWidget *w = NULL, const QString &aboutQuery = QString()) =0;
    //
    virtual QString decodeMSRCode(const QString &text) = 0;
    virtual void resizeChildToParent(QWidget *child, QWidget *parent) = 0;
    virtual QString getMSRCode(const QString &comment, QWidget *parent) = 0 ;
    virtual QDialog *showShadow(QWidget *parent, double pOpacity=-1.0, const QString &pBackgroundcolor=QString()) = 0;
    virtual QObjectList getPluginList(int pPluType=-1) = 0;
};

Q_DECLARE_INTERFACE(PluCoreInterface, "PG.Soft.POS.Plugins.PluCoreInterface/1.0")

#endif // __PLUCOREINTERFACE_H__
