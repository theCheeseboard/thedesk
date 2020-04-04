QT       += core gui tdesktopenvironment

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = thedesk
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    background/background.cpp \
    bar/barwindow.cpp \
    bar/chunkcontainer.cpp \
    bar/mainbarwidget.cpp \
    gateway/appselectionmodel.cpp \
    gateway/appselectionmodellistdelegate.cpp \
    gateway/gateway.cpp \
    gateway/maingatewaywidget.cpp \
    main.cpp \
    plugins/pluginmanager.cpp

HEADERS += \
    background/background.h \
    bar/barwindow.h \
    bar/chunkcontainer.h \
    bar/mainbarwidget.h \
    gateway/appselectionmodel.h \
    gateway/appselectionmodellistdelegate.h \
    gateway/gateway.h \
    gateway/maingatewaywidget.h \
    plugins/plugininterface.h \
    plugins/pluginmanager.h

FORMS += \
    background/background.ui \
    bar/barwindow.ui \
    bar/chunkcontainer.ui \
    bar/mainbarwidget.ui \
    gateway/gateway.ui \
    gateway/maingatewaywidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$OUT_PWD/../libthedesk/ -lthedesk

INCLUDEPATH += $$PWD/../libthedesk
DEPENDPATH += $$PWD/../libthedesk

RESOURCES += \
    resources.qrc
