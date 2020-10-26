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
#include "authwindow.h"
#include "ui_authwindow.h"

#include <PolkitQt1/Agent/Session>
#include <tpopover.h>
#include <QScreen>
#include <QShortcut>
#include <ttoast.h>
#include "overlaywindow.h"
#include "identitypopover.h"
#include "common.h"

struct AuthWindowPrivate {
    OverlayWindow overlay;
    PolkitQt1::Identity::List identities;
    PolkitQt1::Agent::AsyncResult* callback = nullptr;

    PolkitQt1::Agent::Session* session = nullptr;
    QString cookie;
};

AuthWindow::AuthWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AuthWindow) {
    ui->setupUi(this);

    d = new AuthWindowPrivate();
    ui->titleLabel->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, [ = ] {
        ui->titleLabel->backButtonClicked();
    });
}

AuthWindow::~AuthWindow() {
    delete d;
    delete ui;
}

void AuthWindow::setMessage(QString message) {
    ui->messageLabel->setText(message);
    ui->customMessageLabel->setText(message);
}

void AuthWindow::setIdentities(PolkitQt1::Identity::List identities) {
    d->identities = identities;
    ui->changeIdentity->setVisible(!identities.isEmpty());
    ui->customChangeIdentity->setVisible(!identities.isEmpty());
}

void AuthWindow::setCallback(PolkitQt1::Agent::AsyncResult* result) {
    d->callback = result;
}

void AuthWindow::setCookie(QString cookie) {
    d->cookie = cookie;
}

void AuthWindow::showAuthWindow() {
    initiateSession(d->identities.first());

    QScreen* screen = QApplication::screenAt(QCursor::pos());
    d->overlay.setGeometry(screen->geometry());
    d->overlay.showFullScreen();

    QTimer::singleShot(0, [ = ] {
        tPopover* popover = new tPopover(this);
        popover->setPopoverWidth(SC_DPI(600));
        popover->setPopoverSide(tPopover::Bottom);
        popover->setPerformBlur(false);
        connect(ui->titleLabel, &tTitleLabel::backButtonClicked, popover, &tPopover::dismiss);
        connect(this, &AuthWindow::dismiss, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, this, [ = ] {
            if (d->session) {
                d->session->setProperty("cancelled", true);
                d->session->cancel();
                d->session->deleteLater();
            }

            if (d->callback) {
                d->callback->setError("org.freedesktop.PolicyKit1.Error.Cancelled");
                d->callback->setCompleted();
            }

            d->overlay.close();
            popover->deleteLater();
        });
        popover->show(&d->overlay);
    });
}

void AuthWindow::cancel() {
    ui->titleLabel->backButtonClicked();
}

void AuthWindow::initiateSession(PolkitQt1::Identity identity) {
    if (d->session) {
        d->session->setProperty("cancelled", true);
        d->session->cancel();
        d->session->deleteLater();
    }

    ui->stackedWidget->setCurrentWidget(ui->loadingPage, false);
    ui->usernameLabel->setText(Common::stringForIdentity(identity));
    ui->customUsernameLabel->setText(Common::stringForIdentity(identity));
    d->session = new PolkitQt1::Agent::Session(d->identities.first(), d->cookie);
    d->session->setProperty("cancelled", false);
    connect(d->session, &PolkitQt1::Agent::Session::request, this, [ = ](QString request, bool echo) {
        if (request.startsWith("password:", Qt::CaseInsensitive)) {
            //Use the password pane
            ui->passwordBox->setText("");
            ui->passwordBox->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
            ui->stackedWidget->setCurrentWidget(ui->passwordPage);
            ui->passwordBox->setFocus();
        } else {
            //Use the request pane
            ui->responseBox->setText("");
            ui->responseBox->setPlaceholderText(request);
            ui->responseBox->setEchoMode(echo ? QLineEdit::Normal : QLineEdit::Password);
            ui->stackedWidget->setCurrentWidget(ui->customResponsePage);
            ui->responseBox->setFocus();
        }
    });
    connect(d->session, &PolkitQt1::Agent::Session::completed, this, [ = ](bool gainedAuthorisation) {
        if (!d->session->property("cancelled").toBool()) {
            if (gainedAuthorisation) {
                d->session->deleteLater();
                d->session = nullptr;

                d->callback->setCompleted();
                emit dismiss();
            } else {
                initiateSession(identity);
            }
        }
    });
    connect(d->session, &PolkitQt1::Agent::Session::showError, this, [ = ](QString error) {
        tToast* toast = new tToast(this);
        toast->setTitle(tr("Error"));
        toast->setText(error);
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(this);
    });
    connect(d->session, &PolkitQt1::Agent::Session::showInfo, this, [ = ](QString info) {
        tToast* toast = new tToast(this);
        toast->setTitle(tr("Message"));
        toast->setText(info);
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(this);
    });
    d->session->initiate();
}

void AuthWindow::on_okPasswordButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    d->session->setResponse(ui->passwordBox->text());
}

void AuthWindow::on_changeIdentity_clicked() {
    IdentityPopover* id = new IdentityPopover(d->identities, this);
    tPopover* popover = new tPopover(id);
    popover->setPopoverWidth(SC_DPI(400));
    popover->setPopoverSide(tPopover::Bottom);
    connect(id, &IdentityPopover::selectIdentity, this, [ = ](PolkitQt1::Identity identity) {
        this->initiateSession(identity);
    });
    connect(id, &IdentityPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, id, &IdentityPopover::deleteLater);
    popover->show(this);
}

void AuthWindow::on_customChangeIdentity_clicked() {
    ui->changeIdentity->click();
}

void AuthWindow::on_okResponseButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);
    d->session->setResponse(ui->responseBox->text());
}
