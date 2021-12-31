#include "sleepmonitor.h"

#include <tsettings.h>
#include <QDBusConnection>

#include <statemanager.h>
#include <powermanager.h>

struct SleepMonitorPrivate {
    tSettings settings;
};

SleepMonitor::SleepMonitor(QObject* parent) : QObject{parent} {
    d = new SleepMonitorPrivate();

    QDBusConnection::systemBus().connect("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PrepareForSleep", this, SLOT(PrepareForSleep(bool)));
}

SleepMonitor::~SleepMonitor() {
    delete d;
}

void SleepMonitor::PrepareForSleep(bool sleeping) {
    if (sleeping) {
        if (d->settings.value("Power/suspend.lockScreen").toBool()) {
            //Lock the screen now
            StateManager::powerManager()->performPowerOperation(PowerManager::Lock);
        }
    }
}
