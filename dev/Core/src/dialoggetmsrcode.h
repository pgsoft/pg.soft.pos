#ifndef DIALOGGETMSRCODE_H
#define DIALOGGETMSRCODE_H

#include <QDialog>

namespace Ui {
class DialogGetMSRCode;
}

class DialogGetMSRCode : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogGetMSRCode(const QString &pText, QWidget *parent = 0);
    ~DialogGetMSRCode();
    QString getCode();
private:
    Ui::DialogGetMSRCode *ui;

};

#endif // DIALOGGETMSRCODE_H
