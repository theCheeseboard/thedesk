#ifndef X11SCREENCOPYSESSION_H
#define X11SCREENCOPYSESSION_H

#include "abstractscreencopysession.h"

class X11ScreenCopySession : public AbstractScreenCopySession {
        Q_OBJECT
    public:
        explicit X11ScreenCopySession(ScreenCopySource source, QObject* parent = nullptr);

    signals:

        // AbstractScreenCopySession interface
    public:
        QImage currentImage();
};

#endif // X11SCREENCOPYSESSION_H
