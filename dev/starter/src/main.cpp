#include "mainobject.h"
#include <QApplication>
#include <QTextCodec>
#include <QStyleFactory>
#include <QMessageBox>
//
int main(int argc, char ** argv)
{
    //создание объекта приложения
    QApplication app( argc, argv );
    //локализация - настройка отображения русского текста
    // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    // QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //параметры приложения
    app.setStyle(QStyleFactory::create("plastique")); //стиль графических элементов
    app.setApplicationVersion("1.0");
    app.setOrganizationDomain("Pancaches.Group");
    app.setOrganizationName(QObject::tr("Pancaches Group Ltd"));
    app.setApplicationName("PGPOS");
    //главный объект приложения
    MainObject *mainObject = new MainObject();
    //если объект создан
    int appReturnValue = -1; //значения возврат по результату работы приложения, по-умолчанию = -1
    if (mainObject){
        //если объект готов к работе
        if (mainObject->isOk())
        {
            if (mainObject->selectStartupPlugin()){
                if (mainObject->launchStartupPlugin()){
                    /**/
                    mainObject->hideSplash();
                    /*связываем событие закрытия последнего окна со слотом завершения
                    * цикла основного обработчика событий приложения
                    */
                    //app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
                    //запуск цикла основного обработчика событий приложения
                    appReturnValue = app.exec();
                }else{
                    QMessageBox::critical(0,
                                          QObject::tr("Error!"),
                                          QObject::tr("The system was unable to run a startup plugin!!!\n"
                                                      "Notify your administrator about this error!"),
                                          QMessageBox::Ok,0,0);

                }
            }else{
                QMessageBox::critical(0,
                                      QObject::tr("Error!"),
                                      QObject::tr("The system could not to find any startup plug-in !!!!\n"
                                                  "Notify your administrator about this error!"),
                                      QMessageBox::Ok,0,0);

            }
        }
        //удаляем объект основногоо класса
        delete mainObject;
        mainObject=NULL;
    }
    return appReturnValue;
}
