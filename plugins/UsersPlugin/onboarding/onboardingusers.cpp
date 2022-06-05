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
#include "onboardingusers.h"
#include "ui_onboardingusers.h"

#include "settings/user.h"
#include "settings/usersmodel.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <onboardingmanager.h>
#include <statemanager.h>
#include <terrorflash.h>
#include <ttoast.h>

struct OnboardingUsersPrivate {
        bool userAdded = false;
        QString firstUser;
};

OnboardingUsers::OnboardingUsers(QWidget* parent) :
    OnboardingPage(parent),
    ui(new Ui::OnboardingUsers) {
    ui->setupUi(this);

    d = new OnboardingUsersPrivate();

    ui->addUserTitleLabel->setBackButtonShown(true);
    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->addUserDescriptionLabel->setText(tr("It's time to get to know you. Apps will use this information to personalise your experience, and you'll use the password to log in."));
    ui->accountTypeWidget->setVisible(false);
    ui->userList->setModel(new UsersModel(this));
}

OnboardingUsers::~OnboardingUsers() {
    delete d;
    delete ui;
}

void OnboardingUsers::on_addUserTitleLabel_backButtonClicked() {
    if (d->userAdded) {
        ui->stackedWidget->setCurrentWidget(ui->allUsersPage);
    } else {
        StateManager::onboardingManager()->previousStep();
    }
}

void OnboardingUsers::on_addUserCompleteButton_clicked() {
    if (ui->fullNameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->fullNameBox);
        return;
    }

    if (ui->usernameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->usernameBox);
        return;
    }

    if (ui->passwordBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->passwordBox);
        return;
    }

    if (ui->passwordBox->text() != ui->confirmPasswordBox->text()) {
        tErrorFlash::flashError(ui->confirmPasswordBox);
        return;
    }
    ui->stackedWidget->setCurrentWidget(ui->spinnerPage);

    // Add the user account
    int accountType = d->userAdded ? (ui->administratorButton->isChecked() ? 1 : 0) : 1;
    QDBusMessage createMessage = QDBusMessage::createMethodCall("org.freedesktop.Accounts", "/org/freedesktop/Accounts", "org.freedesktop.Accounts", "CreateUser");
    createMessage.setArguments({ui->usernameBox->text(),
        ui->fullNameBox->text(),
        accountType});

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(createMessage));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        QString error;
        if (watcher->isError()) {
            error = watcher->error().message();
        } else {
            UserPtr u(new User(watcher->reply().arguments().first().value<QDBusObjectPath>()));

            // Set the user's password
            tPromiseResults<void> results;
            results = u->setPassword(ui->passwordBox->text(), ui->passwordHintBox->text())->await();

            if (results.error.isEmpty()) {
                error = results.error;
            }
        }

        if (error.isEmpty()) {
            if (d->userAdded) {
                ui->userListDescriptionLabel->setText(tr("You've added these users to your system"));
            } else {
                ui->userListDescriptionLabel->setText(tr("Thanks, %1. If other people will be using this device, you can add them now, or you can add them later.").arg(ui->fullNameBox->text()));
            }

            ui->stackedWidget->setCurrentWidget(ui->allUsersPage);
            d->userAdded = true;
            ui->accountTypeWidget->setVisible(true);

            ui->addUserDescriptionLabel->setText(tr("Enter the details of the next user to be added"));
        } else {
            // Bail out
            QTimer::singleShot(1000, [=] {
                ui->stackedWidget->setCurrentWidget(ui->addUserPage);

                tToast* toast = new tToast();
                toast->setTitle(tr("Couldn't create user"));
                toast->setText(error);
                connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
                toast->show(this);
            });
        }
    });
}

void OnboardingUsers::on_addUserButton_clicked() {
    resetAddUserForm();
    ui->stackedWidget->setCurrentWidget(ui->addUserPage);
}

void OnboardingUsers::on_nextButton_clicked() {
    StateManager::onboardingManager()->nextStep();
}

void OnboardingUsers::on_titleLabel_backButtonClicked() {
    StateManager::onboardingManager()->previousStep();
}

QString OnboardingUsers::name() {
    return QStringLiteral("OnboardingUsers");
}

QString OnboardingUsers::displayName() {
    return tr("Users");
}

void OnboardingUsers::on_fullNameBox_textChanged(const QString& arg1) {
    ui->usernameBox->setText(arg1.split(" ").first().toLower());
}

void OnboardingUsers::resetAddUserForm() {
    ui->fullNameBox->clear();
    ui->usernameBox->clear();
    ui->passwordBox->clear();
    ui->confirmPasswordBox->clear();
    ui->passwordHintBox->clear();
    ui->administratorButton->setChecked(false);
    ui->standardUserButton->setChecked(true);
}

void OnboardingUsers::on_administratorButton_clicked() {
    ui->administratorButton->setChecked(true);
    ui->standardUserButton->setChecked(false);
}

void OnboardingUsers::on_standardUserButton_clicked() {
    ui->administratorButton->setChecked(false);
    ui->standardUserButton->setChecked(true);
}
