#include "plu.h"
#include <QMessageBox>

Plu::Plu()
{
    pluLoadPath = ""; //путь, где обнаружен плагин
    appParameters=NULL; //параметры приложения
    pluParameters=NULL; // параметры плагина
    pluList=NULL; //список всех обнаруженных плагинов
    pluFileName = "core";
}

Plu::~Plu(){

}

int Plu::getPluType() //получить тип плагина
{
    return (int) PLU_CORE;
}

int Plu::getPluSortPosition(){
    return 0;
}

QString Plu::getPluUID() const {
    return pluFileName;
}

QString Plu::getPluName() const {
    return pluFileName;
}

QString Plu::getPluDescription() const {
    return tr("Внешний плагин функций Ядра.");
}

QWidget * Plu::getPluWidget(){
    return NULL;
}

QString Plu::getPluLoadPath() const {
    return pluLoadPath;
}

QSettings * Plu::getPluParameters(){
    return pluParameters;
}
QSettings *Plu::getAppParameters(){
    return appParameters;
}
//установить путь и краткое название для плагина
void Plu::setPluLoadPath(const QString &pPath, const QString &pPluName){
    pluLoadPath = pPath;
    pluFileName = pPluName;
    pluParameters = new QSettings(getPluLoadPath()+"/"+getPluName()+".ini",QSettings::IniFormat,this);
    if (pluParameters)
    {
        //
        pluParameters->sync();
        //далее представлен шаблон добавление в ини-файл параметра
        //QString key;
        //QVariant defaultValue;

        //key = "<введите название группы (если необходимо)>/<введите название ключа параметра>";
        //defaultValue =QVariant(<введите значение по-умолчанию нужного типа, совместимого с QVariant>);
        //if (!pluParameters->contains(key)) pluParameters->setValue(key,defaultValue);
        pluParameters->sync();
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Внимание!!!")
                             ,tr("Ошибка получения параметров плагина <%1>!")
                             .arg(getPluName()));
    }
}

//установить ссылку на параметры приложения
void Plu::setAppParameters(QSettings *pSettings){
    appParameters = pSettings;
}

//установить список всех обнаруженных плагинов
void Plu::setPluList(QList<QObject *> *pList){
    pluList = pList;
}
//выполнить целевое действие плагина
QVariant Plu::performPluAction(const QVariant &parameter){
    QVariant rv;
    //
    if (parameter.isValid()){

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
        if (execSQL(&query,0,tr("Добавление записи в таблицу: %1").arg(pTableName)))
        {
            query.prepare(QString("SELECT max(rowid) FROM \"%1\"").arg(pTableName));
            if (execSQL(&query) && query.next())
            {
                lRV = query.value(0).toInt();
            }
            else
            {
                QMessageBox::warning(NULL,
                                     tr("Ошибка!!!"),
                                     tr("Нет ответа при добавлении записи в таблицу: ")+pTableName);
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
        if (execSQL(&query,0,tr("Обновление записи в таблице: %1").arg(pTableName)))
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
        lRV = execSQL(&query,0,tr("Удаление записи в таблице: %1").arg(pTableName));
    }
    return lRV;

}


//получить структуру таблицы БД
QSqlRecord Plu::getStructureOfTable(const QString &pTableName, QSqlDatabase *pDB){
    QSqlRecord rV;
    QSqlQuery query(*pDB);
    query.prepare(QString("select * from \"%1\" limit 1").arg(pTableName));
    if (execSQL(&query,0,tr("Получение структуры таблицы: %1").arg(pTableName)))
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
                                 ,tr("Внимание!")
                                 ,tr("Ошибка при выполнении SQL запроса\n"
                                     "%1\n\n"
                                     "%2\n\n"
                                     "%3")
                                 .arg(aboutQuery).arg(pQuery->lastError().text(), pQuery->executedQuery())
                                 );
        }
    }else{
        QMessageBox::warning(parentWidget
                             ,tr("Внимание!")
                             ,tr("Ошибка при выполнении SQL запроса\n"
                                 "%1\n\nПустой запрос!").arg(aboutQuery)
                             );
    }
    return rv;
}

//Q_EXPORT_PLUGIN2(core,Plu)
