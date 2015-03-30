/*
основной класс, реализующий все подготовительные
операции по выбору и запуску стартап плагина
*/


#ifndef MAINOBJECT_H
#define MAINOBJECT_H

#include <QObject>
#include <QSplashScreen>
#include <QPixmap>
#include <QDir>
#include <QSettings>
class MainObject : public QObject
{
    Q_OBJECT
public:
    explicit MainObject(QObject *parent = 0);
    virtual ~MainObject();

private:
//vars members
    QSplashScreen *onloadApiSplashScreen; //виджет-заставка для отображения процесса запуска ПО
    QObjectList pluList; //список ссылок на все обнаруженные плагины

    //functions members
    bool readParameters();
    bool updateComponents(bool pShowError=true); //функция, выполняющая обновление компонентов ПО
    bool loadPlugins(bool pShowError=true); //поиска и загрузка плагинов
    bool initModelView(); //инициализация объекта
    void closeAllPlugins();
    void showSplashMessage(const QString &pText=QString());
    bool createSplash();
    bool moveComponents(const QString &pStartExchangePath, const QString &pCurrentExchangePath);
public:
    bool isOk(); //получить общее состояние объекта true=все ок, false = есть проблемы
    bool selectUserModule(bool pShowError=true); //выбор стартап плагина
    bool launchUserModule(); //запуск выбранного стартап плагина
    void hideSplash();
signals:

public slots:

};

#endif // MAINOBJECT_H
