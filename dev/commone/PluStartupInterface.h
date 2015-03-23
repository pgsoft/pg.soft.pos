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
    virtual int getPluState() = 0;
    virtual void slotPluStart() = 0;
    virtual void slotPluStop() = 0;
};

Q_DECLARE_INTERFACE(PluStartupInterface, "AZ_Soft.Plugins.PluStartupInterface/3.0")

#endif // __PLUSTARTUPINTERFACE_H__
