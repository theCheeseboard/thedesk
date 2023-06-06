#include "portalcommon.h"

#include <QWindow>

void PortalCommon::reparentWindow(QWidget* w, QString parentWindow) {
    if (parentWindow.startsWith("x11:")) {
        w->setAttribute(Qt::WA_NativeWindow, true);
        w->setWindowModality(Qt::WindowModal);
        auto parent = QWindow::fromWinId(parentWindow.mid(4).toULongLong(nullptr, 16));
        w->windowHandle()->setTransientParent(parent);

        QObject::connect(w, &QWidget::destroyed, parent, &QWindow::deleteLater);
    }
}

void PortalCommon::setupCoro(std::function<QCoro::Task<>(QDBusMessage)> coro, const QDBusMessage& message) {
    message.setDelayedReply(true);
    coro(message.createReply());
}
