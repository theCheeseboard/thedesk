#ifndef SCREENCASTINTERFACE_H
#define SCREENCASTINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QObject>

struct ScreencastInterfacePrivate;
class ScreencastInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.ScreenCast")
        Q_PROPERTY(uint version READ version)
        Q_PROPERTY(uint AvailableSourceTypes READ availableSourceTypes)
        Q_PROPERTY(uint AvailableCursorModes READ availableCursorModes)

    public:
        explicit ScreencastInterface(QObject* parent = nullptr);
        ~ScreencastInterface();

        uint version();
        uint availableSourceTypes();
        uint availableCursorModes();

    public slots:
        Q_SCRIPTABLE uint CreateSession(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QVariantMap options, const QDBusMessage& message, QVariantMap& results);
        Q_SCRIPTABLE uint SelectSources(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QVariantMap options, const QDBusMessage& message, QVariantMap& results);
        Q_SCRIPTABLE uint Start(QDBusObjectPath handle, QDBusObjectPath sessionHandle, QString appId, QString parentWindow, QVariantMap options, const QDBusMessage& message, QVariantMap& results);

    private:
        ScreencastInterfacePrivate* d;
};

#endif // SCREENCASTINTERFACE_H
