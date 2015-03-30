#include "dialoggetmsrcode.h"
#include "ui_dialoggetmsrcode.h"

DialogGetMSRCode::DialogGetMSRCode(const QString &pText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGetMSRCode)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Tool | Qt::CustomizeWindowHint | Qt::FramelessWindowHint;
    setWindowFlags(flags);
    if (!pText.isEmpty()) ui->lComment->setText(pText);

    ui->leMSRCode->setFocus();
}

DialogGetMSRCode::~DialogGetMSRCode()
{
    delete ui;
}

QString DialogGetMSRCode::getCode(){
    QString rv = ui->leMSRCode->text().trimmed();
    return rv;
}
