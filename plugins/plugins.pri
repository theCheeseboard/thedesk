QT += tdesktopenvironment
CONFIG += plugin

unix:!macx: {
    LIBS += -L$$OUT_PWD/../../libthedesk/ -lthedesk
    INCLUDEPATH += $$PWD/../libthedesk ../../desktop/plugins
    DEPENDPATH += $$PWD/../libthedesk
}

