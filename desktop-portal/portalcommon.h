#ifndef PORTALCOMMON_H
#define PORTALCOMMON_H

#include <QCoroTask>
#include <QDBusMessage>
#include <QWidget>

namespace PortalCommon {
    void reparentWindow(QWidget* w, QString parentWindow);
    void setupCoro(std::function<QCoro::Task<>(QDBusMessage)> coro, const QDBusMessage& message);
}; // namespace PortalCommon

#endif // PORTALCOMMON_H
