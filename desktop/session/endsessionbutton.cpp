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
#include "endsessionbutton.h"

#include <QPainter>

struct EndSessionButtonPrivate {
    double timeRemaining = 0;
};

EndSessionButton::EndSessionButton(QWidget* parent) : QCommandLinkButton(parent) {
    d = new EndSessionButtonPrivate();
}

EndSessionButton::~EndSessionButton() {
    delete d;
}

void EndSessionButton::setTimeRemaining(double timeRemaining) {
    d->timeRemaining = timeRemaining;
    this->update();
}

void EndSessionButton::paintEvent(QPaintEvent* event) {
    QCommandLinkButton::paintEvent(event);

    QPainter painter(this);
    painter.setBrush(QColor(255, 255, 255, 50));
    painter.setPen(Qt::transparent);
    painter.drawRect(0, 0, static_cast<int>(this->width() * d->timeRemaining / 60.0), this->height());
}
