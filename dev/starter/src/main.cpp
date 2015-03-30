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
            /**/
            mainObject->hideSplash();
            //
            if (mainObject->selectUserModule()){
                if (mainObject->launchUserModule()){
                    /*связываем событие закрытия последнего окна со слотом завершения
                    * цикла основного обработчика событий приложения
                    */
                    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
                    //запуск цикла основного обработчика событий приложения
                    appReturnValue = app.exec();
                }
            }/*else{
                textError = QObject::tr("Can not find user module!");
            }*/
        }else{
            textError = QObject::tr("Can not to initialize the main object!");
        }
    }else{
        textError = QObject::tr("Main object not created!");
    }
    //
    if (!textError.isEmpty()){
        QMessageBox::critical(0,
                              QObject::tr("Error!"),
                              textError+"\n"
                                        "Notify your administrator!",
                              QMessageBox::Ok,0,0);
    }
    if (mainObject){
        //удаляем объект основногоо класса
        delete mainObject;
        mainObject=NULL;
    }
    return appReturnValue;
}
