#include "lockmanager.h"

#include "mainwindow.h"
#include <QTimer>
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>
#include <Wm/desktopwm.h>
#include <lockergrabs.h>

struct LockManagerPrivate {
        QList<MainWindow*> openWindows;
        QTimer* grabTimer;

        bool debugMode = false;
};

LockManager* LockManager::instance() {
    static auto manager = new LockManager();
    return manager;
}

void LockManager::setDebug(bool debugMode) {
    d->debugMode = debugMode;
    if (debugMode) {
        LockerGrabs::releaseGrab();
    }

    this->openLockWindows();
}

void LockManager::openLockWindows() {
    for (auto window : d->openWindows) {
        window->close();
        window->deleteLater();
    }
    d->openWindows.clear();

    if (d->debugMode) {
        auto* w = new MainWindow();
        w->show();
        d->openWindows.append(w);
    } else {
        for (auto screen : ScreenDaemon::instance()->screens()) {
            auto* w = new MainWindow();
            w->setWindowFlags(Qt::WindowStaysOnTopHint);
            w->show();
            w->setGeometry(screen->geometry());
            w->showFullScreen();
            d->openWindows.append(w);
        }
    }
}

void LockManager::establishGrab() {
    if (d->debugMode) return;

    LockerGrabs::establishGrab();
}

void LockManager::raiseAll() {
    for (auto w : d->openWindows) {
        w->raise();
    }
}

LockManager::LockManager(QObject* parent) :
    QObject{parent} {
    d = new LockManagerPrivate();

    connect(ScreenDaemon::instance(), &ScreenDaemon::screensUpdated, this, &LockManager::openLockWindows);
    openLockWindows();

    d->grabTimer = new QTimer();
    d->grabTimer->setInterval(5000);
    connect(d->grabTimer, &QTimer::timeout, this, &LockManager::establishGrab);
    connect(d->grabTimer, &QTimer::timeout, this, &LockManager::raiseAll);
    d->grabTimer->start();

    establishGrab();
}
