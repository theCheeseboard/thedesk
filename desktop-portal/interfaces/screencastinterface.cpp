#include "screencastinterface.h"

#include "portalcommon.h"
#include "private/screencast/screencastsession.h"
#include <QCoroCore>
#include <QDBusConnection>
#include <QUuid>
#include <dialogs/screencastdialog.h>
#include <tlogger.h>

struct ScreencastInterfacePrivate {
        QMap<QString, ScreencastSessionPtr> sessions;
};

ScreencastInterface::ScreencastInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
    d = new ScreencastInterfacePrivate();
}

ScreencastInterface::~ScreencastInterface() {
    delete d;
}

uint ScreencastInterface::version() {
    return 5;
}

uint ScreencastInterface::availableSourceTypes() {
    return 1; // Monitor
}

uint ScreencastInterface::availableCursorModes() {
    return 1; // Hidden
}

uint ScreencastInterface::CreateSession(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    auto id = QUuid::createUuid().toString();
    auto session = new ScreencastSession(sessionHandle);
    d->sessions.insert(sessionHandle.path(), ScreencastSessionPtr(session));

    connect(session, &ScreencastSession::done, this, [id, this, sessionHandle] {
        d->sessions.remove(sessionHandle.path());
    });

    results.insert("session_id", id);
    return 0;
}

uint ScreencastInterface::SelectSources(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    auto session = d->sessions.value(sessionHandle.path());
    if (!session) {
        // TODO: Throw DBus error?
        tWarn("ScreencastInterface") << "SelectSources called with invalid session handle";
        return 0;
    }

    session->configureOptions(options);
    return 0;
}

uint ScreencastInterface::Start(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QString parentWindow, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    auto session = d->sessions.value(sessionHandle.path());
    if (!session) {
        // TODO: Throw DBus error?
        tWarn("ScreencastInterface") << "Start called with invalid session handle";
        return 0;
    }

    PortalCommon::setupCoro([options, parentWindow, handle, this, appId](QDBusMessage reply) -> QCoro::Task<> {
        QVariantMap results;
        results.insert("persist_mode", uint(0));

        ScreencastDialog dialog(appId);
        PortalCommon::reparentWindow(&dialog, parentWindow);
        dialog.open();

        auto result = co_await qCoro(&dialog, &ScreencastDialog::finished);
        if (result != ScreencastDialog::Accepted) {
            reply.setArguments({uint(1), {}});
            QDBusConnection::sessionBus().send(reply);
        }

        reply.setArguments({uint(0), results});
        QDBusConnection::sessionBus().send(reply);
        co_return;
    },
        message);
    return 0;
}
