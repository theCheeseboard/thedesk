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
#include "messagedialoghelper.h"

#include <QWindow>
#include <QPointer>
#include "messagedialog.h"

struct MessageDialogHelperPrivate {
    MessageDialog* dlg;
    QPointer<QWindow> parent;
};

MessageDialogHelper::MessageDialogHelper() : QPlatformMessageDialogHelper() {
    d = new MessageDialogHelperPrivate();
    d->dlg = new MessageDialog();
    connect(d->dlg, &MessageDialog::clicked, this, &MessageDialogHelper::clicked);
}

MessageDialogHelper::~MessageDialogHelper() {
    d->dlg->deleteLater();
    delete d;
}

void MessageDialogHelper::updateWindowGeometry() {
    QRect geometry = d->dlg->geometry();
    geometry.moveCenter(d->parent->geometry().center());
    geometry.moveTop(d->parent->geometry().top());
    d->dlg->setGeometry(geometry);
}

void MessageDialogHelper::exec() {
    d->dlg->setOptions(this->options());
    d->dlg->exec();
}

bool MessageDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow* parent) {
    d->dlg->setOptions(this->options());
    d->dlg->setWindowFlags(windowFlags);
    d->dlg->setWindowModality(windowModality);
    d->dlg->show();

    if (d->parent) {
        d->parent->disconnect(this);
    }
    d->parent = parent;
    connect(parent, &QWindow::widthChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::heightChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::xChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::yChanged, this, &MessageDialogHelper::updateWindowGeometry);
    updateWindowGeometry();

    return true;
}

void MessageDialogHelper::hide() {
    d->dlg->hide();
}

void MessageDialogHelper::setOptions(const QSharedPointer<QMessageDialogOptions>& options) {
    QPlatformMessageDialogHelper::setOptions(options);
    d->dlg->setOptions(options);
}
