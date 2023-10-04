#include "waylandportalbackend.h"

#include "private/waylandportalscreencopyframe.h"
#include <QApplication>
#include <QCoroCore>
#include <QPixmap>
#include <qpa/qplatformnativeinterface.h>
#include <tlogger.h>
#include <wayland-client-protocol.h>

struct WaylandPortalBackendPrivate {
        WaylandPortalBackend* parent;
        wl_display* display;
        wl_seat* seat;
        QList<wl_output*> outputs;
        wl_shm* shm;
};

WaylandPortalBackend::WaylandPortalBackend(QObject* parent) :
    AbstractPortalBackend{parent} {
    d = new WaylandPortalBackendPrivate();
    d->parent = this;
    d->display = reinterpret_cast<wl_display*>(qApp->platformNativeInterface()->nativeResourceForIntegration("display"));

    wl_registry_listener listener = {
        [](void* data, wl_registry* registry, quint32 name, const char* interface, quint32 version) {
        auto* backend = static_cast<WaylandPortalBackendPrivate*>(data);
        if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
            backend->parent->QtWayland::zwlr_screencopy_manager_v1::init(registry, name, 1);
        } else if (strcmp(interface, wl_output_interface.name) == 0) {
            auto output = static_cast<wl_output*>(wl_registry_bind(registry, name, &wl_output_interface, std::min(version, static_cast<quint32>(1))));
            backend->outputs.append(output);
        } else if (strcmp(interface, wl_seat_interface.name) == 0) {
            wl_seat* seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, std::min(version, static_cast<quint32>(1))));
            backend->seat = seat;
        } else if (strcmp(interface, wl_shm_interface.name) == 0) {
            backend->shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, std::min(version, static_cast<quint32>(1))));
        }
        },
        [](void* data, wl_registry* registry, quint32 name) {
        Q_UNUSED(data)
        Q_UNUSED(registry)
        Q_UNUSED(name)
    }};

    wl_registry* registry = wl_display_get_registry(d->display);
    wl_registry_add_listener(registry, &listener, d);
    wl_display_roundtrip(d->display);

    if (!this->QtWayland::zwlr_screencopy_manager_v1::isInitialized()) {
        tWarn("WaylandPortalBackend") << "The compositor doesn't support the wlr-screencopy-manager-v1 protocol";
    }
    wl_registry_destroy(registry);
}

WaylandPortalBackend::~WaylandPortalBackend() {
    delete d;
}

QCoro::Task<QPixmap> WaylandPortalBackend::takeScreenshot(QScreen* screen) {
    WaylandPortalScreencopyFrame capture(this->capture_output(false, d->outputs.first()), d->shm, d->display);
    co_await qCoro(&capture, &WaylandPortalScreencopyFrame::done);

    if (capture.ok()) {
        co_return capture.pixmap();
    } else {
        QPixmap px(screen->size());
        px.fill(Qt::red);
        co_return px;
    }
}
