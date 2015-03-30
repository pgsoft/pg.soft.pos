#include "mainobject.h"
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QTextCodec>
#include <QStyleFactory>
#include "commonefunctions.h"
#include <QScopedPointer>
#include <QProgressBar>
#include <QDebug>
#include "AnyPluginInterface.h"
#include "PluUserModuleInterface.h"
#include "PluCoreInterface.h"
#include "enums.h"
#include "dialogselectusermodule.h"

#define _PROGRESSBAR qobject_cast<QProgressBar *>(variantToObject(property("progressBar")))
MainObject::MainObject(QObject *parent) :
    QObject(parent)
{
    onloadApiSplashScreen=NULL;
    setProperty("mainObjectState", QVariant(initModelView()));
}

MainObject::~MainObject()
{
    closeAllPlugins();
    // delete splash screen window object
    if (onloadApiSplashScreen)
    {
        delete onloadApiSplashScreen;
        onloadApiSplashScreen=NULL;
    }
}

bool MainObject::isOk(){

    return property("mainObjectState").toBool();
}
void MainObject::showSplashMessage(const QString &pText){
    if(onloadApiSplashScreen){
        onloadApiSplashScreen->showMessage(pText,Qt::AlignLeft,Qt::white);
        qApp->processEvents();
    }
}

bool MainObject::createSplash(){
    bool rv=false;
    //create splash widget
    QPixmap *pixmap = new QPixmap(":/splash/images/splash.png");
    while (pixmap){
        int splashScreenWidth=400; //ширина виджета заставки
        int splashScreenHeight=300; //высота виджета заставкик
        onloadApiSplashScreen = new QSplashScreen(pixmap->scaled(QSize(splashScreenWidth, splashScreenHeight)));
        if (!onloadApiSplashScreen) break;
        //ручное добавление в заставку нескольких авторских виджетов
        int verOff=20;
        //версия ПО
        QLabel *label = new QLabel("ver. "+qApp->QCoreApplication::applicationVersion()
                                   , onloadApiSplashScreen);
        if (!label) break;
        label->move(verOff, splashScreenHeight-60);
        label->show();
        //владелец ПО
        label = new QLabel(qApp->QCoreApplication::organizationName()+ " (c) All rights reserved"
                           , onloadApiSplashScreen);
        if (!label) break;
        label->move(verOff,splashScreenHeight-30);
        label->show();
        //progress bar
        QProgressBar *progressBar = new QProgressBar(onloadApiSplashScreen);
        if (!progressBar) break;
        setProperty("progressBar", objectToVariant(progressBar));
        progressBar->setStyleSheet(QLatin1String("QProgressBar {\n"
                                                 "  border: 0px;\n"
                                                 "	background-color: transparent;\n"
                                                 "	height: 5px;\n"
                                                 "	margin: 4px;\n"
                                                 "}\n"
                                                 "\n"
                                                 "QProgressBar::chunk {\n"
                                                 "    \n"
                                                 "	background-color: rgb(255, 255, 0);\n"
                                                 "    width: 1px;\n"
                                                 "}"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Horizontal);
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);
        progressBar->setValue(0);
        progressBar->resize((int)splashScreenWidth/3,5);
        progressBar->move(5,30);
        progressBar->setVisible(false);
        //
        rv=true;
        onloadApiSplashScreen->show();
        showSplashMessage();
        break;
    }
    return rv;
}

bool MainObject::initModelView(){
    bool rv = false; //значение возврата из функции, пу = false
    //цикл действий
    while (true){
        //локализация - настройка отображения русского текста
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        //параметры приложения
        qobject_cast<QApplication *>(qApp)->setStyle(QStyleFactory::create("plastique")); //стиль графических элементов
        qApp->setApplicationVersion("1.0");
        qApp->setOrganizationDomain("Pancaches.Group");
        qApp->setOrganizationName("Pancaches Group Ltd");

        if (!createSplash()) break; //нет виджета заставки - прервать цикл действий
        //
        if (!readParameters()) break; //ошибка - прервать цикл действий
        //
        if (!addAppTranslator(qApp->applicationName())){
            QMessageBox::warning(NULL,
                                 QObject::tr("Error!"),
                                 qApp->property("addAppTranslatorMessage").toString(),
                                 QMessageBox::Ok,0,0);
        }
        //
        showSplashMessage(QObject::tr("Updating of components..."));
        if (!updateComponents()) break; //ошибка - прервать цикл действий
        //
        showSplashMessage(QObject::tr("Loading of components..."));
        if (!loadPlugins()) break; //ошибка - прервать цикл действий
        //
        //ошибок нет - все ок
        showSplashMessage(QObject::tr("Starting..."));
        rv=true;
        //конец цикла действий
        break;
    }
    return rv;
}
bool MainObject::readParameters(){
    bool rv = false; //значение возврата из функции, пу = false
    //создаем объект управления параметрами приложения
    QSettings *apiSettings = new QSettings(qApp->QCoreApplication::applicationFilePath().replace(".exe","")+".ini"
                                ,QSettings::IniFormat
                                ,this);
    if (apiSettings){
        qApp->setProperty("settings",objectToVariant(apiSettings));
        apiSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
        //
        QVariantMap keyValueMap;
        keyValueMap.insert("App/Locale",QVariant("en"));
        keyValueMap.insert("App/ShowErrorOfTranslation",QVariant(true));
        //
        updateSettings(keyValueMap,apiSettings);
        //
        rv = true;
    }
    return rv;
}


void MainObject::closeAllPlugins(){
    //close all plugins and clear plugins list
    for (int i=0; i<pluList.size(); ++i){
        if (pluList[i]) delete pluList[i];
        pluList[i]=NULL;
    }
    pluList.clear();
    qApp->setProperty("pluCore",QVariant());
}

bool MainObject::moveComponents(const QString &pStartExchangePath, const QString &pCurrentExchangePath){
    bool rv = true;
    qApp->setProperty("moveComponentsMessage",QVariant());
    QString relativePathFile,oldFileName,newFilename,tmpFilename, textError;
    QDir dir(pCurrentExchangePath);
    QFileInfoList curDirFileInfoList =
            dir.entryInfoList(
                QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot|QDir::NoSymLinks
                ,QDir::DirsFirst);
    foreach (QFileInfo fInfo, curDirFileInfoList)
    {
        relativePathFile = oldFileName = newFilename = fInfo.absoluteFilePath();

        relativePathFile.replace(pStartExchangePath,"");
        showSplashMessage(QString("%1:\n<%2>...")
                          .arg(QObject::tr("Updating"))
                          .arg(relativePathFile));
        if (fInfo.isDir()){
            if (!(rv = checkAppPath(relativePathFile))) {
                textError = QObject::tr("Can not create relative path:")+relativePathFile;
                break;
            }
            if (!(rv = moveComponents(pStartExchangePath, oldFileName))) { break; }
        }else{
            //
            newFilename.replace(pStartExchangePath,qApp->applicationDirPath());
            if (!newFilename.contains(qApp->applicationFilePath(),Qt::CaseInsensitive)){
                tmpFilename="";
                if (QFile::exists(newFilename)){
                    tmpFilename = newFilename+QDateTime::currentDateTime().toString(".yyyyMMdd.hh.mm.ss.RZV");
                    if (!(rv = QFile::rename(newFilename, tmpFilename))){
                        textError = QObject::tr("Can not rename file:")+newFilename
                                +QObject::tr("\nto file:")+tmpFilename;
                        break;
                    }
                }
                if (!(rv = QFile::rename(oldFileName, newFilename))){
                    textError = QObject::tr("Can not rename file:")+oldFileName
                            +QObject::tr("\nto file:")+newFilename;
                    break;
                }
                if (!tmpFilename.isEmpty()){
                    if (!(rv = QFile::remove(tmpFilename))){
                        textError = QObject::tr("Can not remove tmp file:")+tmpFilename;
                        break;
                    }
                }
            }
        }
        if (!rv) break;
    }
    if (!textError.isEmpty()){
        qApp->setProperty("moveComponentsMessage",QVariant(textError));
    }
    return rv;
}

bool MainObject::updateComponents(bool pShowError){
    qApp->setProperty("updateComponentsMessage",QVariant());
    QString path = "exchange/pos/server";
    QString textError;
    bool rv = checkAppPath(path);
    if (!rv){
        textError = qApp->property("checkAppPathMessage").toString();
    }else{
        _PROGRESSBAR->setVisible(true);
        closeAllPlugins();
        path = qApp->applicationDirPath() + "/" + path;
        rv = moveComponents(path, path);
        if (!rv){
            textError = qApp->property("moveComponentsMessage").toString();
        }
        _PROGRESSBAR->setVisible(false);
    }
    if (!textError.isEmpty()){
        if  (pShowError){
            QMessageBox::critical(NULL,
                                  QObject::tr("Error!"),
                                  textError+QObject::tr("\nNotify your administrator!"),
                                  QMessageBox::Ok,0,0);
        }
        qApp->setProperty("updateComponentsMessage",QVariant(textError));
    }   //
    return rv;
}

bool MainObject::loadPlugins(bool pShowError){
    qApp->setProperty("loadPluginsMessage",QVariant());
    QString pluPath = "plugins";
    QString textError;
    bool rv = checkAppPath(pluPath);
    bool coreFound=false;
    if (!rv){
        textError = qApp->property("checkAppPathMessage").toString();
    }else{
        closeAllPlugins();
        QDir dir = QDir(qApp->applicationDirPath());
        dir.cd(pluPath);
        qApp->setProperty("pluPath",dir.absolutePath());
        foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
        {
            QPluginLoader loader(fileInfo.absoluteFilePath(), qApp);
            if (QObject *pluginObject = loader.instance())
            {
                showSplashMessage(QString("%1:\n<%2>...")
                                  .arg(QObject::tr("Loading"))
                                  .arg(fileInfo.fileName()));
                pluList.append(pluginObject);
                if (AnyPluginInterface *anyPlugin = qobject_cast<AnyPluginInterface *>(pluginObject))
                {
                    anyPlugin->setFileName(fileInfo.baseName());
                }
                if (!coreFound && pluginObject->property("pluType").toInt()==PLU_CORE){
                    qApp->setProperty("pluCore",objectToVariant(pluginObject));
                    coreFound=true;
                }
            }
        }
        //изменим значение возврата и,если не обнаружен плагин ядра, то и выведем сообщение

        if (!coreFound){
            rv = false;
            textError=QObject::tr("The Core module not found!");
        }
    }
    if (!textError.isEmpty()){
        if  (pShowError){
            QMessageBox::critical(NULL,
                                  QObject::tr("Error!"),
                                  textError+QObject::tr("\nNotify your administrator!"),
                                  QMessageBox::Ok,0,0);
        }
        qApp->setProperty("loadPluginsMessage",QVariant(textError));
    }
    QVariant v;
    v.setValue<QObjectList>(pluList);
    qApp->setProperty("pluList",v);
    return rv;
}

bool MainObject::selectUserModule(bool pShowError){
    bool rv=false; //return value by default = true
    qApp->setProperty("userModule", QVariant());
    QObject *curPluUserModule=NULL;
    QString textError;
    /*
     * поиск и выбор (если больше одного) стартап плагина из общего списка плагинов
    */
    //поиск
    QMap<int, QObject *> pluginsMap;
    for (int i=0; i<pluList.size(); ++i){
        if (pluList.at(i)->property("pluType").toInt() == PLU_USERMODULE){
            pluginsMap.insertMulti(pluList.at(i)->property("pluSortPosition").toInt(), pluList.at(i));
        }
    }
    // startupPluginsMap содержит отсоритованный списко ссылок на стартап плагины
    if (pluginsMap.size()){
        QObjectList userModulePluginsList = pluginsMap.values();
        QScopedPointer<DialogSelectUserModule> d(new DialogSelectUserModule());
        if (!d.isNull()){
            for (int i=0; i<userModulePluginsList.size(); ++i){
                d->addUserModuleWidget(
                            qobject_cast<QWidget *>(
                                variantToObject(
                                    userModulePluginsList.at(i)->property("pluWidget"))));
            }
            d->showMaximized();
            if (d->exec() == QDialog::Accepted){
                curPluUserModule = d->getSelectedUserModuleObject();
            }
        }else{
            textError=tr("Can not create window for selecting user module!");
        }
    }
    if (curPluUserModule){
        rv=true;
        qApp->setProperty("userModule", objectToVariant(curPluUserModule));
    }
    if (!textError.isEmpty()){
        if  (pShowError){
            QMessageBox::critical(NULL,
                                  QObject::tr("Error!"),
                                  textError+QObject::tr("\nNotify your administrator!"),
                                  QMessageBox::Ok,0,0);
        }
        qApp->setProperty("selectUserModuleMessage",QVariant(textError));
    }
    return rv;
}

bool MainObject::launchUserModule(){
    bool rv=false; //значение возврата - по-умолчанию = false
    //если есть выбранный стартап плагин и его состояние true ,т.е. он готов к работе
    PluUserModuleInterface *curPluUserModule =
            qobject_cast<PluUserModuleInterface *>(variantToObject(qApp->property("userModule")));
    if (curPluUserModule){
        //запуск целевой функции выбранного стартап плагина
        rv = curPluUserModule->show();
    }
    return rv;
}

void MainObject::hideSplash(){
    if (onloadApiSplashScreen) onloadApiSplashScreen->hide();
}
