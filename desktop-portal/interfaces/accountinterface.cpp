#include "accountinterface.h"

#include "dialogs/accountdialog.h"
#include "portalcommon.h"
#include <QCoroCore>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>

AccountInterface::AccountInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
}

uint AccountInterface::version() {
    return 1;
}

uint AccountInterface::GetUserInformation(QDBusObjectPath handle, QString appId, QString window, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    auto coro = [message, options, appId, window]() -> QCoro::Task<> {
        auto reply = message.createReply();

        QString reason;
        if (options.contains("reason")) reason = options.value("reason").toString();
        auto dialog = new AccountDialog(appId, reason);
        PortalCommon::reparentWindow(dialog, window);
        dialog->show();

        auto result = co_await qCoro(dialog, &AccountDialog::finished);

        if (result == AccountDialog::Rejected) {
            reply.setArguments({uint(1), QVariantMap()});
            QDBusConnection::sessionBus().send(reply);
        } else {
            reply.setArguments({uint(0), dialog->results()});
            QDBusConnection::sessionBus().send(reply);
        }
    };

    coro();

    message.setDelayedReply(true);
    return 0;
}
