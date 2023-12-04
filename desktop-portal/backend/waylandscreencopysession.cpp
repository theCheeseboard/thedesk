#include "waylandscreencopysession.h"

#include "private/waylandportalscreencopyframe.h"
#include "waylandportalbackend.h"
#include <QImage>

struct WaylandScreenCopySessionPrivate {
        QImage image;
        WaylandPortalScreencopyFrame* frame;
};

WaylandScreenCopySession::WaylandScreenCopySession(ScreenCopySource source, QObject* parent) :
    AbstractScreenCopySession{parent} {
    d = new WaylandScreenCopySessionPrivate();

    auto backend = static_cast<WaylandPortalBackend*>(AbstractPortalBackend::instance());

    d->frame = backend->captureOutput(reinterpret_cast<wl_output*>(source.data));
    connect(d->frame, &WaylandPortalScreencopyFrame::done, this, [this] {
        d->image = d->frame->image();
        emit imageReady();
    });
}

WaylandScreenCopySession::~WaylandScreenCopySession() {
    d->frame->deleteLater();
    delete d;
}

QImage WaylandScreenCopySession::currentImage() {
    return d->image;
}
