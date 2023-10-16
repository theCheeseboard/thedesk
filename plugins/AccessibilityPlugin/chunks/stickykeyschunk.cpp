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

#include <QPainter>
#include <Wm/desktopaccessibility.h>
#include <Wm/desktopwm.h>
#include <actionquickwidget.h>
#include <barmanager.h>
#include <libcontemporary_global.h>
#include <statemanager.h>
#include <tsettings.h>

struct StickyKeysChunkPrivate {
        Qt::KeyboardModifiers latched;
        Qt::KeyboardModifiers locked;

        ActionQuickWidget* quickWidget;
        tSettings settings;

        bool registered = false;
};

StickyKeysChunk::StickyKeysChunk() :
    Chunk() {
    d = new StickyKeysChunkPrivate();

    connect(DesktopWm::accessibility(), &DesktopAccessibility::accessibilityOptionEnabledChanged, this, [this](DesktopAccessibility::AccessibilityOption option, bool enabled) {
        if (option == DesktopAccessibility::StickyKeys) {
            updateRegistration(enabled);
        }
    });
    connect(DesktopWm::accessibility(), &DesktopAccessibility::stickyKeysStateChanged, this, [this](Qt::KeyboardModifiers latched, Qt::KeyboardModifiers locked) {
        d->latched = latched;
        d->locked = locked;

        this->update();
    });

    d->quickWidget = new ActionQuickWidget(this);
    d->quickWidget->addAction(tr("Disable Sticky Keys"), [=] {
        d->settings.setValue("Accessibility/stickykeys.active", false);
    });

    updateRegistration(DesktopWm::accessibility()->isAccessibilityOptionEnabled(DesktopAccessibility::StickyKeys));
}

StickyKeysChunk::~StickyKeysChunk() {
    updateRegistration(false);
    delete d;
}

QSize StickyKeysChunk::sizeHint() const {
    return QSize(16, 16);
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
    return 16;
}

int StickyKeysChunk::statusBarHeight() {
    return 16;
}

void StickyKeysChunk::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QRect rect;
    rect.setSize(QSize(16, 16));
    rect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    QRect shiftRect;
    shiftRect.setHeight(rect.height() / 2 - 2);
    shiftRect.setWidth(rect.width() - 6);
    shiftRect.moveTopLeft(rect.topLeft() + QPoint(2, 2));

    QRect ctrlRect;
    ctrlRect.setHeight(shiftRect.height());
    ctrlRect.setWidth(4);
    ctrlRect.moveTop(shiftRect.bottom() + 1);
    ctrlRect.moveLeft(shiftRect.left());

    QRect superRect = ctrlRect;
    superRect.moveLeft(ctrlRect.right() + 1);

    QRect altRect = superRect;
    altRect.moveLeft(superRect.right() + 1);

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

QWidget* StickyKeysChunk::quickWidget() {
    return d->quickWidget;
}
