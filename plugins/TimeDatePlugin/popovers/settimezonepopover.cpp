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
#include "settimezonepopover.h"
#include "ui_settimezonepopover.h"

#include "timezonesmodel.h"

struct SetTimezonePopoverPrivate {
    TimezonesModel* model;
};

SetTimezonePopover::SetTimezonePopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SetTimezonePopover) {
    ui->setupUi(this);
    d = new SetTimezonePopoverPrivate();
    d->model = new TimezonesModel();

    ui->titleLabel->setBackButtonShown(true);
    ui->listView->setModel(d->model);
    ui->listView->setItemDelegate(new TimezonesModelDelegate());
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->spinner->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));
}

SetTimezonePopover::~SetTimezonePopover() {
    delete ui;
    delete d;
}

void SetTimezonePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void SetTimezonePopover::on_listView_activated(const QModelIndex& index) {
    ui->stackedWidget->setCurrentWidget(ui->spinnerPage);

    QDBusMessage setTimezoneMessage = QDBusMessage::createMethodCall("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTimezone");
    setTimezoneMessage.setArguments({index.data(Qt::UserRole).toString(), true});
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(setTimezoneMessage));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            ui->stackedWidget->setCurrentWidget(ui->timezonesPage);
        } else {
            emit done();
        }
        watcher->deleteLater();
    });
}

void SetTimezonePopover::on_searchBox_textChanged(const QString& arg1) {
    d->model->search(arg1);
}
