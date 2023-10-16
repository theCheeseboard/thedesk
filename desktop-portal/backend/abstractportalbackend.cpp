#include "abstractportalbackend.h"

#include "waylandportalbackend.h"
#include "x11portalbackend.h"
#include <tx11info.h>

AbstractPortalBackend::AbstractPortalBackend(QObject* parent) :
    QObject{parent} {
}

AbstractPortalBackend* AbstractPortalBackend::instance() {
    static AbstractPortalBackend* instance = nullptr;
    if (!instance) {
        if (tX11Info::isPlatformX11()) {
            instance = new X11PortalBackend();
        } else {
            instance = new WaylandPortalBackend();
        }
    }
    return instance;
}
