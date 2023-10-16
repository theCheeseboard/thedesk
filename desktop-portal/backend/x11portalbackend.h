#ifndef X11PORTALBACKEND_H
#define X11PORTALBACKEND_H

#include "abstractportalbackend.h"
#include <QObject>

class X11PortalBackend : public AbstractPortalBackend {
        Q_OBJECT
    public:
        explicit X11PortalBackend(QObject* parent = nullptr);

        // AbstractPortalBackend interface
    public:
        QCoro::Task<QPixmap> takeScreenshot(QScreen* screen);
};

#endif // X11PORTALBACKEND_H
