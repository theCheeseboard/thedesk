#include "accessinterface.h"

#include "dialogs/accessdialog.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <Screens/systempopover.h>

AccessInterface::AccessInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
}

uint AccessInterface::AccessDialog(QDBusObjectPath handle, QString appId, QString parentWindow, QString title, QString subtitle, QString body, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    auto dialog = new class AccessDialog();
    dialog->setTitle(title);
    dialog->setSubtitle(subtitle);
    dialog->setBody(body);
    dialog->setDenyButtonText(options.value("deny_label", tr("Deny")).toString());
    dialog->setAllowButtonText(options.value("grant_label", tr("Allow")).toString());

    auto replyTemplate = message.createReply();

    auto* popover = new SystemPopover(dialog);
    popover->setPopoverWidth(600);
    popover->setPopoverSide(tPopover::Bottom);
    connect(dialog, &AccessDialog::accept, this, [popover, replyTemplate] {
        auto reply = QDBusMessage(replyTemplate);
        reply.setArguments({uint(0), QVariantMap()});
        QDBusConnection::sessionBus().send(replyTemplate);
        popover->dismiss();
    });
    connect(dialog, &AccessDialog::reject, this, [popover, replyTemplate] {
        auto reply = QDBusMessage(replyTemplate);
        reply.setArguments({uint(1), QVariantMap()});
        QDBusConnection::sessionBus().send(replyTemplate);
        popover->dismiss();
    });
    connect(popover, &tPopover::dismissed, this, [popover] {
        popover->deleteLater();
    });
    popover->show();

    message.setDelayedReply(true);
    return 1;
}
