QT += tdesktopenvironment
CONFIG += plugin

unix:!macx: {
    LIBS += -L$$OUT_PWD/../../libthedesk/ -lthedesk
    INCLUDEPATH += $$PWD/../libthedesk ../../desktop/plugins
    DEPENDPATH += $$PWD/../libthedesk

    target.path = /usr/lib/thedesk/plugins
    INSTALLS += target
}

QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$_PRO_FILE_PWD_/translations) $$shell_quote($$OUT_PWD) && \
    $$QMAKE_COPY $$quote($$_PRO_FILE_PWD_/defaults.conf) $$shell_quote($$OUT_PWD)
