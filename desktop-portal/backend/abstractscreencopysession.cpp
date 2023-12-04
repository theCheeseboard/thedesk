#include "abstractscreencopysession.h"

#include "waylandscreencopysession.h"
#include "x11screencopysession.h"
#include <tx11info.h>

AbstractScreenCopySession::AbstractScreenCopySession(QObject* parent) :
    QObject(parent) {
}

AbstractScreenCopySession* AbstractScreenCopySession::sessionFor(ScreenCopySource source) {
    if (tX11Info::isPlatformX11()) {
        return new X11ScreenCopySession(source);
    } else {
        return new WaylandScreenCopySession(source);
    }
}
