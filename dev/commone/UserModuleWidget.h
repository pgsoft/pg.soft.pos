#ifndef USERMODULEWIDGET_H
#define USERMODULEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
namespace Ui {
class UserModuleWidget;
}

class UserModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserModuleWidget(const QString &pName
                              , const QString &pDescription
                              , QPixmap *pPixmap
                              , QObject *pPluginObject
                              , QWidget *parent = 0);
    ~UserModuleWidget();

    //
protected:
    void paintEvent(QPaintEvent *);
private:
    Ui::UserModuleWidget *ui;
public slots:
    void on_pbDescription_clicked();
signals:
    void clicked();
};

#endif // USERMODULEWIDGET_H
