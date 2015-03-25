#ifndef __PLUCOREINTERFACE_H__
#define __PLUCOREINTERFACE_H__

#include <QSqlDatabase>
#include <QString>

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

};

Q_DECLARE_INTERFACE(PluCoreInterface, "PG.Soft.Plugins..PluCoreInterface/1.0")

#endif // __PLUCOREINTERFACE_H__
