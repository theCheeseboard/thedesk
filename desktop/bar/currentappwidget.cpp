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
#include "currentappwidget.h"
#include "ui_currentappwidget.h"

#include <tvariantanimation.h>
#include <Wm/desktopwm.h>
#include <Applications/application.h>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <keygrab.h>
#include <private/quickwidgetcontainer.h>
#include "currentappwidgetmenu.h"

struct CurrentAppWidgetPrivate {
    QPalette pal;
    QGraphicsOpacityEffect* opacity;
    tVariantAnimation* anim;

    QuickWidgetContainer* menuContainer;
    CurrentAppWidgetMenu* menu;

    KeyGrab* forceStopGrab;
};

CurrentAppWidget::CurrentAppWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CurrentAppWidget) {
    ui->setupUi(this);

    d = new CurrentAppWidgetPrivate();
    d->pal = this->palette();
    d->pal.setColor(QPalette::Window, QColor(0, 0, 0, 0));
    this->setPalette(d->pal);

    d->opacity = new QGraphicsOpacityEffect(this);
    d->opacity->setOpacity(1);
    this->setGraphicsEffect(d->opacity);

    d->anim = new tVariantAnimation(this);
    d->anim->setStartValue(0.0);
    d->anim->setEndValue(1.0);
    d->anim->setDuration(200);
    connect(d->anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        d->opacity->setOpacity(value.toReal());
    });

    d->menu = new CurrentAppWidgetMenu();

    d->menuContainer = new QuickWidgetContainer(this);
    d->menuContainer->setQuickWidget(d->menu);
    connect(d->menuContainer, &QuickWidgetContainer::hiding, this, &CurrentAppWidget::activeWindowChanged);
    connect(d->menu, &CurrentAppWidgetMenu::done, d->menuContainer, &QuickWidgetContainer::hideContainer);

    connect(DesktopWm::instance(), &DesktopWm::activeWindowChanged, this, &CurrentAppWidget::activeWindowChanged);
    activeWindowChanged();

    d->forceStopGrab = new KeyGrab(QKeySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_Escape), "force-stop");
    connect(d->forceStopGrab, &KeyGrab::activated, this, [ = ] {
        DesktopWmWindowPtr active = DesktopWm::activeWindow();
        if (!active->application()) return;
        d->menu->setWindow(active);
        d->menu->showForceStopScreen();

        if (!d->menuContainer->isShowing()) d->menuContainer->showContainer();
    });
}

CurrentAppWidget::~CurrentAppWidget() {
    delete d;
    delete ui;
}

void CurrentAppWidget::barHeightChanging(float barTransitionPercentage) {
    if (qFuzzyIsNull(barTransitionPercentage)) {
        this->setFixedWidth(QWIDGETSIZE_MAX);
        this->setVisible(true);
    } else if (qFuzzyCompare(barTransitionPercentage, 1)) {
        this->setVisible(false);
    } else {
        this->setFixedWidth(this->sizeHint().width() * (1 - barTransitionPercentage));
        this->setVisible(true);
    }
}

void CurrentAppWidget::activeWindowChanged() {
    if (d->menuContainer->isShowing()) return;
    DesktopWmWindowPtr active = DesktopWm::activeWindow();
    if (active) {
        ApplicationPointer app = active->application();
        if (app) {
            ui->iconLabel->setPixmap(QIcon::fromTheme(app->getProperty("Icon").toString()).pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            ui->currentAppLabel->setText(app->getProperty("Name").toString());

            if (d->anim->direction() == tVariantAnimation::Backward) {
                d->anim->setDirection(tVariantAnimation::Forward);
                d->anim->start();
            }
        } else {
            if (d->anim->direction() == tVariantAnimation::Forward) {
                d->anim->setDirection(tVariantAnimation::Backward);
                d->anim->start();
            }
        }
    } else {
        ui->iconLabel->setPixmap(QIcon(":/thedesk/desktop/thedesk.svg").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
        ui->currentAppLabel->setText(QStringLiteral("theDesk"));
        if (d->anim->direction() == tVariantAnimation::Backward) {
            d->anim->setDirection(tVariantAnimation::Forward);
            d->anim->start();
        }
    }
}

void CurrentAppWidget::enterEvent(QEvent* event) {
    QPalette pal = d->pal;
    pal.setColor(QPalette::Window, QColor(255, 255, 255, 100));
    this->setPalette(pal);
}

void CurrentAppWidget::leaveEvent(QEvent* event) {
    this->setPalette(d->pal);
}

void CurrentAppWidget::mousePressEvent(QMouseEvent* event) {
    QPalette pal = d->pal;
    pal.setColor(QPalette::Window, QColor(0, 0, 0, 100));
    this->setPalette(pal);
}

void CurrentAppWidget::mouseReleaseEvent(QMouseEvent* event) {
    QPalette pal = d->pal;
    pal.setColor(QPalette::Window, QColor(255, 255, 255, 100));
    this->setPalette(pal);

    if (this->underMouse()) {
        //Click!
        DesktopWmWindowPtr active = DesktopWm::activeWindow();
        if (!active || !active->application()) return;
        d->menu->setWindow(active);
        if (!d->menuContainer->isShowing()) d->menuContainer->showContainer();
    }
}
