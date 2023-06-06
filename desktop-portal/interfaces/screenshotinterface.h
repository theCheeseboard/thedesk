#ifndef SCREENSHOTINTERFACE_H
#define SCREENSHOTINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class QDBusMessage;
class ScreenshotInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Screenshot")
        Q_PROPERTY(uint version READ version)

    public:
        explicit ScreenshotInterface(QObject* parent = nullptr);

        uint version();

    public slots:
        Q_SCRIPTABLE uint Screenshot(QDBusObjectPath handle, QString app_id, QString parent_window, QVariantMap options, const QDBusMessage& message, QVariantMap& results);
        Q_SCRIPTABLE uint PickColor(QDBusObjectPath handle, QString app_id, QString parent_window, QVariantMap options, const QDBusMessage& message, QVariantMap& results);
};

struct ScreenshotColorReply {
        double red;
        double green;
        double blue;
};

Q_DECLARE_METATYPE(ScreenshotColorReply);

#endif // SCREENSHOTINTERFACE_H
