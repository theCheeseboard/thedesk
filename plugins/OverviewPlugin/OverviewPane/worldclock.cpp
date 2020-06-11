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
#include "worldclock.h"
#include "ui_worldclock.h"

#include <QPainter>
#include <QProcess>
#include <TimeDate/desktoptimedate.h>

#include <statemanager.h>
#include <statuscentermanager.h>

struct WorldClockPrivate {
    QTimeZone tz;
    bool mouseDown = false;
};

WorldClock::WorldClock(QTimeZone tz, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::WorldClock) {
    ui->setupUi(this);

    d = new WorldClockPrivate();
    d->tz = tz;

    QString city = QString(tz.id()).replace("_", " ");
    if (city.contains("/")) city = city.split("/").at(1);
    city = city.toUpper();

    ui->tzLabel->setText(city);

    this->setMouseTracking(true);

    updateClock();
}

WorldClock::~WorldClock() {
    delete d;
    delete ui;
}

void WorldClock::updateClock() {
    QString text;

    QDateTime date = QDateTime::currentDateTimeUtc();
    date = date.addSecs(d->tz.offsetFromUtc(date));
    text = DesktopTimeDate::timeString(date, DesktopTimeDate::Time);

    int dayDifference = QDateTime::currentDateTime().daysTo(date);
    if (dayDifference < 0) {
        text.append(QStringLiteral("-%1").arg(dayDifference * -1));
    } else if (dayDifference > 0) {
        text.append(QStringLiteral("+%1").arg(dayDifference));
    }

    ui->clockLabel->setText(text);

}


void WorldClock::mousePressEvent(QMouseEvent* event) {
    d->mouseDown = true;
    this->update();
}

void WorldClock::mouseReleaseEvent(QMouseEvent* event) {
    d->mouseDown = false;
    if (this->underMouse()) {
        StateManager::statusCenterManager()->hide();
        QProcess::startDetached("the24", {});
    }
    this->update();
}

void WorldClock::enterEvent(QEvent* event) {
    this->update();
}

void WorldClock::leaveEvent(QEvent* event) {
    this->update();
}

void WorldClock::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(Qt::transparent);
    if (d->mouseDown) {
        painter.setBrush(QColor(0, 0, 0, 127));
    } else if (this->underMouse()) {
        painter.setBrush(QColor(255, 255, 255, 127));
    }
    painter.drawRect(0, 0, this->width(), this->height());
}
