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
        this->enable();
        d->enabled = true;
    } else if (!enabled && d->enabled) {
        this->disable();
        d->enabled = false;
    }
}

bool AbstractA11yController::enabled() {
    return d->enabled;
}
