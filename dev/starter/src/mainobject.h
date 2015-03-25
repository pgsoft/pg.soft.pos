/*
основной класс, реализующий все подготовительные
операции по выбору и запуску стартап плагина
*/


#ifndef MAINOBJECT_H
#define MAINOBJECT_H

#include <QObject>
#include <QSplashScreen>
#include <QPixmap>
#include <QProgressBar>
#include <QDir>
#include <QSettings>
class MainObject : public QObject
{
    Q_OBJECT
public:
    explicit MainObject(QObject *parent = 0);
    virtual ~MainObject();

private:
    QPixmap *pixmap;
    QProgressBar *progressBar;
//vars members
    QSplashScreen *onloadApiSplashScreen; //виджет-заставка для отображения процесса запуска ПО
    QSettings *apiSettings; //ссылка на объект параметров приложения
    bool mainObjectState; //состояние главного объекта
    QObjectList pluList; //список ссылок на все обнаруженные плагины
    int splashScreenWidth; //ширина виджета заставки
    int splashScreenHeight; //высота виджета заставкик

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
    bool selectStartupPlugin(); //выбор стартап плагина
    bool launchStartupPlugin(); //запуск выбранного стартап плагина
    void hideSplash();
signals:

public slots:

};

#endif // MAINOBJECT_H
