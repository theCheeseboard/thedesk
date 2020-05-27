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
#include "securityeappeap.h"
#include "ui_securityeappeap.h"

#include <terrorflash.h>
#include <QFile>
#include <QFileDialog>

struct SecurityEapPeapPrivate {
    QByteArray caCert;
};

SecurityEapPeap::SecurityEapPeap(QWidget* parent) :
    SecurityEap(parent),
    ui(new Ui::SecurityEapPeap) {
    ui->setupUi(this);

    d = new SecurityEapPeapPrivate();
    ui->titleLabel->setBackButtonShown(true);
}

SecurityEapPeap::~SecurityEapPeap() {
    delete ui;
    delete d;
}

void SecurityEapPeap::on_titleLabel_backButtonClicked() {
    emit back();
}

void SecurityEapPeap::on_connectButton_clicked() {
    if (ui->username->text().isEmpty()) {
        tErrorFlash::flashError(ui->username);
        return;
    }

    if (ui->password->text().isEmpty()) {
        tErrorFlash::flashError(ui->password);
        return;
    }

    if (!ui->caCertificate->text().isEmpty()) {
        QFile file(ui->caCertificate->text());
        if (!file.open(QFile::ReadOnly)) {
            tErrorFlash::flashError(ui->caCertificate);
            return;
        }

        d->caCert = file.readAll();
        file.close();
    }

    emit done();
}


void SecurityEapPeap::populateSetting(NetworkManager::Security8021xSetting::Ptr setting) {
    setting->setEapMethods({NetworkManager::Security8021xSetting::EapMethodPeap});

    if (ui->peapVersionAuto->isChecked()) {
        setting->setPhase1PeapVersion(NetworkManager::Security8021xSetting::PeapVersionUnknown);
    } else if (ui->peapVersion0->isChecked()) {
        setting->setPhase1PeapVersion(NetworkManager::Security8021xSetting::PeapVersionZero);
    } else {
        setting->setPhase1PeapVersion(NetworkManager::Security8021xSetting::PeapVersionOne);
    }

    switch (ui->phase2Auth->currentIndex()) {
        case 0: //MSCHAPv2
            setting->setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
            break;
        case 1: //MD5
            setting->setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMd5);
            break;
        case 2: //GTC
            setting->setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
            break;
    }

    setting->setCaCertificate(d->caCert);
    setting->setAnonymousIdentity(ui->anonymousIdentity->text());
    setting->setIdentity(ui->username->text());
    setting->setPassword(ui->password->text());
}

void SecurityEapPeap::on_browseCaCertButton_clicked() {
    QFileDialog* d = new QFileDialog(this);
    d->setAcceptMode(QFileDialog::AcceptOpen);
    d->setNameFilters({tr("Certificates (*.pem, *.cer)")});
    connect(d, &QFileDialog::finished, this, [ = ](int result) {
        if (result == QFileDialog::Accepted) {
            ui->caCertificate->setText(d->selectedFiles().first());
        }
        d->deleteLater();
    });
    d->open();
}
