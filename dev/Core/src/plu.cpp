#include "plu.h"
#include <QMessageBox>
#include "commonefunctions.h"

Plu::Plu()
{
    pluParameters=NULL; // параметры плагина
    pluUID = "core";
    setProperty("core", pluUID);
    if (!addAppTranslator(getUID())){
        QMessageBox::warning(NULL,
                             QObject::tr("Error!"),
                             qApp->property("addAppTranslatorMessage").toString(),
                             QMessageBox::Ok,0,0);
        //break; //ошибка - прервать цикл действий
    }
}

Plu::~Plu(){

}

QString Plu::getUID() const {
    return pluUID;
}

QString Plu::getFileName() const {
    return property("pluFileName").toString();
}
QString Plu::getName() const {
    return property("pluName").toString();
}

QString Plu::getDescription() const {
    return tr("The Core module.");
}

QWidget * Plu::getWidget(){
    return NULL;
}

QSettings * Plu::getParameters(){
    return pluParameters;
}
//установить путь и краткое название для плагина
void Plu::setFileName(const QString &pFileName){
    QString pluLoadPath = qApp->property("pluPath").toString();
    setProperty("pluFileName", pFileName.trimmed());
    pluParameters = new QSettings(pluLoadPath+"/"+getFileName()+".ini",QSettings::IniFormat,this);
    if (pluParameters)
    {
        //
        pluParameters->sync();
        //далее представлен шаблон добавление в ини-файл параметра
        QString key;
        QVariant defaultValue;

        key = "pluName";
        defaultValue =QVariant(getFileName());
        if (!pluParameters->contains(key)) pluParameters->setValue(key,defaultValue);
        pluParameters->sync();
        setProperty("pluName", pluParameters->value(key));
        //
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Attantion!!!")
                             ,tr("Can not receive parameters of module:\n")+getFileName());
    }
}

//выполнить целевое действие плагина
QVariant Plu::launchAction(const QVariant &parameter){
    QVariant rv;
    //
    if (parameter.isValid()){
        //to do something
    }
    //to do something
    //
    return rv;
}

//вставить запись в таблицу БД
int Plu::insertData(const QString &pTableName
                    , const QSqlRecord &pRecord
                    , QSqlDatabase * pDB){
    int lRV=-1; //значение возврата
    //если есть данные в записи и имя таблицы не пустое
    if (!pRecord.isEmpty() && !pTableName.isEmpty())
    {
        //подготовим текстовые фрагменты для скл-запроса на добавление записи
        QString sqlColumns("");
        QString sqlValues("");
        for (int i=0; i < pRecord.count(); i++)
        {
            if (!(pRecord.value(i).isValid())) continue;
            sqlColumns=QString("%1%2\"%3\"")
                    .arg(sqlColumns)
                    .arg(sqlColumns.isEmpty()?"":",")
                    .arg(pRecord.fieldName(i));
            sqlValues=QString("%1%2:p%3")
                    .arg(sqlValues)
                    .arg(sqlValues.isEmpty()?"":",")
                    .arg(pRecord.fieldName(i));
        }
        QSqlQuery query(*pDB);
        query.prepare(QString("INSERT INTO \"%1\" (%2) VALUES (%3)")
                      .arg(pTableName)
                      .arg(sqlColumns)
                      .arg(sqlValues));
        for (int i=0; i < pRecord.count(); i++)
        {
            if (!(pRecord.value(i).isValid())) continue;
            query.bindValue(QString(":p%1").arg(pRecord.fieldName(i)), pRecord.value(i));
        }
        if (execSQL(&query,0,tr("Insert record to table: ")+pTableName))
        {
            query.prepare(QString("SELECT max(rowid) FROM \"%1\"").arg(pTableName));
            if (execSQL(&query) && query.next())
            {
                lRV = query.value(0).toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                                     tr("Error!!!"),
                                     tr("No answer due inserting record to the table: ")+pTableName);
            }
        }
    }
    return lRV;
}

//обновить запись в таблице БД
int Plu::updateData(const QString &pTableName
                           , const QSqlRecord &pRecord
                           , const QString &pIDFieldName
                           , QSqlDatabase * pDB) {
    int lRV=-1;
    QVariant recordIDValue;
    if (!pRecord.isEmpty() && !pTableName.isEmpty())
    {
        QString sqlFields("");
        for (int i=0; i < pRecord.count(); i++)
        {
            if (!(pRecord.value(i).isValid())) continue;
            if (pRecord.fieldName(i) == pIDFieldName) continue;
            sqlFields=QString("%1%2\"%3\"=:p%4")
                    .arg(sqlFields)
                    .arg(sqlFields.isEmpty()?"":",")
                    .arg(pRecord.fieldName(i))
                    .arg(pRecord.fieldName(i));
        }
        QSqlQuery query(*pDB);
        query.prepare(QString("UPDATE \"%1\" SET %2 WHERE (\"%3\"=:pIDFieldValue)")
                      .arg(pTableName)
                      .arg(sqlFields)
                      .arg(pIDFieldName));
        for (int i=0; i < pRecord.count(); i++)
        {
            if (!(pRecord.value(i).isValid())) continue;
            if (pRecord.fieldName(i) == pIDFieldName)
            {
                recordIDValue = pRecord.value(i);
                query.bindValue(":pIDFieldValue", recordIDValue);
                continue;
            }
            query.bindValue(QString(":p%1").arg(pRecord.fieldName(i)), pRecord.value(i));
        }
        if (execSQL(&query,0,tr("Updating of record in the table: ")+pTableName))
        {
            lRV = recordIDValue.toInt();
        }
    }
    return lRV;

}

//удалить запись в таблице БД
bool Plu::deleteData(const QString &pTableName
                     , const QSqlRecord &pRecord
                     , const QString &pIDFieldName
                     , QSqlDatabase * pDB){
    bool lRV=false;
    if (!pRecord.isEmpty() && !pTableName.isEmpty())
    {
        QSqlQuery query(*pDB);
        query.prepare(QString("DELETE FROM \"%1\" WHERE (\"%2\"=:pIDFieldValue)")
                      .arg(pTableName)
                      .arg(pIDFieldName));
        query.bindValue(":pIDFieldValue", pRecord.value(pIDFieldName));
        lRV = execSQL(&query,0,tr("Deleting record in the table: ")+pTableName);
    }
    return lRV;

}


//получить структуру таблицы БД
QSqlRecord Plu::getStructureOfTable(const QString &pTableName, QSqlDatabase *pDB){
    QSqlRecord rV;
    QSqlQuery query(*pDB);
    query.prepare(QString("select * from \"%1\" limit 1").arg(pTableName));
    if (execSQL(&query,0,tr("Getting structure of the table: ")+pTableName))
    {
        rV = query.record();
    }
    return rV;

}

bool Plu::execSQL(QSqlQuery *pQuery, QWidget *parentWidget, const QString & aboutQuery){
    bool rv=false;
    if (pQuery){
        if (pQuery->exec()){
            rv = true;
        }else{
            QMessageBox::warning(parentWidget
                                 ,tr("Attantion!")
                                 ,tr("Error executing SQL")
                                 +QString(
                                     "\n"
                                     "%1\n\n"
                                     "%2\n\n"
                                     "%3")
                                 .arg(aboutQuery).arg(pQuery->lastError().text(), pQuery->executedQuery())
                                 );
        }
    }else{
        QMessageBox::warning(parentWidget
                             ,tr("Attantion!")
                             ,tr("Error executing SQL")
                             +"\n"+aboutQuery+"\n\n"
                             +tr("Empty query!")
                );
    }
    return rv;
}

//Q_EXPORT_PLUGIN2(core,Plu)
