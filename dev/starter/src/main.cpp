#include "mainobject.h"
#include <QApplication>
#include <QMessageBox>
//
int main(int argc, char ** argv)
{
    //создание объекта приложения
    QApplication app( argc, argv );
    //главный объект приложения
    MainObject *mainObject = new MainObject();
    //если объект создан
    int appReturnValue = -1; //значения возврат по результату работы приложения, по-умолчанию = -1
    QString textError;
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
                    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
                    //запуск цикла основного обработчика событий приложения
                    appReturnValue = app.exec();
                }else{
                    textError = QObject::tr("Система не смогла запустить пользовательский модуль!");
                }
            }else{
                textError = QObject::tr("Система не нашла пользовательский модуль!");
            }
        }else{
            textError = QObject::tr("Не инициализирован главный объект!");
        }
    }else{
        textError = QObject::tr("Не создан главный объект!");
    }
    //
    if (!textError.isEmpty()){
        QMessageBox::critical(0,
                              QObject::tr("Ошибка!"),
                              QObject::tr("%1\n"
                                          "Сообщите вашему администратору!").arg(textError),
                              QMessageBox::Ok,0,0);
    }
    if (mainObject){
        //удаляем объект основногоо класса
        delete mainObject;
        mainObject=NULL;
    }
    return appReturnValue;
}
