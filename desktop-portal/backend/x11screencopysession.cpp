#include "x11screencopysession.h"

#include <QImage>

X11ScreenCopySession::X11ScreenCopySession(ScreenCopySource source, QObject* parent) :
    AbstractScreenCopySession{parent} {
}

QImage X11ScreenCopySession::currentImage() {
    return QImage();
}
