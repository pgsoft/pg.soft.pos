#include "usermodulewidget.h"
#include "ui_usermodulewidget.h"
#include "commonefunctions.h"
#include <QToolTip>
#include <QPainter>

UserModuleWidget::UserModuleWidget(const QString &pName
                                   , const QString &pDescription
                                   , QPixmap *pPixmap
                                   , QObject *pPluginObject
                                   , QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserModuleWidget)
{
    ui->setupUi(this);
    ui->labelName->setText(pName);
    ui->pbDescription->setToolTip(pDescription);
    if (pPixmap){
        ui->pbPixmap->setIcon(QIcon(*pPixmap));
    }
    setProperty("pluginObject",objectToVariant(pPluginObject));
    connect(ui->pbPixmap,SIGNAL(clicked()), this, SIGNAL(clicked()));
}

UserModuleWidget::~UserModuleWidget()
{
    delete ui;
}

void UserModuleWidget::on_pbDescription_clicked(){
    QPushButton *b=ui->pbDescription;
    QToolTip::showText(b->mapToGlobal(QPoint(0,0))
                       , b->toolTip()
                       , b
                       , QRect()
                       , 5000);
}

void UserModuleWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
