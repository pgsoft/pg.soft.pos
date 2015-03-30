#include "dialogselectusermodule.h"
#include "ui_dialogselectusermodule.h"
#include "commonefunctions.h"

DialogSelectUserModule::DialogSelectUserModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectUserModule)
{
    ui->setupUi(this);
    Qt::WindowFlags flags =   Qt::CustomizeWindowHint | Qt::FramelessWindowHint; ;
    setWindowFlags(flags);
    selectedUserModuleObject=NULL;
}

DialogSelectUserModule::~DialogSelectUserModule()
{
    delete ui;
}

void DialogSelectUserModule::addUserModuleWidget(QWidget *pWidget){
    if (pWidget){
        connect(pWidget, SIGNAL(clicked()), this, SLOT(userModuleWidget_clicked()));
        ui->horizontalLayoutMain->insertWidget(1,pWidget);
        pWidget->setStyleSheet(pWidget->styleSheet());
        pWidget->show();
    }
}

QObject *DialogSelectUserModule::getSelectedUserModuleObject(){
    return selectedUserModuleObject;
}

void DialogSelectUserModule::userModuleWidget_clicked(){
    selectedUserModuleObject = variantToObject(sender()->property("pluginObject"));
    accept();
}
