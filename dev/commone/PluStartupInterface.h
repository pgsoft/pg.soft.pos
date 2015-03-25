#ifndef __PLUSTARTUPINTERFACE_H__
#define __PLUSTARTUPINTERFACE_H__

#include <QtCore>
#include <QtGui>

class PluStartupInterface
{
public:
    virtual ~PluStartupInterface() {}
protected:
    int pluState;
public:
    virtual int getSortPosition() = 0; //получить позицию для сортировки в списке плагинов
    virtual int getState() = 0;
    virtual void slotStart() = 0;
    virtual void slotStop() = 0;
};

Q_DECLARE_INTERFACE(PluStartupInterface, "AZ_Soft.Plugins.PluStartupInterface/3.0")

#endif // __PLUSTARTUPINTERFACE_H__
