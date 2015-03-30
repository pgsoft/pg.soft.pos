#ifndef __PLUUSERMODULEINTERFACE_H__
#define __PLUUSERMODULEINTERFACE_H__

#include <QtCore>
#include <QtGui>

class PluUserModuleInterface
{
public:
    virtual ~PluUserModuleInterface() {}
public:
    virtual QWidget *getWidget() = 0; //получить графическое представление плагина
    virtual int getSortPosition() = 0; //получить позицию для сортировки в списке плагинов
    virtual void setSortPosition(int) = 0; //получить позицию для сортировки в списке плагинов
    virtual int getState() = 0;
    //выполнить целевое действие плагина
    virtual bool show(const QVariant &parameter=QVariant()) = 0;
};

Q_DECLARE_INTERFACE(PluUserModuleInterface, "PG.Soft.POS.Plugins.PluUserModuleInterface/1.0")

#endif // __PLUUSERMODULEINTERFACE_H__
