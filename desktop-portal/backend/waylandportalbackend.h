#ifndef WAYLANDPORTALBACKEND_H
#define WAYLANDPORTALBACKEND_H

#include "abstractportalbackend.h"
#include <QObject>
#include <qwayland-wlr-screencopy-unstable-v1.h>

struct WaylandPortalBackendPrivate;
class WaylandPortalBackend : public AbstractPortalBackend,
                             public QtWayland::zwlr_screencopy_manager_v1 {
        Q_OBJECT
    public:
        explicit WaylandPortalBackend(QObject* parent = nullptr);
        ~WaylandPortalBackend();

    private:
        WaylandPortalBackendPrivate* d;

        // AbstractPortalBackend interface
    public:
        QCoro::Task<QPixmap> takeScreenshot(QScreen* screen);
};

#endif // WAYLANDPORTALBACKEND_H
