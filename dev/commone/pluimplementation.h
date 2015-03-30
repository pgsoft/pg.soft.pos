#ifndef PLUIMPLEMENTATION_CPP
#define PLUIMPLEMENTATION_CPP

Plu::Plu()
{
    initModelView();
}

Plu::~Plu(){

}
QString Plu::getUID() const {
    return property("pluUID").toString();
}

QString Plu::getFileName() const {
    return property("pluFileName").toString();
}
QString Plu::getName() const {
    return property("pluName").toString();
}

QString Plu::getDescription() const {
    return property("pluDescription").toString();
}

QSettings * Plu::getParameters(){
    return qobject_cast<QSettings *>(variantToObject(property("pluParameters")));
}
//установить путь и краткое название для плагина
void Plu::setFileName(const QString &pFileName){
    QString pluLoadPath = qApp->property("pluPath").toString();
    setProperty("pluFileName", pFileName.trimmed());
    QSettings *pluParameters = new QSettings(pluLoadPath+"/"+getFileName()+".ini",QSettings::IniFormat,this);
    if (pluParameters)
    {
        setProperty("pluParameters", objectToVariant(pluParameters));
        //
        QVariantMap keyValueMap;
        keyValueMap.insert("pluName",QVariant(getFileName()));
        keyValueMap.insert("pluDescription",QVariant(getDescription()));
        keyValueMap.insert("pluSortPosition",QVariant(0));
        //
        updateSettings(keyValueMap, pluParameters, this, false);
        //
        setDefaultParameters();
    }
    else
    {
        QMessageBox::warning(0
                             ,tr("Attantion!!!")
                             ,tr("Can not receive parameters of module:\n")+getFileName());
    }
}

#endif // PLUIMPLEMENTATION_CPP
