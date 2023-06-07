#include "notificationinterface.h"

#include <Applications/application.h>
#include <tnotification.h>

typedef QPair<QString, QString> NotificationIdentifier;

struct NotificationInterfacePrivate {
        QMap<QPair<QString, QString>, tNotification*> notifications;
};

NotificationInterface::NotificationInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
    d = new NotificationInterfacePrivate();
}

NotificationInterface::~NotificationInterface() {
    delete d;
}

void NotificationInterface::AddNotification(QString app_id, QString id, QVariantMap notification) {
    auto title = notification.value("title").toString();
    auto body = notification.value("body").toString();
    auto priority = notification.value("priority").toString();
    auto defaultAction = notification.value("default-action").toString();
    auto defaultActionTarget = notification.value("default-action-target");
    auto buttons = qdbus_cast<QList<QVariantMap>>(notification.value("buttons").value<QDBusArgument>());

    Application app(app_id);

    NotificationIdentifier nid{app_id, id};
    tNotification* n;
    if (d->notifications.contains(nid)) {
        n = d->notifications.value(nid);
    } else {
        n = new tNotification();
        connect(n, &tNotification::actionClicked, this, [app_id, id, this, buttons, defaultActionTarget, defaultAction](QString key) {
            if (key == defaultAction) {
                emit ActionInvoked(app_id, id, key, {defaultActionTarget});
            }

            for (const auto &button : buttons) {
                auto action = button.value("action").toString();
                if (action == key) {
                    auto target = button.value("target");
                    emit ActionInvoked(app_id, id, key, {target});
                }
            }
        });
    }

    n->insertHint("desktop-entry", app_id);
    n->setAppName(app.getProperty("Name").toString());
    n->setAppIcon(app.getProperty("Icon").toString());
    n->setSummary(title);
    n->setText(body);

    if (priority == "low") n->setUrgency(tNotification::Low);
    if (priority == "normal") n->setUrgency(tNotification::Normal);
    if (priority == "high") n->setUrgency(tNotification::Critical);
    if (priority == "urgent") n->setUrgency(tNotification::Critical);

    n->insertAction(defaultAction, tr("Activate"));
    for (const auto &button : buttons) {
        n->insertAction(button.value("action").toString(), button.value("label").toString());
    }

    n->post(false);
    d->notifications.insert(nid, n);
}

void NotificationInterface::RemoveNotification(QString app_id, QString id) {
    NotificationIdentifier nid{app_id, id};
    if (d->notifications.contains(nid)) {
        auto notification = d->notifications.take(nid);
        notification->dismiss();
        notification->deleteLater();
    }
}
