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
#include "onboardingtimezone.h"
#include "ui_onboardingtimezone.h"

#include <statemanager.h>
#include <onboardingmanager.h>
#include "timezonesmodel.h"

struct OnboardingTimeZonePrivate {
    TimezonesModel* model;
};

OnboardingTimeZone::OnboardingTimeZone(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingTimeZone) {
    ui->setupUi(this);
    d = new OnboardingTimeZonePrivate();
    d->model = new TimezonesModel();

    ui->titleLabel->setBackButtonShown(true);
    ui->listView->setModel(d->model);
    ui->listView->setItemDelegate(new TimezonesModelDelegate());
    ui->nextButton->setEnabled(false);
}

OnboardingTimeZone::~OnboardingTimeZone() {
    delete ui;
}

QString OnboardingTimeZone::name() {
    return QStringLiteral("OnboardingTimeZone");
}

QString OnboardingTimeZone::displayName() {
    return tr("Time Zone");
}

void OnboardingTimeZone::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}

void OnboardingTimeZone::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

void OnboardingTimeZone::on_searchBox_textChanged(const QString& arg1) {
    d->model->search(arg1);
}

void OnboardingTimeZone::on_listView_activated(const QModelIndex& index) {
    QDBusMessage setTimezoneMessage = QDBusMessage::createMethodCall("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTimezone");
    setTimezoneMessage.setArguments({index.data(Qt::UserRole).toString(), true});
    QDBusConnection::systemBus().call(setTimezoneMessage, QDBus::NoBlock);

    StateManager::onboardingManager()->setDateVisible(true);
    ui->nextButton->setEnabled(true);
}

void OnboardingTimeZone::on_ntpCheckbox_toggled(bool checked) {
    QDBusMessage setNtpMessage = QDBusMessage::createMethodCall("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetNTP");
    setNtpMessage.setArguments({checked, true});
    QDBusConnection::systemBus().call(setNtpMessage);
}
