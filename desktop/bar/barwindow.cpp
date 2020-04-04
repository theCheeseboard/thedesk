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
#include "barwindow.h"
#include "ui_barwindow.h"

#include <QScreen>
#include <Wm/desktopwm.h>

#include "mainbarwidget.h"
#include <tvariantanimation.h>

struct BarWindowPrivate {
    MainBarWidget* mainBarWidget;
    tVariantAnimation* heightAnim;

    bool expanding = true;
};

BarWindow::BarWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::BarWindow) {

    ui->setupUi(this);
    d = new BarWindowPrivate();

    d->mainBarWidget = new MainBarWidget(this);
    connect(d->mainBarWidget, &MainBarWidget::expandedHeightChanged, this, &BarWindow::barHeightChanged);
    connect(d->mainBarWidget, &MainBarWidget::statusBarHeight, this, &BarWindow::barHeightChanged);

    d->heightAnim = new tVariantAnimation(this);
    d->heightAnim->setDuration(500);
    d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(d->heightAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt() + 1);
    });

    //Tell the window manager that this is a "taskbar" type window
    DesktopWm::instance()->setSystemWindow(this, DesktopWm::SystemWindowTypeTaskbar);
    this->barHeightChanged();

    connect(qApp, &QApplication::primaryScreenChanged, this, &BarWindow::updatePrimaryScreen);
    updatePrimaryScreen();

    ui->line->raise();
}

BarWindow::~BarWindow() {
    delete ui;
    delete d;
}

void BarWindow::resizeEvent(QResizeEvent* event) {
    ui->line->setGeometry(0, this->height() - 1, this->width(), this->height());
    ui->mainWidget->setFixedWidth(this->width());
    ui->mainWidget->barHeightChanged(this->height() - 1);
}

void BarWindow::enterEvent(QEvent* event) {
    d->heightAnim->setStartValue(this->height() - 1);
    d->heightAnim->setEndValue(d->mainBarWidget->expandedHeight());

    d->heightAnim->stop();
    d->heightAnim->start();
}

void BarWindow::leaveEvent(QEvent* event) {
    d->heightAnim->setStartValue(this->height() - 1);
    d->heightAnim->setEndValue(d->mainBarWidget->statusBarHeight());

    d->heightAnim->stop();
    d->heightAnim->start();
}

void BarWindow::updatePrimaryScreen() {
    QScreen* primaryScreen = qApp->primaryScreen();
    this->setFixedWidth(primaryScreen->geometry().width());
    this->move(primaryScreen->geometry().topLeft());
}

void BarWindow::barHeightChanged() {
    d->heightAnim->setStartValue(this->height() - 1);
    d->heightAnim->setEndValue(d->expanding ? d->mainBarWidget->expandedHeight() : d->mainBarWidget->statusBarHeight());

    d->heightAnim->stop();
    d->heightAnim->start();
    d->mainBarWidget->setFixedHeight(d->mainBarWidget->expandedHeight());

    DesktopWm::setScreenMarginForWindow(this, qApp->primaryScreen(), Qt::TopEdge, d->mainBarWidget->statusBarHeight() + 1);
}

