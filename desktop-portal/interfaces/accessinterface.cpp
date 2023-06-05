#include "accessinterface.h"

#include <QDBusMessage>

AccessInterface::AccessInterface(QObject* parent) :
    QDBusAbstractAdaptor{parent} {
}

uint AccessInterface::AccessDialog(QDBusObjectPath handle, QString appId, QString parentWindow, QString title, QString subtitle, QString body, QVariantMap options, const QDBusMessage& message, QVariantMap& results) {
    return 0;
}
