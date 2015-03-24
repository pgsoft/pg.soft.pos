/*
основной класс, реализующий все подготовительные
операции по выбору и запуску стартап плагина
*/


#ifndef MAINOBJECT_H
#define MAINOBJECT_H

#include <QObject>
#include "AnyPluginInterface.h"
#include "PluStartupInterface.h"
#include "PluCoreInterface.h"
#include <QSplashScreen>
#include <QPixmap>


class MainObject : public QObject
{
    Q_OBJECT
public:
    explicit MainObject(QObject *parent = 0);
    virtual ~MainObject();

private:
    QPixmap *pixmap;
    //vars members
    QSplashScreen *onloadApiSplashScreen; //виджет-заставка для отображения процесса запуска ПО
    QSettings *apiSettings; //ссылка на объект параметров приложения
    bool mainObjectState; //состояние главного объекта
    QList<QObject *> pluList; //список ссылок на все обнаруженные плагины
    PluCoreInterface *currentPluCore; //ссылка на объект плагина ядра
    PluStartupInterface * currentPluStartup;//ссылка на объек выбранного стартап плагина
    int splashScreenWidth; //ширина виджета заставки
    int splashScreenHeight; //высота виджета заставкик

    //functions members
    bool readParameters();
    bool updateComponents(); //функция, выполняющая обновление компонентов ПО
    bool loadPlugins(bool pShowError=true); //поиска и загрузка плагинов
    bool initModelView(); //инициализация объекта
    void closeAllPlugins();
    void showSplashMessage(const QString &pText=QString());
public:
    bool isOk(); //получить общее состояние объекта true=все ок, false = есть проблемы
    bool selectStartupPlugin(); //выбор стартап плагина
    bool launchStartupPlugin(); //запуск выбранного стартап плагина
    void hideSplash();
signals:

public slots:

};

#endif // MAINOBJECT_H
