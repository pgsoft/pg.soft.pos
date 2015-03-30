#edit block
TARGET = sale
TEMPLATE = lib
QT = core widgets sql
CONFIG += qt warn_on release plugin
PLUGIN_SUBDIR = /Plugins

#auto project vars
QT_WORK = D:/Work/Qt
PROJECT_DOMAIN=PG.Soft
PROJECT_NAME=POS
PROJECT_DIR = $${QT_WORK}/$${PROJECT_DOMAIN}/$${PROJECT_NAME}
PROJECT_TEMP = $${QT_WORK}/Temp/$${PROJECT_DOMAIN}/$${PROJECT_NAME}/$${TARGET}
PROJECT_DEV = $${PROJECT_DIR}/dev
TARGET_DIR = $${PROJECT_DEV}/$${TARGET}

#auto qmake vars
DESTDIR = $${PROJECT_DIR}/bin$${PLUGIN_SUBDIR}
OBJECTS_DIR = build
MOC_DIR = $${OBJECTS_DIR}
UI_DIR = $${OBJECTS_DIR}
INCLUDEPATH += ./src \
    $${PROJECT_DEV}/Commone

#edit block
TRANSLATIONS    = ts/$${TARGET}_en.ts \
                  ts/$${TARGET}_ru.ts \
                  ts/$${TARGET}_he.ts
CODECFORSRC     = UTF-8
HEADERS = src/plu.h \
    ../commone/commonefunctions.h \  
    ../commone/UserModuleWidget.h
SOURCES += src/plu.cpp \ 
    ../commone/commonefunctions.cpp \   
    ../commone/UserModuleWidget.cpp
RESOURCES += \  
    rc/plu.qrc \
    ../commone/rc/UserModuleWidget.qrc
RC_FILE += 

FORMS += \  
    ../commone/ui/UserModuleWidget.ui
