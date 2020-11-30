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
#include "tetheringsettingspopover.h"
#include "ui_tetheringsettingspopover.h"

#include <terrorflash.h>
#include <QHostInfo>
#include <tsettings.h>

struct TetheringSettingsPopoverPrivate {
    tSettings settings;
};

TetheringSettingsPopover::TetheringSettingsPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TetheringSettingsPopover) {
    ui->setupUi(this);
    d = new TetheringSettingsPopoverPrivate();

    ui->titleLabel->setBackButtonShown(true);

    QString ssid = d->settings.value("NetworkPlugin/tethering.ssid").toString();
    ssid = ssid.replace("$hostname", QHostInfo::localHostName());
    ui->ssidBox->setText(ssid);
    ui->keyBox->setText(d->settings.value("NetworkPlugin/tethering.key").toString());
}

TetheringSettingsPopover::~TetheringSettingsPopover() {
    delete ui;
    delete d;
}

void TetheringSettingsPopover::on_ssidBox_textChanged(const QString& arg1) {
    QString ssid = arg1;
    if (arg1.isEmpty()) {
        ui->ssidBox->setText(QHostInfo::localHostName());
        ssid = QHostInfo::localHostName();
    }

    d->settings.setValue("NetworkPlugin/tethering.ssid", ssid);
}

void TetheringSettingsPopover::on_keyBox_textChanged(const QString& arg1) {
    if (arg1.length() < 8) {
        tErrorFlash::flashError(ui->keyBox);
        ui->keyError->setVisible(true);
        ui->keyError->setText(tr("Network key needs to be at least 8 characters long"));
        return;
    }

    ui->keyError->setVisible(false);

    d->settings.setValue("NetworkPlugin/tethering.key", arg1);
}

void TetheringSettingsPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
