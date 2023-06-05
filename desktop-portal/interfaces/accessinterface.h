#ifndef ACCESSINTERFACE_H
#define ACCESSINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class QDBusMessage;
class AccessInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Access")

    public:
        explicit AccessInterface(QObject* parent = nullptr);

    public slots:
        Q_SCRIPTABLE uint AccessDialog(QDBusObjectPath handle, QString appId, QString parentWindow, QString title, QString subtitle, QString body, QVariantMap options, const QDBusMessage& message, QVariantMap& results);

    signals:
};

#endif // ACCESSINTERFACE_H
