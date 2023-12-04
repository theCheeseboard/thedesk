#include "x11portalbackend.h"

#include <QPixmap>

X11PortalBackend::X11PortalBackend(QObject* parent) :
    AbstractPortalBackend{parent} {
}

QCoro::Task<QPixmap> X11PortalBackend::takeScreenshot(QScreen* screen) {
    co_return screen->grabWindow(0);
}

QList<ScreenCopySource> X11PortalBackend::screenCopySources() {
    return {};
}
