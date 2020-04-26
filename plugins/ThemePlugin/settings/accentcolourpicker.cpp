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
#include "accentcolourpicker.h"

#include <QPainter>
#include <tsettings.h>
#include <QMouseEvent>
#include <QKeyEvent>

struct AccentColourPickerPrivate {
    tSettings* settings;

    QString colorName;
    QColor color;
    bool checked;
};

AccentColourPicker::AccentColourPicker(QWidget* parent) : QWidget(parent) {
    d = new AccentColourPickerPrivate();
    d->settings = new tSettings("theDesk.platform", this);
    connect(d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Palette/accent") {
            d->checked = value.toString() == d->colorName;
            this->update();
        }
    });

    this->setFocusPolicy(Qt::StrongFocus);
}

AccentColourPicker::~AccentColourPicker() {
    delete d;
}

void AccentColourPicker::setColorName(QString name) {
    d->colorName = name;
    d->checked = d->settings->value("Palette/accent").toString() == name;
    updateColor();
}

QSize AccentColourPicker::sizeHint() const {
    return SC_DPI_T(QSize(24, 24), QSize);
}

void AccentColourPicker::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setBrush(d->color);
    painter.setPen(Qt::transparent);
    painter.drawRect(0, 0, this->width(), this->height());

    if (d->checked) {
        QRect pxRect;
        pxRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
        pxRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));
        painter.drawPixmap(pxRect, QIcon::fromTheme("dialog-ok").pixmap(pxRect.size()));
    }
}

void AccentColourPicker::mousePressEvent(QMouseEvent* event) {
    event->accept();
}

void AccentColourPicker::mouseReleaseEvent(QMouseEvent* event) {
    QRect geometry = this->geometry();
    geometry.moveTopLeft(QPoint(0, 0));
    if (geometry.contains(event->pos())) {
        d->settings->setValue("Palette/accent", d->colorName);
        d->settings->sync();
    }
}

void AccentColourPicker::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        d->settings->setValue("Palette/accent", d->colorName);
        d->settings->sync();
    }
}

void AccentColourPicker::updateColor() {
    if (d->colorName == "blue") {
        d->color = QColor(0, 50, 150);
    } else if (d->colorName == "green") {
        d->color = QColor(0, 85, 0);
    } else if (d->colorName == "orange") {
        d->color = QColor(200, 50, 0);
    } else if (d->colorName == "pink") {
        d->color = QColor(150, 0, 150);
    }

    this->update();
}
