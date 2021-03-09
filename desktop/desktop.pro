QT       += core gui tdesktopenvironment network multimedia multimediawidgets quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = thedesk
CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)

QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$PWD/translations) $$shell_quote($$OUT_PWD) && \
    $$QMAKE_COPY $$quote($$PWD/defaults.conf) $$shell_quote($$OUT_PWD)

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
    bar/currentappwidget.cpp \
    bar/currentappwidgetmenu.cpp \
    bar/mainbarwidget.cpp \
    bar/taskbarwidget.cpp \
    cli/commandline.cpp \
    gateway/appsearchprovider.cpp \
    gateway/appselectionmodel.cpp \
    gateway/appselectionmodellistdelegate.cpp \
    gateway/gateway.cpp \
    gateway/gatewaysearchmodel.cpp \
    gateway/gatewaysearchmodeldelegate.cpp \
    gateway/maingatewaywidget.cpp \
    gateway/searchresultswidget.cpp \
    main.cpp \
    run/rundialog.cpp \
    session/endsession.cpp \
    session/endsessionbutton.cpp \
    statuscenter/leftpanedelegate.cpp \
    statuscenter/statuscenter.cpp \
    statuscenter/statuscenterleftpane.cpp \
    statuscenter/statuscenterquickswitch.cpp \
    systemsettings/about/about.cpp \
    systemsettings/about/acknowledgements.cpp \
    systemsettings/about/changehostnamepopover.cpp \
    systemsettings/pluginmanagement/managepluginpopover.cpp \
    systemsettings/pluginmanagement/pluginitemdelegate.cpp \
    systemsettings/pluginmanagement/pluginmanagement.cpp \
    systemsettings/pluginmanagement/pluginmodel.cpp \
    systemsettings/recovery/recovery.cpp \
    systemsettings/systemsettings.cpp \
    systemsettings/systemsettingsleftpane.cpp

HEADERS += \
    background/background.h \
    bar/barwindow.h \
    bar/chunkcontainer.h \
    bar/currentappwidget.h \
    bar/currentappwidgetmenu.h \
    bar/mainbarwidget.h \
    bar/taskbarwidget.h \
    cli/commandline.h \
    gateway/appsearchprovider.h \
    gateway/appselectionmodel.h \
    gateway/appselectionmodellistdelegate.h \
    gateway/gateway.h \
    gateway/gatewaysearchmodel.h \
    gateway/gatewaysearchmodeldelegate.h \
    gateway/maingatewaywidget.h \
    gateway/searchresultswidget.h \
    run/rundialog.h \
    session/endsession.h \
    session/endsessionbutton.h \
    statuscenter/leftpanedelegate.h \
    statuscenter/statuscenter.h \
    statuscenter/statuscenterleftpane.h \
    statuscenter/statuscenterquickswitch.h \
    systemsettings/about/about.h \
    systemsettings/about/acknowledgements.h \
    systemsettings/about/changehostnamepopover.h \
    systemsettings/pluginmanagement/managepluginpopover.h \
    systemsettings/pluginmanagement/pluginitemdelegate.h \
    systemsettings/pluginmanagement/pluginmanagement.h \
    systemsettings/pluginmanagement/pluginmodel.h \
    systemsettings/recovery/recovery.h \
    systemsettings/systemsettings.h \
    systemsettings/systemsettingsleftpane.h

FORMS += \
    background/background.ui \
    bar/barwindow.ui \
    bar/chunkcontainer.ui \
    bar/currentappwidget.ui \
    bar/currentappwidgetmenu.ui \
    bar/mainbarwidget.ui \
    bar/taskbarwidget.ui \
    gateway/gateway.ui \
    gateway/maingatewaywidget.ui \
    gateway/searchresultswidget.ui \
    run/rundialog.ui \
    session/endsession.ui \
    statuscenter/statuscenter.ui \
    statuscenter/statuscenterleftpane.ui \
    statuscenter/statuscenterquickswitch.ui \
    systemsettings/about/about.ui \
    systemsettings/about/acknowledgements.ui \
    systemsettings/about/changehostnamepopover.ui \
    systemsettings/pluginmanagement/managepluginpopover.ui \
    systemsettings/pluginmanagement/pluginmanagement.ui \
    systemsettings/recovery/recovery.ui \
    systemsettings/systemsettings.ui \
    systemsettings/systemsettingsleftpane.ui

unix {
    target.path = /usr/bin/

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/translations

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/

    media.files = media/
    media.path = /usr/share/thedesk/

    acknowledgements.files = acknowledgements.html
    acknowledgements.path = /usr/share/thedesk/acknowledgements.html

    INSTALLS += target translations defaults media acknowledgements
}

DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$[QT_INSTALL_LIBS]\\\"

unix:!macx: LIBS += -L$$OUT_PWD/../libthedesk/ -lthedesk

INCLUDEPATH += $$PWD/../libthedesk
DEPENDPATH += $$PWD/../libthedesk

RESOURCES += \
    resources.qrc

DISTFILES += \
    acknowledgements.html \
    defaults.conf \
    onboarding/OnboardingVideoForm.qml
