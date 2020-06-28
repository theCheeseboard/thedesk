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
#include "notification.h"

#include <Applications/application.h>

struct NotificationPrivate {
    quint32 id;
    qint32 timeout;

    QString summary;
    QString body;
    Notification::Urgency urgency = Notification::Normal;

    QList<Notification::Action> actions;

    ApplicationPointer application;
    bool isActive = true;
};

Notification::Notification(quint32 id) {
    d = new NotificationPrivate();
    d->id = id;
}

Notification::~Notification() {
    delete d;
}

quint32 Notification::id() {
    return d->id;
}

void Notification::setSummary(QString summary) {
    d->summary = summary;
    emit summaryChanged(summary);
}

QString Notification::summary() {
    return d->summary;
}

void Notification::setBody(QString body) {
    d->body = body;
    emit bodyChanged(body);
}

QString Notification::body() {
    return d->body;
}

void Notification::setTimeout(qint32 timeout) {
    if (timeout == -1) timeout = 5000;

    d->timeout = timeout;
    emit timeoutChanged(timeout);
}

qint32 Notification::timeout() {
    return d->timeout;
}

void Notification::setApplication(ApplicationPointer application) {
    d->application = application;
    emit applicationChanged(application);
}

ApplicationPointer Notification::application() {
    return d->application;
}

void Notification::setActions(QList<Notification::Action> actions) {
    d->actions = actions;
    emit actionsChanged(actions);
}

QList<Notification::Action> Notification::actions() {
    return d->actions;
}

void Notification::setUrgency(Notification::Urgency urgency) {
    d->urgency = urgency;
}

Notification::Urgency Notification::urgency() {
    return d->urgency;
}

void Notification::dismiss(Notification::NotificationCloseReason reason) {
    if (!d->isActive) return;
    d->isActive = false;
    emit dismissed(reason);
}
