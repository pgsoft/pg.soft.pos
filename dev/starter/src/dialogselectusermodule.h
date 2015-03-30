#ifndef DIALOGSELECTUSERMODULE_H
#define DIALOGSELECTUSERMODULE_H

#include <QDialog>

namespace Ui {
class DialogSelectUserModule;
}

class DialogSelectUserModule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectUserModule(QWidget *parent = 0);
    ~DialogSelectUserModule();
//
    void addUserModuleWidget(QWidget *pWidget);
    QObject *getSelectedUserModuleObject();
private:
    Ui::DialogSelectUserModule *ui;
    QObject *selectedUserModuleObject;
private slots:
    void userModuleWidget_clicked();

};

#endif // DIALOGSELECTUSERMODULE_H
