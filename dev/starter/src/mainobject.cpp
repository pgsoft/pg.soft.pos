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
#include "PluStartupInterface.h"
#include "PluCoreInterface.h"

MainObject::MainObject(QObject *parent) :
    QObject(parent)
{
    apiSettings = NULL;
    pixmap = NULL;
    onloadApiSplashScreen=NULL;
    splashScreenWidth = 400;
    splashScreenHeight = 300;
    mainObjectState=initModelView();
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
    // delete settings object
    if (apiSettings)
    {
        apiSettings->sync();
        delete apiSettings;
        apiSettings=NULL;
    }
    if(pixmap)
    {
        delete pixmap;
        pixmap = NULL;
    }
}

bool MainObject::isOk(){

    return mainObjectState;
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
    pixmap = new QPixmap(":/splash/images/splash.png");
    while (pixmap){

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
        progressBar = new QProgressBar(onloadApiSplashScreen);
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
    apiSettings = new QSettings(qApp->QCoreApplication::applicationFilePath().replace(".exe","")+".ini"
                                ,QSettings::IniFormat
                                ,qApp);
    if (apiSettings){
        apiSettings->setIniCodec(QTextCodec::codecForName("UTF-8"));
        apiSettings->sync();
        //
        QString key;
        /*далее показан шаблон, как определить параметры для приложения
        key="someGroup/someKey";
        if (!apiSettings->contains(key)) apiSettings->setValue(key,QVariant(someTypeSomeDefaultValue));
        */
        key="App/Locale";
        if (!apiSettings->contains(key)) apiSettings->setValue(key,QVariant("en"));
        //синхронизация изменений параметров с файлом параметров
        apiSettings->sync();
        //
        qApp->setProperty("settings",objectToVariant(apiSettings));
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
        progressBar->setVisible(true);
        closeAllPlugins();
        path = qApp->applicationDirPath() + "/" + path;
        rv = moveComponents(path, path);
        if (!rv){
            textError = qApp->property("moveComponentsMessage").toString();
        }
        progressBar->setVisible(false);
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
        QDir pluDir = QDir(qApp->applicationDirPath());
        pluDir.cd(pluPath);
        qApp->setProperty("pluPath",pluDir.absolutePath());
        foreach (QFileInfo pluInfo, pluDir.entryInfoList(QDir::Files))
        {
            QPluginLoader loader(pluInfo.absoluteFilePath(), qApp);
            if (QObject *plugin = loader.instance())
            {
                if (AnyPluginInterface *plu = qobject_cast<AnyPluginInterface *>(plugin))
                {
                    showSplashMessage(QString("%1: <%2>...")
                                      .arg(QObject::tr("Loading"))
                                      .arg(pluInfo.fileName()));
                    plu->setFileName(pluInfo.baseName());
                    pluList.append(plugin);
                    showSplashMessage(QString("%1: <%2>...")
                                      .arg(QObject::tr("Loaded"))
                                      .arg(plu->getName()));
                }
                if (!coreFound && qobject_cast<PluCoreInterface *>(plugin)){
                    qApp->setProperty("pluCore",objectToVariant(plugin));
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

bool MainObject::selectStartupPlugin(){
    bool rv=false; //return value by default = true
    qApp->setProperty("currentPluStartup", QVariant());
    QObject *currentPluStartup=NULL;
    /*
     * поиск и выбор (если больше одного) стартап плагина из общего списка плагинов
    */
    //поиск
    QMap<int, QObject *> startupPluginsMap;
    for (int i=0; i<pluList.size(); ++i){
        if (PluStartupInterface *pluStartup = qobject_cast<PluStartupInterface *>(pluList.at(i))){
            startupPluginsMap.insert(pluStartup->getSortPosition(), pluList.at(i));
        }
    }
    // startupPluginsMap содержит отсоритованный списко ссылок на стартап плагины
    if (startupPluginsMap.size()){
        QObjectList startupPluginsList = startupPluginsMap.values();
        currentPluStartup = startupPluginsList.at(0);
        if (startupPluginsList.size()>1){
            /*to do
             * делаем ГУИ форму для выбора из нескольких стартап плагинов
            */
        }
    }
    if (currentPluStartup){
        rv=true;
        qApp->setProperty("currentPluStartup", objectToVariant(currentPluStartup));
    }
    return rv;
}

bool MainObject::launchStartupPlugin(){
    bool rv=false; //значение возврата - по-умолчанию = false
    //если есть выбранный стартап плагин и его состояние true ,т.е. он готов к работе
    PluStartupInterface *currentPluStartup =
            qobject_cast<PluStartupInterface *>(variantToObject(qApp->property("currentPluStartup")));
    if (currentPluStartup){
        //запуск целевой функции выбранного стартап плагина
        currentPluStartup->slotStart();
        rv=currentPluStartup->getState(); //возвращаем признак успешности запуска плагина
    }
    return rv;
}

void MainObject::hideSplash(){
    if (onloadApiSplashScreen) onloadApiSplashScreen->hide();
}
