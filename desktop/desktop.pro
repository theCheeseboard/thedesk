QT       += core gui tdesktopenvironment network multimedia multimediawidgets

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
    bar/mainbarwidget.cpp \
    bar/taskbarwidget.cpp \
    cli/commandline.cpp \
    common/common.cpp \
    gateway/appselectionmodel.cpp \
    gateway/appselectionmodellistdelegate.cpp \
    gateway/gateway.cpp \
    gateway/maingatewaywidget.cpp \
    main.cpp \
    onboarding/onboarding.cpp \
    onboarding/onboardingbar.cpp \
    onboarding/onboardingbetathankyou.cpp \
    onboarding/onboardingcontroller.cpp \
    onboarding/onboardingfinal.cpp \
    onboarding/onboardingstepper.cpp \
    onboarding/onboardingvideo.cpp \
    onboarding/onboardingwelcome.cpp \
    plugins/pluginmanager.cpp \
    server/sessionserver.cpp \
    session/endsession.cpp \
    session/endsessionbutton.cpp \
    statuscenter/statuscenter.cpp \
    statuscenter/statuscenterleftpane.cpp \
    statuscenter/statuscenterquickswitch.cpp \
    systemsettings/about/about.cpp \
    systemsettings/pluginmanagement/managepluginpopover.cpp \
    systemsettings/pluginmanagement/pluginitemdelegate.cpp \
    systemsettings/pluginmanagement/pluginmanagement.cpp \
    systemsettings/pluginmanagement/pluginmodel.cpp \
    systemsettings/recovery/recovery.cpp \
    systemsettings/systemsettings.cpp \
    systemsettings/systemsettingsleftpane.cpp \
    transparentdialog.cpp

HEADERS += \
    background/background.h \
    bar/barwindow.h \
    bar/chunkcontainer.h \
    bar/mainbarwidget.h \
    bar/taskbarwidget.h \
    cli/commandline.h \
    common/common.h \
    gateway/appselectionmodel.h \
    gateway/appselectionmodellistdelegate.h \
    gateway/gateway.h \
    gateway/maingatewaywidget.h \
    onboarding/onboarding.h \
    onboarding/onboardingbar.h \
    onboarding/onboardingbetathankyou.h \
    onboarding/onboardingcontroller.h \
    onboarding/onboardingfinal.h \
    onboarding/onboardingstepper.h \
    onboarding/onboardingvideo.h \
    onboarding/onboardingwelcome.h \
    plugins/plugininterface.h \
    plugins/pluginmanager.h \
    server/sessionserver.h \
    session/endsession.h \
    session/endsessionbutton.h \
    statuscenter/statuscenter.h \
    statuscenter/statuscenterleftpane.h \
    statuscenter/statuscenterquickswitch.h \
    systemsettings/about/about.h \
    systemsettings/pluginmanagement/managepluginpopover.h \
    systemsettings/pluginmanagement/pluginitemdelegate.h \
    systemsettings/pluginmanagement/pluginmanagement.h \
    systemsettings/pluginmanagement/pluginmodel.h \
    systemsettings/recovery/recovery.h \
    systemsettings/systemsettings.h \
    systemsettings/systemsettingsleftpane.h \
    transparentdialog.h

FORMS += \
    background/background.ui \
    bar/barwindow.ui \
    bar/chunkcontainer.ui \
    bar/mainbarwidget.ui \
    bar/taskbarwidget.ui \
    gateway/gateway.ui \
    gateway/maingatewaywidget.ui \
    onboarding/onboarding.ui \
    onboarding/onboardingbar.ui \
    onboarding/onboardingbetathankyou.ui \
    onboarding/onboardingfinal.ui \
    onboarding/onboardingvideo.ui \
    onboarding/onboardingwelcome.ui \
    session/endsession.ui \
    statuscenter/statuscenter.ui \
    statuscenter/statuscenterleftpane.ui \
    statuscenter/statuscenterquickswitch.ui \
    systemsettings/about/about.ui \
    systemsettings/pluginmanagement/managepluginpopover.ui \
    systemsettings/pluginmanagement/pluginmanagement.ui \
    systemsettings/recovery/recovery.ui \
    systemsettings/systemsettings.ui \
    systemsettings/systemsettingsleftpane.ui \
    transparentdialog.ui

unix {
    target.path = /usr/bin/

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/translations

    headers.files = plugins/plugininterface.h
    headers.path = /usr/include/thedesk/

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theDesk/

    media.files = media/
    media.path = /usr/share/thedesk/

    INSTALLS += target translations defaults headers media
}

unix:!macx: LIBS += -L$$OUT_PWD/../libthedesk/ -lthedesk

INCLUDEPATH += $$PWD/../libthedesk
DEPENDPATH += $$PWD/../libthedesk

RESOURCES += \
    resources.qrc

DISTFILES += \
    defaults.conf
