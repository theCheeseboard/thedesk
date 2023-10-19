#ifndef WALLPAPERINTERFACE_H
#define WALLPAPERINTERFACE_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

struct WallpaperInterfacePrivate;
class WallpaperInterface : public QDBusAbstractAdaptor {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.impl.portal.Wallpaper");

    public:
        explicit WallpaperInterface(QObject* parent = nullptr);
        ~WallpaperInterface();

    public slots:
        Q_SCRIPTABLE uint SetWallpaperURI(QDBusObjectPath handle, QString appId, QString parentWindow, QString uri, QVariantMap options);

    private:
        WallpaperInterfacePrivate* d;
};

#endif // WALLPAPERINTERFACE_H
