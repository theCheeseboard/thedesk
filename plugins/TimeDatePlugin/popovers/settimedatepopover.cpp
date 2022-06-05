/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#include "settimedatepopover.h"
#include "ui_settimedatepopover.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <tdatetimepicker.h>

struct SetTimeDatePopoverPrivate {
        tDateTimePicker* datePicker;
        tDateTimePicker* timePicker;
};

SetTimeDatePopover::SetTimeDatePopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SetTimeDatePopover) {
    ui->setupUi(this);
    d = new SetTimeDatePopoverPrivate();
    ui->titleLabel->setBackButtonShown(true);

    d->datePicker = new tDateTimePicker(this);
    d->timePicker = new tDateTimePicker(this);
    ui->pickerLayout->addWidget(d->datePicker);
    ui->pickerLayout->addWidget(d->timePicker);

    QFont fnt = d->datePicker->font();
    fnt.setPointSize(30);
    d->datePicker->setFont(fnt);
    d->timePicker->setFont(fnt);

    d->datePicker->setDateTime(QDateTime::currentDateTime());
    d->datePicker->setPickOptions(tDateTimePicker::PickDate);
    d->timePicker->setDateTime(QDateTime::currentDateTime());
    d->timePicker->setPickOptions(tDateTimePicker::PickTime);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->spinner->setFixedSize(SC_DPI_T(QSize(32, 32), QSize));
}

SetTimeDatePopover::~SetTimeDatePopover() {
    delete ui;
    delete d;
}

void SetTimeDatePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void SetTimeDatePopover::on_applyButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->spinnerPage);

    QDateTime selectedDateTime(d->datePicker->currentDateTime().date(), d->timePicker->currentDateTime().time());
    qint64 milliSinceEpoch = selectedDateTime.toMSecsSinceEpoch() * 1000;

    QDBusMessage setTimeMessage = QDBusMessage::createMethodCall("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTime");
    setTimeMessage.setArguments({milliSinceEpoch, false, true});
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(setTimeMessage));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (watcher->isError()) {
            ui->stackedWidget->setCurrentWidget(ui->dateTimePage);
        } else {
            emit done();
        }
        watcher->deleteLater();
    });
}
