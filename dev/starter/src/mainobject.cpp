#include "mainobject.h"
#include <QLabel>
#include <QMessageBox>
#include <QTranslator>

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

bool MainObject::initModelView(){
    bool rv = false; //значение возврата из функции, пу = false
    pixmap = new QPixmap(":/splash/splash.png");
    //цикл действий
    while (true){
        //create splash widget
        onloadApiSplashScreen = new QSplashScreen(NULL
                                                  , pixmap->scaled(QSize(splashScreenWidth, splashScreenHeight)
                                                          , Qt::KeepAspectRatio
                                                          , Qt::SmoothTransformation));
        if (!onloadApiSplashScreen) break; //нет виджета заставки - прервать цикл действий
        onloadApiSplashScreen->show();
        qApp->processEvents();
        //
        onloadApiSplashScreen->showMessage(QObject::tr("Чтение параметров..."));
        qApp->processEvents();
        if (!readParameters()) break; //ошибка - прервать цикл действий
        //
        onloadApiSplashScreen->showMessage(QObject::tr("Получение данных с удаленного сервера..."));
        qApp->processEvents();
        if (!downloadComponents()) break; //ошибка - прервать цикл действий
        //
        onloadApiSplashScreen->showMessage(QObject::tr("Обновление компонентов ПО..."));
        qApp->processEvents();
        if (!updateComponents()) break; //ошибка - прервать цикл действий
        //
        onloadApiSplashScreen->showMessage(QObject::tr("Загрузка компонентов ПО..."));
        qApp->processEvents();
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

        //
        QTranslator *translator = new QTranslator(this);
        translator->load(qApp->applicationName()+"_"+apiSettings->value("App/Locale").toString());
        qApp->installTranslator(translator);


        //ручное добавление в заставку нескольких авторских виджетов
        //версия ПО
        QLabel *verLabel = new QLabel(QObject::tr("версия")+" "+qApp->QCoreApplication::applicationVersion(), onloadApiSplashScreen);
        int verOff=20;
        verLabel->move(verOff, (int) (splashScreenHeight*2/3));
        verLabel->resize(splashScreenWidth - verOff*2, verLabel->fontMetrics().height()+4);
        //владелец ПО
        QString ownerText = QString("(c) %1").arg(qApp->QCoreApplication::organizationName());
        QLabel *ownerLabel = new QLabel(ownerText, onloadApiSplashScreen);
        QFont lfont = ownerLabel->font();
        lfont.setBold(true);
        lfont.setPointSize(14);
        ownerLabel->setFont(lfont);
        ownerLabel->move(1,30);
        ownerLabel->resize(splashScreenWidth-2, 30);
        ownerLabel->setAlignment(Qt::AlignHCenter);
        ownerLabel->show();
        rv = true;
    }
    return rv;
}

bool MainObject::downloadComponents(){
    bool rv = true; //значение возврата из функции, пу = true
    //пока заглушка
    return rv;
}

bool MainObject::uploadData(){
    bool rv = true; //значение возврата из функции, пу = true
    //пока заглушка
    /*ayk todo: создание отдельного объекта класса, который
     * в параллельном потоке с определенной цикличностью,
     * возможно заданной в параметрах приложения выполняет отправку
     * в определенном формате накопленных данных */
    return rv;
}


void MainObject::closeAllPlugins(){
    //close all plugins and clear plugins list
    for (int i=0; i<pluList.size(); ++i){
        delete pluList[i];
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

bool MainObject::loadPlugins(){
    bool rv = true; //значение возврата из функции, пу = true
    QDir pluDir = QDir(qApp->applicationDirPath());
    pluDir.cd("plugins");
    foreach (QFileInfo pluInfo, pluDir.entryInfoList(QDir::Files))
    {
        //QMessageBox::information(0,"",pluInfo.absoluteFilePath());
        QPluginLoader loader(pluInfo.absoluteFilePath(), qApp);
        if (QObject *plugin = loader.instance())
        {
            if (AnyPluginInterface *plu = qobject_cast<AnyPluginInterface *>(plugin))
            {
                onloadApiSplashScreen->showMessage(QString("%1 <%2>...").arg(QObject::tr("Загрузка компоненты")).arg(plu->getPluName()));
                qApp->processEvents();
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
        QMessageBox::critical(onloadApiSplashScreen,
                              QObject::tr("Критическая ошибка!"),
                              QObject::tr("Система не смогла найти модуль ядра!!!\n"
                                          "Сообщите администратору об ошибке!"),
                              QMessageBox::Ok,0,0);
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
