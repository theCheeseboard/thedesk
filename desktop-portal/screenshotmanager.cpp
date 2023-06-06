#include "screenshotmanager.h"

#include "dialogs/screenshotwindow.h"
#include <tapplication.h>

struct ScreenshotManagerPrivate {
        QPixmap finalResult;
        bool accepted = false;
        QList<ScreenshotWindow*> screenshotWindows;
};

ScreenshotManager::ScreenshotManager(QObject* parent) :
    QObject{parent} {
    d = new ScreenshotManagerPrivate();

    for (auto screen : qApp->screens()) {
        auto w = new ScreenshotWindow(screen);
        connect(w, &ScreenshotWindow::screenshotAvailable, this, &ScreenshotManager::accept);
        connect(w, &ScreenshotWindow::cancelled, this, &ScreenshotManager::reject);
        d->screenshotWindows.append(w);
    }
}

ScreenshotManager::~ScreenshotManager() {
    for (auto window : d->screenshotWindows) {
        window->deleteLater();
    }
    delete d;
}

void ScreenshotManager::setupForTheDesk() {
    for (auto window : d->screenshotWindows) {
        window->setupForTheDesk();
    }
}

void ScreenshotManager::showScreenshotWindows() {
    for (auto window : d->screenshotWindows) {
        window->showFullScreen();
    }
}

QPixmap ScreenshotManager::finalPixmap() {
    return d->finalResult;
}

bool ScreenshotManager::accepted() {
    return d->accepted;
}

void ScreenshotManager::accept(QPixmap pixmap) {
    d->finalResult = pixmap;
    d->accepted = true;

    auto w = d->screenshotWindows.constFirst();
    connect(w, &ScreenshotWindow::animationComplete, this, &ScreenshotManager::finished);

    for (auto window : d->screenshotWindows) {
        window->animateTake();
    }
}

void ScreenshotManager::reject() {
    d->accepted = false;
    auto w = d->screenshotWindows.constFirst();
    connect(w, &ScreenshotWindow::animationComplete, this, &ScreenshotManager::finished);

    for (auto window : d->screenshotWindows) {
        window->animateDiscard();
    }
}
