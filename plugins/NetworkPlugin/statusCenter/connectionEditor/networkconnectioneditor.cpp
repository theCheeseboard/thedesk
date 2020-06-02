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
#include "networkconnectioneditor.h"
#include "ui_networkconnectioneditor.h"

#include <QDBusPendingCallWatcher>
#include <ttoast.h>
#include <terrorflash.h>

#include <NetworkManagerQt/Settings>
#include "wificonnectioneditorpane.h"
#include "ipv4connectioneditorpane.h"

struct NetworkConnectionEditorPrivate {
    NetworkManager::ConnectionSettings::Ptr connectionSettings;
    NetworkManager::Connection::Ptr connection;
    NMVariantMapMap oldSettings;
    bool haveUnsaved = false;

    QList<ConnectionEditorPane*> panes;
};

NetworkConnectionEditor::NetworkConnectionEditor(NetworkManager::ConnectionSettings::Ptr connectionSettings, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NetworkConnectionEditor) {
    ui->setupUi(this);

    this->init();
    d->connectionSettings = connectionSettings;

    ui->saveWidget->setVisible(true);
    ui->unsavedPane->setVisible(false);

    this->populate();
}

NetworkConnectionEditor::NetworkConnectionEditor(NetworkManager::Connection::Ptr connection, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::NetworkConnectionEditor) {
    ui->setupUi(this);

    this->init();
    d->connectionSettings = connection->settings();
    d->connection = connection;
    d->oldSettings = connection->settings()->toMap();

    ui->saveWidget->setVisible(false);
    ui->unsavedPane->setVisible(true);
    ui->unsavedPane->setFixedHeight(0);

    this->populate();
    d->haveUnsaved = false;
    updateUnsaved();
}

NetworkConnectionEditor::~NetworkConnectionEditor() {
    delete d;
    delete ui;
}

int NetworkConnectionEditor::preferredPopoverWidth() {
    return SC_DPI(-300);
}

void NetworkConnectionEditor::setSaveButtonText(QString text) {
    ui->saveButton->setText(text);
}

void NetworkConnectionEditor::on_titleLabel_backButtonClicked() {
    if (d->connection && d->haveUnsaved) {
        tErrorFlash::flashError(ui->unsavedPane);
        return;
    }
    emit rejected();
}

void NetworkConnectionEditor::on_connectionName_textChanged(const QString& arg1) {
    ui->titleLabel->setText(arg1);
    d->connectionSettings->setId(arg1);
}

void NetworkConnectionEditor::init() {
    d = new NetworkConnectionEditorPrivate();
    ui->leftWidget->setFixedWidth(SC_DPI(300));
    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);
}

void NetworkConnectionEditor::populate() {
    ui->connectionName->setText(d->connectionSettings->id());
    ui->titleLabel->setText(d->connectionSettings->id());

    for (NetworkManager::Setting::Ptr setting : d->connectionSettings->settings()) {
        ConnectionEditorPane* pane = nullptr;

        switch (setting->type()) {
            case NetworkManager::Setting::Wireless:
                pane = new WifiConnectionEditorPane(setting, this);
                break;
            case NetworkManager::Setting::Ipv4:
                pane = new IPv4ConnectionEditorPane(setting, this);
                break;
            case NetworkManager::Setting::Adsl:
            case NetworkManager::Setting::Cdma:
            case NetworkManager::Setting::Gsm:
            case NetworkManager::Setting::Infiniband:
            case NetworkManager::Setting::Ipv6:
            case NetworkManager::Setting::Ppp:
            case NetworkManager::Setting::Pppoe:
            case NetworkManager::Setting::Security8021x:
            case NetworkManager::Setting::Serial:
            case NetworkManager::Setting::Vpn:
            case NetworkManager::Setting::Wired:
            case NetworkManager::Setting::WirelessSecurity:
            case NetworkManager::Setting::Bluetooth:
            case NetworkManager::Setting::OlpcMesh:
            case NetworkManager::Setting::Vlan:
            case NetworkManager::Setting::Wimax:
            case NetworkManager::Setting::Bond:
            case NetworkManager::Setting::Bridge:
            case NetworkManager::Setting::BridgePort:
            case NetworkManager::Setting::Team:
            case NetworkManager::Setting::Generic:
            case NetworkManager::Setting::Tun:
            case NetworkManager::Setting::Vxlan:
            case NetworkManager::Setting::IpTunnel:
            case NetworkManager::Setting::Proxy:
            case NetworkManager::Setting::User:
            case NetworkManager::Setting::OvsBridge:
            case NetworkManager::Setting::OvsInterface:
            case NetworkManager::Setting::OvsPatch:
            case NetworkManager::Setting::OvsPort:
            case NetworkManager::Setting::Match:
            case NetworkManager::Setting::Tc:
            case NetworkManager::Setting::TeamPort:
            case NetworkManager::Setting::Macsec:
            case NetworkManager::Setting::Dcb:
            case NetworkManager::Setting::WireGuard:
                continue;
        }

        if (pane) {
            ui->stackedWidget->addWidget(pane);
            d->panes.append(pane);

            QListWidgetItem* item = new QListWidgetItem();
            item->setText(pane->displayName());
            ui->listWidget->addItem(item);

            connect(pane, &ConnectionEditorPane::setConnectionName, this, [ = ](QString connectionName) {
                d->connectionSettings->setId(connectionName);
                ui->connectionName->setText(connectionName);
                ui->titleLabel->setText(connectionName);
            });
            connect(pane, &ConnectionEditorPane::changed, this, [ = ] {
                d->haveUnsaved = true;
                updateUnsaved();
            });
        }
    }
}

void NetworkConnectionEditor::updateUnsaved() {
    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(ui->unsavedPane->height());
    if (d->haveUnsaved) {
        ui->leftLine->setVisible(true);
        anim->setEndValue(ui->unsavedPane->sizeHint().height());
    } else {
        anim->setEndValue(0);
    }
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        ui->unsavedPane->setFixedHeight(value.toInt());
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        if (anim->endValue() == 0) ui->leftLine->setVisible(false);
        anim->deleteLater();
    });
    anim->start();
}

void NetworkConnectionEditor::on_saveButton_clicked() {
    //Save the new connection settings
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(NetworkManager::addConnection(d->connectionSettings->toMap()));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't save settings"));
            toast->setText(watcher->error().message());
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        } else {
            QDBusObjectPath path = watcher->reply().arguments().first().value<QDBusObjectPath>();
            NetworkManager::Connection::Ptr connection(new NetworkManager::Connection(path.path()));
            emit accepted(connection);
        }
        watcher->deleteLater();
    });
}

void NetworkConnectionEditor::on_saveModifiedButton_clicked() {
    //Notify all panes about intent to save
    for (ConnectionEditorPane* pane : d->panes) {
        if (!pane->prepareSave()) {
            //Can't save this one; go there to let the user know
            ui->listWidget->setCurrentRow(ui->stackedWidget->indexOf(pane));
            return;
        }
    }

    //Save the changes
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(d->connection->update(d->connectionSettings->toMap()));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
        if (watcher->isError()) {
            tToast* toast = new tToast();
            toast->setTitle(tr("Couldn't save settings"));
            toast->setText(watcher->error().message());
            connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
            toast->show(this);
        } else {
            d->connectionSettings = d->connection->settings();
            for (ConnectionEditorPane* pane : d->panes) {
                pane->reload(d->connectionSettings->setting(pane->type()));
            }
            d->haveUnsaved = false;
            updateUnsaved();
        }
        watcher->deleteLater();
    });
}

void NetworkConnectionEditor::on_revertButton_clicked() {
    //Undo the changes
    d->connectionSettings->fromMap(d->oldSettings);
    for (ConnectionEditorPane* pane : d->panes) {
        pane->reload(d->connectionSettings->setting(pane->type()));
    }
    d->haveUnsaved = false;
    updateUnsaved();
}
