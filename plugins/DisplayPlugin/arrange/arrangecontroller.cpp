/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "arrangecontroller.h"

#include "overlaywindow.h"
#include "arrangewidget.h"
#include <tpopover.h>
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>

struct ArrangeControllerPrivate {
    QList<ArrangeWidget*> arrangers;
};

ArrangeController::ArrangeController(QObject* parent) : QObject(parent) {
    d = new ArrangeControllerPrivate();
}

ArrangeController::~ArrangeController() {
    delete d;
}

void ArrangeController::begin() {
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        ArrangeWidget* w = new ArrangeWidget(screen);
        connect(w, &ArrangeWidget::reject, this, [ = ] {
            for (ArrangeWidget* w : d->arrangers) {
                w->dismiss();
            }
        });
        connect(w, &ArrangeWidget::destroyed, this, [ = ] {
            d->arrangers.removeOne(w);
            if (d->arrangers.isEmpty()) {
                emit done();
            }
        });
        d->arrangers.append(w);
    }
}
