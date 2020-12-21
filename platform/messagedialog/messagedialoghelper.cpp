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
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QMainWindow>
#include <tscrim.h>
#include <tcsdtools.h>
#include <tvariantanimation.h>
#include "messagedialog.h"

struct MessageDialogHelperPrivate {
    MessageDialog* dlg;
    QEventLoop eventLoop;
    QPointer<QWindow> parent;
    QPointer<QWidget> parentWidget;
    QPointer<QWidget> cover2;

    const int coverOverscan = SC_DPI(50);
};

MessageDialogHelper::MessageDialogHelper() : QPlatformMessageDialogHelper() {
    d = new MessageDialogHelperPrivate();
    d->dlg = new MessageDialog();
    connect(d->dlg, &MessageDialog::clicked, this, [ = ] {
        if (d->eventLoop.isRunning()) d->eventLoop.exit();
    });
    connect(d->dlg, &MessageDialog::clicked, this, &MessageDialogHelper::clicked);
}

MessageDialogHelper::~MessageDialogHelper() {
    d->dlg->deleteLater();
    if (d->cover2) d->cover2->deleteLater();
    delete d;
}

void MessageDialogHelper::updateWindowGeometry() {
    if (d->parent) {
        QRect geometry;
        if (d->parentWidget) {
            geometry = QRect(-d->coverOverscan, -d->coverOverscan, d->parentWidget->width() + 2 * d->coverOverscan, d->parentWidget->height() + 2 * d->coverOverscan);
        } else {
            geometry = QRect(-d->coverOverscan, -d->coverOverscan, d->parent->width() + 2 * d->coverOverscan, d->parent->height() + 2 * d->coverOverscan);
        }
        if (d->cover2) d->cover2->setGeometry(geometry);
        d->dlg->updateGeometry();
    } else {
        d->dlg->setFixedSize(d->dlg->frameGeometry().size());
    }
}

void MessageDialogHelper::exec() {
    d->dlg->setOptions(this->options());
    d->eventLoop.exec();
}

bool MessageDialogHelper::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow* parent) {
    QWidget* widget = nullptr;

    if (parent) {
        for (QWidget* w : QApplication::allWidgets()) {
            if (w->winId() == parent->winId()) widget = w->window();
        }
    }
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget);
    if (mainWindow && tCsdTools::csdsInstalled(mainWindow)) widget = mainWindow->centralWidget();

    d->parentWidget = widget;

    d->dlg->setOptions(this->options());
    d->dlg->setWindowFlags(windowFlags);

    if (d->parent) {
        d->parent->disconnect(this);
    }
    d->parent = parent;
    connect(parent, &QWindow::widthChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::heightChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::xChanged, this, &MessageDialogHelper::updateWindowGeometry);
    connect(parent, &QWindow::yChanged, this, &MessageDialogHelper::updateWindowGeometry);

    if (widget) {
        tScrim::scrimForWidget(widget)->show();

        d->dlg->setParent(widget);
        d->dlg->show();
        d->dlg->raise();

        //For some reason, Qt doesn't render this properly on tCsd apps
        //but it fixes itself if we have a transparent widget on top
        d->cover2 = new QWidget();
        d->cover2->setAttribute(Qt::WA_TransparentForMouseEvents);
        d->cover2->setParent(widget);
        d->cover2->show();
        d->cover2->raise();

        d->dlg->animateIn();
    }

    updateWindowGeometry();

    d->dlg->show();

    return true;
}

void MessageDialogHelper::hide() {
    if (d->parentWidget) {
        d->dlg->animateOut();
        tScrim::scrimForWidget(d->parentWidget)->hide();
        if (d->cover2) d->cover2->hide();
    } else {
        d->dlg->hide();
        if (d->cover2) d->cover2->hide();
    }
}

void MessageDialogHelper::setOptions(const QSharedPointer<QMessageDialogOptions>& options) {
    QPlatformMessageDialogHelper::setOptions(options);
    d->dlg->setOptions(options);
}
