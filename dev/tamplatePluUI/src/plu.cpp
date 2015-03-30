#include "plu.h"
#include <QMessageBox>
#include "commonefunctions.h"
#include "enums.h"
//part of Plu class standard implementation
#include "pluimplementation.h"
//

void Plu::initModelView(){
    setProperty("pluType", QVariant(PLU_USERMODULE));
    setProperty("pluUID", "<name of module>");
    setProperty("pluName", getUID());
    setProperty("pluDescription", tr("Module: ")+getUID());
    setProperty("pluFileName", QVariant(""));
    setProperty("pluParameters", QVariant(0));
    setProperty("pluState", QVariant(0));

    //add plu translation
    bool showErrorOfTranslation=false;
    QSettings *appSettings = qobject_cast<QSettings *>(variantToObject(qApp->property("settings")));
    if (appSettings){
        showErrorOfTranslation =appSettings->value("App/ShowErrorOfTranslation").toBool();
    }
    if (!addAppTranslator(getUID()) && showErrorOfTranslation){
        QMessageBox::warning(NULL,
                             QObject::tr("Error!"),
                             qApp->property("addAppTranslatorMessage").toString(),
                             QMessageBox::Ok,0,0);
        //break; //ошибка - прервать цикл действий
    }    setProperty("pluWidget", objectToVariant(getWidget()));
    setProperty("pluState", QVariant(1));

}
void Plu::setDefaultParameters(){
    QSettings *pluParameters = getParameters();
    if (pluParameters)
    {
        QVariantMap keyValueMap;
        //keyValueMap.insert("<key>",QVariant(<defaultValue>));
        //
        updateSettings(keyValueMap, pluParameters, this, true);
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Attantion!!!")
                             ,tr("Can not receive parameters of module:\n")+getFileName());
    }
    //
    setProperty("pluWidget", objectToVariant(getWidget()));

}
void Plu::setSortPosition(int pSortPosition){
    QSettings *pluParameters = getParameters();
    if (pluParameters)
    {
        QVariantMap keyValueMap;
        keyValueMap.insert("pluSortPosition",QVariant(pSortPosition));
        updateSettings(keyValueMap, pluParameters, this, true);
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Attantion!!!")
                             ,tr("Can not receive parameters of module:\n")+getFileName());
    }
}

int Plu::getSortPosition(){
    return property("pluSortPosition").toInt();
}

int Plu::getState(){
    return property("pluState").toInt();
}

QWidget *Plu::getWidget()
{
    QWidget *rv=NULL;
    //TO DO create QWidget
    return rv;
}

bool Plu::show(const QVariant &parameter){
    bool rv=false;
    //
    if (parameter.isValid()){
        //TO DO something
    }
    //TO DO create user window ets
    //
    return rv;
}