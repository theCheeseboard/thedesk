QT += widgets thelib tdesktopenvironment dbus multimedia multimediawidgets quickwidgets

TEMPLATE = lib
DEFINES += LIBTHEDESK_LIBRARY
TARGET = thedesk

CONFIG += c++11

# Include the-libs build tools
include(/usr/share/the-libs/pri/gentranslations.pri)
include(/usr/share/the-libs/pri/varset.pri)

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
    actionquickwidget.cpp \
    barmanager.cpp \
    chunk.cpp \
    common.cpp \
    gatewaymanager.cpp \
    gatewaysearchprovider.cpp \
    hudmanager.cpp \
    icontextchunk.cpp \
    keygrab.cpp \
    localemanager.cpp \
    onboarding/onboarding.cpp \
    onboarding/onboardingbar.cpp \
    onboarding/onboardingbetathankyou.cpp \
    onboarding/onboardingcontroller.cpp \
    onboarding/onboardingfinal.cpp \
    onboarding/onboardingstepper.cpp \
    onboarding/onboardingvideo.cpp \
    onboarding/onboardingwelcome.cpp \
    onboardingmanager.cpp \
    onboardingpage.cpp \
    plugins/pluginmanager.cpp \
    powermanager.cpp \
    private/localeselector.cpp \
    private/quickwidgetcontainer.cpp \
    quickswitch.cpp \
    quietmodemanager.cpp \
    server/sessionserver.cpp \
    statemanager.cpp \
    statuscentermanager.cpp \
    statuscenterpane.cpp \
    transparentdialog.cpp

HEADERS += \
    actionquickwidget.h \
    barmanager.h \
    chunk.h \
    common.h \
    gatewaymanager.h \
    gatewaysearchprovider.h \
    hudmanager.h \
    icontextchunk.h \
    keygrab.h \
    libthedesk_global.h \
    localemanager.h \
    onboarding/onboarding.h \
    onboarding/onboardingbar.h \
    onboarding/onboardingbetathankyou.h \
    onboarding/onboardingcontroller.h \
    onboarding/onboardingfinal.h \
    onboarding/onboardingstepper.h \
    onboarding/onboardingvideo.h \
    onboarding/onboardingwelcome.h \
    onboardingmanager.h \
    onboardingpage.h \
    plugins/pluginmanager.h \
    plugins/plugininterface.h \
    powermanager.h \
    private/localeselector.h \
    private/onboardingmanager_p.h \
    private/quickwidgetcontainer.h \
    quickswitch.h \
    quietmodemanager.h \
    server/sessionserver.h \
    statemanager.h \
    statuscentermanager.h \
    statuscenterpane.h \
    transparentdialog.h

# Default rules for deployment.
unix {
    target.path = $$THELIBS_INSTALL_LIB

    headers.files = *.h
    headers.path = $$[QT_INSTALL_HEADERS]/libthedesk/

    pluginheaders.files = plugins/plugininterface.h
    pluginheaders.path = $$[QT_INSTALL_HEADERS]/thedesk/

    pluginmanagerheaders.files = plugins/pluginmanager.h
    pluginmanagerheaders.path = $$[QT_INSTALL_HEADERS]/libthedesk/plugins

    onboardingheaders.files = onboarding/onboardingcontroller.h
    onboardingheaders.path = $$[QT_INSTALL_HEADERS]/libthedesk/onboarding

    translations.files = translations/*.qm
    translations.path = /usr/share/thedesk/libthedesk/translations

    INSTALLS += target translations headers pluginheaders onboardingheaders pluginmanagerheaders
}

DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$THELIBS_INSTALL_LIB\\\"

FORMS += \
    actionquickwidget.ui \
    icontextchunk.ui \
    onboarding/onboarding.ui \
    onboarding/onboardingbar.ui \
    onboarding/onboardingbetathankyou.ui \
    onboarding/onboardingfinal.ui \
    onboarding/onboardingvideo.ui \
    onboarding/onboardingwelcome.ui \
    private/localeselector.ui \
    private/quickwidgetcontainer.ui \
    transparentdialog.ui

DISTFILES += \
    onboarding/OnboardingVideoForm.qml

RESOURCES += \
    libthedesk_resources.qrc
