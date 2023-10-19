#include "wallpaperinterface.h"

#include <Background/backgroundcontroller.h>

struct WallpaperInterfacePrivate {
        BackgroundController controller{BackgroundController::Desktop};
};

WallpaperInterface::WallpaperInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
    d = new WallpaperInterfacePrivate();
}

WallpaperInterface::~WallpaperInterface() {
    delete d;
}

uint WallpaperInterface::SetWallpaperURI(QDBusObjectPath handle, QString appId, QString parentWindow, QString uri, QVariantMap options) {
    auto setOn = options.value("set-on").toString();
    auto showPreview = options.value("show-preview").toBool();
    if (setOn.isEmpty()) {
        return 1;
    }

    QUrl url(uri);
    if (!url.isLocalFile()) {
        return 1;
    }

    // TODO: Show a preview if requested

    // TODO: Copy the image to a persistent location
    if (setOn == QStringLiteral("background") || setOn == QStringLiteral("both")) {
        d->controller.setBackground(url.toLocalFile(), BackgroundController::Desktop);
    }
    if (setOn == QStringLiteral("lockscreen") || setOn == QStringLiteral("both")) {
        d->controller.setBackground(url.toLocalFile(), BackgroundController::LockScreen);
    }

    return 0;
}
