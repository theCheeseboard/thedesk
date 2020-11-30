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
#include "datetimepane.h"
#include "ui_datetimepane.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <QIcon>
#include <QDBusInterface>
#include <tpopover.h>
#include "popovers/settimezonepopover.h"

struct DateTimePanePrivate {
    bool updating = false;
};

DateTimePane::DateTimePane() :
    StatusCenterPane(),
    ui(new Ui::DateTimePane) {
    ui->setupUi(this);

    d = new DateTimePanePrivate();

    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->actionsWidget->setFixedWidth(contentWidth);
    ui->timeSyncWidget->setFixedWidth(contentWidth);

    QDBusConnection::systemBus().connect("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
    forceUpdate();
}

DateTimePane::~DateTimePane() {
    delete d;
    delete ui;
}

void DateTimePane::forceUpdate() {
    QDBusInterface timedateInterface("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", QDBusConnection::systemBus());
    QVariantMap changedProperties;
    changedProperties.insert("NTP", timedateInterface.property("NTP"));
    changedProperties.insert("Timezone", timedateInterface.property("Timezone"));
    this->propertiesChanged("org.freedesktop.timedate1", changedProperties, changedProperties.keys());
}

QString DateTimePane::name() {
    return QStringLiteral("DateTimeSettings");
}

QString DateTimePane::displayName() {
    return tr("Date and Time");
}

QIcon DateTimePane::icon() {
    return QIcon::fromTheme("preferences-system-time");
}

QWidget* DateTimePane::leftPane() {
    return nullptr;
}

void DateTimePane::on_titleLabel_backButtonClicked() {
    StateManager::instance()->statusCenterManager()->showStatusCenterHamburgerMenu();
}

void DateTimePane::on_ntpSwitch_toggled(bool checked) {
    if (d->updating) return;
    QDBusMessage setNtpMessage = QDBusMessage::createMethodCall("org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetNTP");
    setNtpMessage.setArguments({checked, true});
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(setNtpMessage));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        forceUpdate();
        watcher->deleteLater();
    });
}

void DateTimePane::propertiesChanged(QString interfaceName, QVariantMap changedProperties, QStringList invalidatedProperties) {
    Q_UNUSED(interfaceName)

    d->updating = true;
    if (invalidatedProperties.contains("NTP")) ui->ntpSwitch->setChecked(changedProperties.value("NTP").toBool());
    d->updating = false;
}

void DateTimePane::on_changeTimezoneButton_clicked() {
    SetTimezonePopover* setTimezone = new SetTimezonePopover();
    tPopover* popover = new tPopover(setTimezone);
    popover->setPopoverWidth(SC_DPI(600));
    connect(setTimezone, &SetTimezonePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, setTimezone, &SetTimezonePopover::deleteLater);
    popover->show(this->window());
}
