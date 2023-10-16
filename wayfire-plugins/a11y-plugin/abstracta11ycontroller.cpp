#include "abstracta11ycontroller.h"

struct AbstractA11yControllerPrivate {
        bool enabled = false;
};

AbstractA11yController::AbstractA11yController(QObject* parent) :
    QObject{parent} {
    d = new AbstractA11yControllerPrivate();
}

AbstractA11yController::~AbstractA11yController() {
    delete d;
}

void AbstractA11yController::setEnabled(bool enabled) {
    if (enabled && !d->enabled) {
        d->enabled = true;
        this->enable();
    } else if (!enabled && d->enabled) {
        d->enabled = false;
        this->disable();
    }
}

bool AbstractA11yController::enabled() {
    return d->enabled;
}
