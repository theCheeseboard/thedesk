#ifndef ACCOUNTINTERFACE_H
#define ACCOUNTINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class QDBusMessage;
class AccountInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Account")
        Q_PROPERTY(uint version READ version)

    public:
        explicit AccountInterface(QObject* parent = nullptr);

        uint version();

    public slots:
        Q_SCRIPTABLE uint GetUserInformation(QDBusObjectPath handle, QString appId, QString window, QVariantMap options, const QDBusMessage& message, QVariantMap& results);
};

#endif // ACCOUNTINTERFACE_H
