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
#include "stickykeyschunk.h"

#include <Wm/desktopwm.h>
#include <Wm/desktopaccessibility.h>
#include <statemanager.h>
#include <barmanager.h>
#include <the-libs_global.h>
#include <QPainter>

struct StickyKeysChunkPrivate {
    Qt::KeyboardModifiers latched;
    Qt::KeyboardModifiers locked;

    bool registered = false;
};

StickyKeysChunk::StickyKeysChunk() : Chunk() {
    d = new StickyKeysChunkPrivate();

    connect(DesktopWm::accessibility(), &DesktopAccessibility::accessibilityOptionEnabledChanged, this, [ = ](DesktopAccessibility::AccessibilityOption option, bool enabled) {
        if (option == DesktopAccessibility::StickyKeys) {
            updateRegistration(enabled);
        }
    });
    connect(DesktopWm::accessibility(), &DesktopAccessibility::stickyKeysStateChanged, this, [ = ](Qt::KeyboardModifiers latched, Qt::KeyboardModifiers locked) {
        d->latched = latched;
        d->locked = locked;

        this->update();
    });

    updateRegistration(DesktopWm::accessibility()->isAccessibilityOptionEnabled(DesktopAccessibility::StickyKeys));
}

StickyKeysChunk::~StickyKeysChunk() {
    updateRegistration(false);
    delete d;
}

QSize StickyKeysChunk::sizeHint() const {
    return SC_DPI_T(QSize(16, 16), QSize);
}

void StickyKeysChunk::updateRegistration(bool reg) {
    if (d->registered == reg) return;
    if (reg) {
        StateManager::barManager()->addChunk(this);
        this->setVisible(true);
    } else {
        StateManager::barManager()->removeChunk(this);
        this->setVisible(false);
    }
    d->registered = reg;
}

QString StickyKeysChunk::name() {
    return "Accessibility-StickyKeys";
}

int StickyKeysChunk::expandedHeight() {
    return SC_DPI(16);
}

int StickyKeysChunk::statusBarHeight() {
    return SC_DPI(16);
}

void StickyKeysChunk::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QRect rect;
    rect.setSize(SC_DPI_T(QSize(16, 16), QSize));
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    QRect shiftRect;
    shiftRect.setHeight(rect.height() / 2 - SC_DPI(2));
    shiftRect.setWidth(rect.width() - SC_DPI(6));
    shiftRect.moveTopLeft(rect.topLeft() + SC_DPI_T(QPoint(2, 2), QPoint));

    QRect ctrlRect;
    ctrlRect.setHeight(shiftRect.height());
    ctrlRect.setWidth(SC_DPI(4));
    ctrlRect.moveTop(shiftRect.bottom() + SC_DPI(1));
    ctrlRect.moveLeft(shiftRect.left());

    QRect superRect = ctrlRect;
    superRect.moveLeft(ctrlRect.right() + SC_DPI(1));

    QRect altRect = superRect;
    altRect.moveLeft(superRect.right() + SC_DPI(1));

    painter.setPen(this->palette().color(QPalette::WindowText));
    painter.setBrush(Qt::transparent);
    painter.drawRect(shiftRect);
    painter.drawRect(ctrlRect);
    painter.drawRect(superRect);
    painter.drawRect(altRect);

    painter.setPen(Qt::transparent);
    painter.setBrush(this->palette().color(QPalette::WindowText));

    if (d->locked & Qt::ShiftModifier || d->latched & Qt::ShiftModifier) {
        painter.drawRect(shiftRect);
    }
    if (d->locked & Qt::ControlModifier || d->latched & Qt::ControlModifier) {
        painter.drawRect(ctrlRect);
    }
    if (d->locked & Qt::MetaModifier || d->latched & Qt::MetaModifier) {
        painter.drawRect(superRect);
    }
    if (d->locked & Qt::AltModifier || d->latched & Qt::AltModifier) {
        painter.drawRect(altRect);
    }
}
