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
#include "endsession.h"
#include "ui_endsession.h"

#include <Wm/desktopwm.h>
#include <QPainter>
#include <tvariantanimation.h>
#include "transparentdialog.h"
#include <QTimer>
#include <tpopover.h>
#include <statemanager.h>
#include <powermanager.h>
#include <onboardingmanager.h>
#include <QDBusMessage>
#include <QMenu>

struct EndSessionPrivate {
    tVariantAnimation* powerOffAnimation;
    EndSessionButton* timedButton;
};

EndSession::EndSession(PowerManager::PowerOperation operation, QString message, QStringList flags, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::EndSession) {
    ui->setupUi(this);

    d = new EndSessionPrivate();

    if (message == "") {
        message = tr("Hey %1, ready to head out? We'll %2 in %n seconds if you don't do anything.");
    }

    ui->rebootInstallUpdatesButton->setVisible(false);
    if (operation != PowerManager::All) {
        ui->powerOffButton->setVisible(false);
        ui->rebootButton->setVisible(false);
        ui->logoutButton->setVisible(false);
        ui->suspendButton->setVisible(false);
        ui->lockButton->setVisible(false);
        ui->screenOffButton->setVisible(false);
        ui->switchUsersButton->setVisible(false);
        ui->hibernateButton->setVisible(false);
    }

    switch (operation) {
        case PowerManager::All:
            if (StateManager::onboardingManager()->isOnboardingRunning()) {
                ui->lockButton->setVisible(false);
                ui->screenOffButton->setVisible(false);
            }

            Q_FALLTHROUGH();
        case PowerManager::PowerOff:
            d->timedButton = ui->powerOffButton;
            break;
        case PowerManager::Reboot:
            if (flags.contains("update")) {
                d->timedButton = ui->rebootInstallUpdatesButton;
            } else {
                d->timedButton = ui->rebootButton;
            }
            break;
        case PowerManager::LogOut:
            d->timedButton = ui->logoutButton;
            break;
        case PowerManager::Suspend:
            d->timedButton = ui->suspendButton;
            break;
        case PowerManager::Lock:
            d->timedButton = ui->lockButton;
            break;
        case PowerManager::TurnOffScreen:
            d->timedButton = ui->screenOffButton;
            break;
        case PowerManager::Hibernate:
            d->timedButton = ui->hibernateButton;
            break;

    }

    d->timedButton->setVisible(true);
    d->timedButton->setProperty("type", "destructive");

    d->powerOffAnimation = new tVariantAnimation();
    d->powerOffAnimation->setStartValue(60.0);
    d->powerOffAnimation->setEndValue(0.0);
    d->powerOffAnimation->setDuration(60000);
    connect(d->powerOffAnimation, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        const QMap<EndSessionButton*, const char*> text = {
            {ui->powerOffButton, QT_TR_NOOP("power off the system")},
            {ui->rebootButton, QT_TR_NOOP("reboot the system")},
            {ui->rebootInstallUpdatesButton, QT_TR_NOOP("reboot the system and install any pending updates")},
            {ui->logoutButton, QT_TR_NOOP("log you out")},
            {ui->suspendButton, QT_TR_NOOP("suspend the system")},
            {ui->lockButton, QT_TR_NOOP("lock the screen")},
            {ui->screenOffButton, QT_TR_NOOP("turn off the screen")},
            {ui->switchUsersButton, QT_TR_NOOP("switch users")},
            {ui->hibernateButton, QT_TR_NOOP("hibernate the system")}
        };

        ui->descriptionLabel->setText(tr(qPrintable(message), nullptr, value.toFloat() < 1 ? 1 : qRound(value.toDouble()))
            .arg(DesktopWm::userDisplayName()).arg(tr(text.value(d->timedButton))));
        d->timedButton->setTimeRemaining(value.toDouble());
    });
    connect(d->powerOffAnimation, &tVariantAnimation::finished, d->timedButton, &QCommandLinkButton::click);
    d->powerOffAnimation->start();

    ui->centeredWidget->setFixedWidth(SC_DPI(600));
    ui->centeredWidgetUpdates->setFixedWidth(SC_DPI(600));
    ui->powerOffButton->setProperty("type", "destructive");
    ui->rebootButton->setProperty("type", "destructive");
    ui->titleLabel->setBackButtonShown(true);
    ui->updatesAvailableTitle->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

EndSession::~EndSession() {
    d->powerOffAnimation->stop();
    delete d;
    delete ui;
}

tPromise<void>* EndSession::showDialog(PowerManager::PowerOperation operation, QString message, QStringList flags) {
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        Q_UNUSED(rej)

        TransparentDialog* dialog = new TransparentDialog();
        dialog->setWindowFlag(Qt::FramelessWindowHint);
        dialog->setWindowFlag(Qt::WindowStaysOnTopHint);
        dialog->showFullScreen();

        QTimer::singleShot(500, [ = ] {
            EndSession* popoverContents = new EndSession(operation, message, flags);

            tPopover* popover = new tPopover(popoverContents);
            popover->setPopoverSide(tPopover::Bottom);
            popover->setPopoverWidth(popoverContents->heightForWidth(dialog->width()));
            connect(popoverContents, &EndSession::done, popover, &tPopover::dismiss);
            connect(popover, &tPopover::dismissed, popoverContents, &EndSession::deleteLater);
            connect(popover, &tPopover::dismissed, [ = ] {
                dialog->deleteLater();
                popover->deleteLater();

                res();
            });
            popover->show(dialog);
        });
    });
}

void EndSession::on_titleLabel_backButtonClicked() {
    d->powerOffAnimation->stop();
    emit done();
}

void EndSession::on_powerOffButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::PowerOff);
    emit done();
}

void EndSession::on_rebootButton_clicked() {
    if (QFile("/var/lib/PackageKit/prepared-update").exists() && !QFile::exists("/system-update")) {
        //Ask the user to install updates
        ui->stackedWidget->setCurrentWidget(ui->updatesAvailablePage);
    } else {
        StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Reboot);
        emit done();
    }
}

void EndSession::on_logoutButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::LogOut);
    emit done();
}

void EndSession::on_suspendButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Suspend);
    emit done();
}

void EndSession::on_lockButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Lock);
    emit done();
}

void EndSession::on_screenOffButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::TurnOffScreen);
    emit done();
}

void EndSession::on_switchUsersButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::SwitchUsers);
    emit done();
}

void EndSession::on_hibernateButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Hibernate);
    emit done();
}

void EndSession::on_updatesAvailableTitle_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void EndSession::on_rebootNoUpdateButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Reboot);
    emit done();
}

void EndSession::on_rebootUpdateButton_clicked() {
    //Reboot
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Reboot, {"update"});
    emit done();
}

void EndSession::on_stackedWidget_switchingFrame(int frame) {
    if (frame == 0) {
        d->powerOffAnimation->start();
    } else {
        d->powerOffAnimation->stop();
    }
}

void EndSession::on_rebootInstallUpdatesButton_clicked() {
    StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Reboot, {"update"});
    emit done();
}

void EndSession::on_rebootButton_customContextMenuRequested(const QPoint& pos) {
    QMenu* menu = new QMenu();
    menu->addSection(tr("Advanced Reboot"));
    menu->addAction(QIcon::fromTheme("system-reboot"), tr("Reboot"), [ = ] {
        //Just reboot
        ui->rebootButton->click();
    });

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "CanRebootToFirmwareSetup");
    QDBusMessage msg = QDBusConnection::systemBus().call(message);

    if (msg.arguments().first().toString() == "yes") {
        menu->addAction(QIcon::fromTheme("system-reboot"), tr("Enter System UEFI Setup"), [ = ] {
            StateManager::instance()->powerManager()->performPowerOperation(PowerManager::Reboot, {"setup"});
            emit done();
        });
    }
    menu->popup(ui->rebootButton->mapToGlobal(pos));
}

