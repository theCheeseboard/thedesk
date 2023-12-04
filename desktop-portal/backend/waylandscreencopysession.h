#ifndef WAYLANDSCREENCOPYSESSION_H
#define WAYLANDSCREENCOPYSESSION_H

#include "abstractscreencopysession.h"

struct WaylandScreenCopySessionPrivate;
class WaylandScreenCopySession : public AbstractScreenCopySession {
        Q_OBJECT
    public:
        explicit WaylandScreenCopySession(ScreenCopySource source, QObject* parent = nullptr);
        ~WaylandScreenCopySession();

    signals:

    private:
        WaylandScreenCopySessionPrivate* d;

        // AbstractScreenCopySession interface
    public:
        QImage currentImage();
};

#endif // WAYLANDSCREENCOPYSESSION_H
