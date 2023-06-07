#ifndef NOTIFICATIONINTERFACE_H
#define NOTIFICATIONINTERFACE_H

#include <QDBusAbstractAdaptor>

struct NotificationInterfacePrivate;
class NotificationInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Notification")
    public:
        explicit NotificationInterface(QObject* parent = nullptr);
        ~NotificationInterface();

    public slots:
        Q_SCRIPTABLE void AddNotification(QString app_id, QString id, QVariantMap notification);
        Q_SCRIPTABLE void RemoveNotification(QString app_id, QString id);

    signals:
        Q_SCRIPTABLE void ActionInvoked(QString app_id, QString id, QString action, QVariantList parameter);

    private:
        NotificationInterfacePrivate* d;
};

#endif // NOTIFICATIONINTERFACE_H
