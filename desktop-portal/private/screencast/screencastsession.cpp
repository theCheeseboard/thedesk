#include "screencastsession.h"

struct ScreencastSessionPrivate {
};

ScreencastSession::ScreencastSession(QDBusObjectPath sessionHandle, QObject* parent) :
    QObject{parent} {
    d = new ScreencastSessionPrivate();
}

ScreencastSession::~ScreencastSession() {
    delete d;
}

void ScreencastSession::configureOptions(QVariantMap options) {
}
