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
#include "splashwindow.h"
#include "ui_splashwindow.h"

#include <tvariantanimation.h>
#include "splash/splashcontroller.h"

SplashWindow::SplashWindow(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SplashWindow) {
    ui->setupUi(this);

    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);

    connect(SplashController::instance(), &SplashController::starting, this, [ = ] {
        ui->stackedWidget->setCurrentWidget(ui->splashPage, this->isVisible());
        this->showFullScreen();
    });
    connect(SplashController::instance(), &SplashController::hideSplashes, this, [ = ] {
        this->hide();
    });
    connect(SplashController::instance(), &SplashController::crash, this, [ = ] {
        ui->stackedWidget->setCurrentWidget(ui->crashPage, this->isVisible());
        this->showFullScreen();
    });
    connect(SplashController::instance(), &SplashController::startFail, this, [ = ] {
        ui->stackedWidget->setCurrentWidget(ui->crashPage, this->isVisible());
        this->showFullScreen();
    });
    connect(SplashController::instance(), &SplashController::question, this, [ = ](QString title, QString message) {
        ui->questionTitle->setText(title);
        ui->questionText->setText(message);
        ui->stackedWidget->setCurrentWidget(ui->questionPage, this->isVisible());
        this->showFullScreen();
    });
}

SplashWindow::~SplashWindow() {
    delete ui;
}

void SplashWindow::hide() {
    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setDuration(500);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setWindowOpacity(value.toDouble());
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        anim->deleteLater();
        QDialog::hide();
        this->setWindowOpacity(1);
    });
    anim->start();
}

void SplashWindow::on_yesButton_clicked() {
    SplashController::instance()->respond(true);
    ui->stackedWidget->setCurrentWidget(ui->splashPage);
}

void SplashWindow::on_noButton_clicked() {
    SplashController::instance()->respond(false);
    ui->stackedWidget->setCurrentWidget(ui->splashPage);
}
