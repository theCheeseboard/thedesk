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
#include "hudwidget.h"
#include "ui_hudwidget.h"

#include <statemanager.h>
#include <hudmanager.h>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <tvariantanimation.h>
#include <QTimer>
#include <QPainter>

struct HudWidgetPrivate {
    QTimer* hideTimer;
    bool shouldShow = false;

    int state = 0;
    double value = 0;
};

HudWidget::HudWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::HudWidget) {
    ui->setupUi(this);
    d = new HudWidgetPrivate();

    this->setFixedHeight(0);

    d->hideTimer = new QTimer(this);
    d->hideTimer->setInterval(3000);
    d->hideTimer->setSingleShot(true);
    connect(d->hideTimer, &QTimer::timeout, this, &HudWidget::animateHide);

    connect(StateManager::hudManager(), &HudManager::requestHud, this, [ = ](QVariantMap params) {
        QPixmap icon = QIcon::fromTheme(params.value("icon", "").toString()).pixmap(SC_DPI_T(QSize(32, 32), QSize));
        QString title = params.value("title", "").toString().toUpper();
        QString text = params.value("text", "").toString();
        double value = params.value("value", 0).toDouble();
        int timeout = params.value("timeout", 3000).toInt();

        ui->iconValueIcon->setPixmap(icon);
        ui->iconTextIcon->setPixmap(icon);
        ui->iconValueTitle->setText(title);
        ui->iconTextTitle->setText(title);
        ui->iconTextText->setText(text);
        ui->iconValueValue->setText(QLocale().toString(value * 100, 'f', 0) + "%");

        d->hideTimer->setInterval(timeout);

        //Decide on the layout to use
        if (params.contains("icon") && params.contains("title") && params.contains("value")) {
            //Use the icon/value HUD
            ui->stackedWidget->setCurrentWidget(ui->iconValuePage);
            d->value = value;
            ui->iconValuePage->update();
        } else if (params.contains("icon") && params.contains("title") && params.contains("text")) {
            //Use the icon/text HUD
            ui->stackedWidget->setCurrentWidget(ui->iconTextPage);
        }

        this->animateShow();
    });
    connect(StateManager::hudManager(), &HudManager::requestHideHud, this, [ = ] {
        //Immediately hide the HUD
        this->animateHide();
    });

    ui->iconValuePage->installEventFilter(this);
}

HudWidget::~HudWidget() {
    delete d;
    delete ui;
}

bool HudWidget::shouldShow() {
    return d->shouldShow;
}

void HudWidget::resizeEvent(QResizeEvent* event) {
    ui->stackedWidget->setFixedWidth(this->width() - SC_DPI(18));
    ui->stackedWidget->setFixedHeight(ui->stackedWidget->sizeHint().height());
    ui->stackedWidget->move(SC_DPI(9), this->height() - ui->stackedWidget->height());
}

bool HudWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Paint && watched == ui->iconValuePage) {
        QPainter painter(ui->iconValuePage);
        painter.setBrush(QColor(255, 255, 255, 50));
        painter.setPen(Qt::transparent);

        double val = d->value;
        while (val > 1) {
            painter.drawRect(0, 0, ui->iconValuePage->width(), ui->iconValuePage->height());
            val -= 1;
        }

        painter.drawRect(0, 0, static_cast<int>(ui->iconValuePage->width() * val), ui->iconValuePage->height());
        return true;
    }
    return false;
}

void HudWidget::animateShow() {
    //Make sure we're idle
    if (d->state != 0) {
        if (d->state == 2) {
            d->hideTimer->stop();
            d->hideTimer->start();
        }
        return;
    }
    d->state = 1;

    d->hideTimer->stop();

    d->shouldShow = true;
    emit shouldShowChanged();

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(0);
    anim->setEndValue(ui->stackedWidget->height() + SC_DPI(9));
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setDuration(250);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt());
        emit shouldShowChanged();
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        anim->deleteLater();
        d->hideTimer->start();
        d->state = 2;
    });
    anim->start();
}

void HudWidget::animateHide() {
    if (d->state != 2) return; //Make sure we're showing
    d->state = 3;

    d->hideTimer->stop();

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(this->height());
    anim->setEndValue(0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setDuration(250);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt());
        emit shouldShowChanged();
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        anim->deleteLater();

        d->state = 0;
        d->shouldShow = false;
        emit shouldShowChanged();
    });
    anim->start();
}
