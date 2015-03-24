#include "mainobject.h"
#include <QApplication>
#include <QLabel>
#include <QMessageBox>
#include <QTranslator>
#include <QTextCodec>
#include <QStyleFactory>
#include "commonefunctions.h"

MainObject::MainObject(QObject *parent) :
    QObject(parent)
{
    apiSettings = NULL;
    currentPluCore = NULL;
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

bool MainObject::initModelView(){
    bool rv = false; //значение возврата из функции, пу = false
    //цикл действий
    while (true){
        //локализация - настройка отображения русского текста
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        //параметры приложения
        qobject_cast<QApplication *>(qApp)->setStyle(QStyleFactory::create("plastique")); //стиль графических элементов
        qApp->setApplicationVersion(QObject::tr("1.0"));
        qApp->setOrganizationDomain(QObject::tr("Pancaches.Group"));
        qApp->setOrganizationName(QObject::tr("Pancaches Group Ltd"));
        qApp->setApplicationName(QObject::tr("POS"));

        //create splash widget
        pixmap = new QPixmap(":/splash/images/splash.png");
        if (pixmap){

            onloadApiSplashScreen = new QSplashScreen(pixmap->scaled(QSize(splashScreenWidth, splashScreenHeight)));

            //ручное добавление в заставку нескольких авторских виджетов
            int verOff=20;
            //версия ПО
            QLabel *verLabel = new QLabel(QObject::tr("версия")+" "+qApp->QCoreApplication::applicationVersion()
                                          , onloadApiSplashScreen);
            verLabel->move(verOff, splashScreenHeight-60);
            verLabel->show();
            //владелец ПО
            QLabel *ownerLabel = new QLabel(qApp->QCoreApplication::organizationName()+ QObject::tr(" (c) All rights reserved")
                                            , onloadApiSplashScreen);
            ownerLabel->move(verOff,splashScreenHeight-30);
            ownerLabel->show();

        }
        if (!onloadApiSplashScreen) break; //нет виджета заставки - прервать цикл действий
        onloadApiSplashScreen->show();
        showSplashMessage();
        //
        if (!readParameters()) break; //ошибка - прервать цикл действий
        //
        QTranslator *translator = new QTranslator(this);
        if (translator){
            translator->load(qApp->applicationName()+"_"+apiSettings->value("App/Locale").toString());
            qApp->installTranslator(translator);
        }else{
            break;
        } //ошибка - прервать цикл действий
        //
        showSplashMessage(QObject::tr("Обновление компонентов ПО..."));
        if (!updateComponents()) break; //ошибка - прервать цикл действий
        //
        showSplashMessage(QObject::tr("Загрузка компонентов ПО..."));
        if (!loadPlugins()) break; //ошибка - прервать цикл действий
        //
        //ошибок нет - все ок
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
    currentPluCore = NULL;
}

bool MainObject::updateComponents(){
    bool rv = true; //значение возврата из функции, пу = true
    closeAllPlugins();
    //
    //do update
    //
    return rv;
}

bool MainObject::loadPlugins(bool pShowError){
    qApp->setProperty("loadPluginsMessage",QVariant());
    QString pluPath = "plugins";
    QString textError;
    bool rv = checkAppPath(pluPath);
    if (!rv){
        textError = qApp->property("checkAppPathMessage").toString();
    }else{
        closeAllPlugins();
        QDir pluDir = QDir(qApp->applicationDirPath());
        pluDir.cd(pluPath);
        foreach (QFileInfo pluInfo, pluDir.entryInfoList(QDir::Files))
        {
            //QMessageBox::information(0,"",pluInfo.absoluteFilePath());
            QPluginLoader loader(pluInfo.absoluteFilePath(), qApp);
            if (QObject *plugin = loader.instance())
            {
                if (AnyPluginInterface *plu = qobject_cast<AnyPluginInterface *>(plugin))
                {
                    showSplashMessage(QString("%1 <%2>...")
                                      .arg(QObject::tr("Загрузка компоненты"))
                                      .arg(plu->getPluName()));
                    plu->setPluLoadPath(pluInfo.absolutePath(),pluInfo.baseName());
                    pluList.append(plugin);
                }
                if (!currentPluCore)
                    currentPluCore = qobject_cast<PluCoreInterface *>(plugin);
            }
        }
        for (int i=0; i<pluList.size(); ++i){
            if (AnyPluginInterface *plu = qobject_cast<AnyPluginInterface *>(pluList.at(i)))
            {
                plu->setPluList(&pluList);
                plu->setAppParameters(apiSettings);
            }
        }
        //изменим значение возврата и,если не обнаружен плагин ядра, то и выведем сообщение
        if (!(rv = currentPluCore)){
            textError=QObject::tr("Система не смогла найти модуль ядра!");
        }
    }
    if (!textError.isEmpty()){
        if  (pShowError){
            QMessageBox::critical(NULL,
                                  QObject::tr("Ошибка!"),
                                  textError+QObject::tr("\nСообщите администратору об ошибке!"),
                                  QMessageBox::Ok,0,0);
        }
        qApp->setProperty("loadPluginsMessage",QVariant(textError));
    }
    return rv;
}

bool MainObject::selectStartupPlugin(){
    bool rv=false; //return value by default = true
    currentPluStartup=NULL;
    /*
     * поиск и выбор (если больше одного) стартап плагина из общего списка плагинов
    */
    //поиск
    QMap<int, PluStartupInterface *> startupPluginsMap;
    int sortPosition = 0 ;
    for (int i=0; i<pluList.size(); ++i){
        if (PluStartupInterface *pluStartup = qobject_cast<PluStartupInterface *>(pluList.at(i))){
            sortPosition = 0 ;
            if (AnyPluginInterface *pluAny = qobject_cast<AnyPluginInterface *>(pluList.at(i))){
                sortPosition = pluAny->getPluSortPosition();
            }
            startupPluginsMap.insert(sortPosition, pluStartup);
        }
    }
    // startupPluginsMap содержит отсоритованный списко ссылок на стартап плагины
    if (startupPluginsMap.size()){
        QList<PluStartupInterface *> startupPluginsList = startupPluginsMap.values();
        currentPluStartup = startupPluginsList.at(0);
        if (startupPluginsList.size()>1){
            /*to do
             * делаем ГУИ форму для выбора из нескольких стартап плагинов
            */
        }
    }
    rv = (currentPluStartup ? true : false );
    return rv;
}

bool MainObject::launchStartupPlugin(){
    bool rv=false; //значение возврата - по-умолчанию = false
    //если есть выбранный стартап плагин и его состояние true ,т.е. он готов к работе
    if (currentPluStartup){
        //запуск целевой функции выбранного стартап плагина
        currentPluStartup->slotPluStart();
        rv=currentPluStartup->getPluState(); //возвращаем признак успешности запуска плагина
    }
    return rv;
}

void MainObject::hideSplash(){
    if (onloadApiSplashScreen) onloadApiSplashScreen->hide();
}
