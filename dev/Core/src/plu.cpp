#include "plu.h"
#include <QMessageBox>
#include "commonefunctions.h"
#include "dialoggetmsrcode.h"
#include "enums.h"

//part of Plu class standard implementation
#include "pluimplementation.h"
//

void Plu::setDefaultParameters(){
    QSettings *pluParameters = getParameters();
    if (pluParameters)
    {
        setProperty("pluParameters", objectToVariant(pluParameters));
        //
        QVariantMap keyValueMap;
        //keyValueMap.insert("<key>",QVariant(<defaultValue>));
        //
        updateSettings(keyValueMap, pluParameters, this);
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Attantion!!!")
                             ,tr("Can not receive parameters of module:\n")+getFileName());
    }

}

void Plu::initModelView(){
    setProperty("pluType", QVariant(PLU_CORE));
    setProperty("pluUID", "core");
    setProperty("pluName", getUID());
    setProperty("pluDescription", tr("Module: ")+getUID());
    setProperty("pluFileName", QVariant(""));
    setProperty("pluParameters", QVariant(0));
    setProperty("pluState", QVariant(0));

    //add plu translation
    if (!addAppTranslator(getUID())){
        QMessageBox::warning(NULL,
                             QObject::tr("Error!"),
                             qApp->property("addAppTranslatorMessage").toString(),
                             QMessageBox::Ok,0,0);
        //break; //ошибка - прервать цикл действий
    }
    setProperty("pluState", QVariant(1));
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

QString Plu::decodeMSRCode(const QString &text){
    QString code( text.trimmed() );
    if (!code.isEmpty()){
        QStringList list = code.split(";");
        if (list.size()){
            if (list.size()>1){
                code = list.at(1);
            } else {
                code = list.at(0);
            }
        }
    }
    return code;
}

QDialog *Plu::showShadow(QWidget *parent, double pOpacity, const QString &pBackgroundcolor){
    QDialog *d = new QDialog(parent);
    Qt::WindowFlags flags =  Qt::Tool | Qt::CustomizeWindowHint | Qt::FramelessWindowHint; ;
    d->setWindowFlags(flags);
    d->setWindowOpacity( pOpacity == -1 ? 0.9 : pOpacity );
    d->setStyleSheet(QString("QDialog{"
                             "\n	background-color: %1;"
                             "\n}")
                     .arg(pBackgroundcolor.isEmpty()? "rgb(0,0,0)" : pBackgroundcolor ));
    d->show();
    resizeChildToParent(d, parent);
    d->activateWindow();
    return d;
}

QString Plu::getMSRCode(const QString &comment, QWidget *parent){
    QString rv;
    QScopedPointer<QDialog> shadow(showShadow(parent));
    if (shadow){
        QScopedPointer<DialogGetMSRCode> d(new DialogGetMSRCode(comment, parent));
        if (d){
            d->show();
            d->activateWindow();
            if (d->exec()){
                rv = decodeMSRCode(d->getCode());
            }
        }
    }
    return rv;
}
void Plu::resizeChildToParent(QWidget *child, QWidget *parent){
    if (!parent || !child) return;
    int difH=child->frameGeometry().height() - child->rect().height();
    int difW=child->frameGeometry().width() - child->width();
    QSize r = parent->size();
    child->resize(r.width()-difW, r.height() - difH);
    child->move(parent->mapToGlobal(QPoint(0,0)));
}

QObjectList Plu::getPluginList(int pPluType){
    QObjectList rv;
    QVariant v=qApp->property("pluList");
    if (v.canConvert<QObjectList>()){
        QObjectList pluList = v.value<QObjectList>();
        if (pPluType==-1){
            rv=pluList;
        }else{
            for(int i=0;i<pluList.size();++i){
                if (pluList.at(i)->property("pluType").toInt()==pPluType){
                    rv.append(pluList.at(i));
                }
            }
        }
    }
    return rv;
}
