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
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <Wm/desktopwm.h>

#include "mainbarwidget.h"
#include "statuscenter/statuscenter.h"
#include <tvariantanimation.h>

#include <tsettings.h>

#include <statemanager.h>
#include <statuscentermanager.h>
#include <barmanager.h>

#include <Gestures/gesturedaemon.h>
#include <tpopover.h>

#include "gateway/gateway.h"

#include <keygrab.h>

struct BarWindowPrivate {
    MainBarWidget* mainBarWidget;
    StatusCenter* statusCenterWidget;

    tVariantAnimation* heightAnim;
    tVariantAnimation* barStatusCenterTransitionAnim;

    QGraphicsOpacityEffect* mainBarOpacity;
    QGraphicsOpacityEffect* statusCenterOpacity;

    QScreen* oldPrimaryScreen = nullptr;

    QList<DesktopWmWindowPtr> maximisedWindows;

    bool barExpanding = true;
    bool statusCenterShown = false;
    bool barPendingShow = false;

    GestureInteractionPtr lastGesture;

    tSettings settings;
};

BarWindow::BarWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::BarWindow) {

    ui->setupUi(this);
    d = new BarWindowPrivate();

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setMouseTracking(true);

    d->mainBarWidget = new MainBarWidget(this);
    connect(d->mainBarWidget, &MainBarWidget::expandedHeightChanged, this, &BarWindow::barHeightChanged);
    connect(d->mainBarWidget, &MainBarWidget::statusBarHeightChanged, this, &BarWindow::barHeightChanged);

    d->statusCenterWidget = new StatusCenter(this);

    d->mainBarOpacity = new QGraphicsOpacityEffect(d->mainBarWidget);
    d->mainBarOpacity->setEnabled(false);
    d->mainBarWidget->setGraphicsEffect(d->mainBarOpacity);

    d->statusCenterOpacity = new QGraphicsOpacityEffect(d->statusCenterWidget);
    d->statusCenterOpacity->setEnabled(false);
    d->statusCenterWidget->setGraphicsEffect(d->statusCenterOpacity);
    d->statusCenterWidget->setVisible(false);

    d->heightAnim = new tVariantAnimation(this);
    d->heightAnim->setDuration(500);
    connect(d->heightAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt() + 1);
    });

    connect(GestureDaemon::instance(), &GestureDaemon::gestureBegin, this, [ = ](GestureInteractionPtr gesture) {
        if (gesture->isValidInteraction(GestureTypes::Swipe, GestureTypes::Down, 3)) {
            if (d->barExpanding) {
                trackStatusCenterPullDownGesture(gesture);
            } else {
                trackBarPullDownGesture(gesture);
            }
        } else if (gesture->isValidInteraction(GestureTypes::Swipe, GestureTypes::Up, 3)) {
            if (d->statusCenterShown) {
                trackStatusCenterPullUpGesture(gesture);
            }
        }
    });

    d->barStatusCenterTransitionAnim = new tVariantAnimation();
    d->barStatusCenterTransitionAnim->setStartValue(0.0);
    d->barStatusCenterTransitionAnim->setEndValue(1.0);
    d->barStatusCenterTransitionAnim->setDuration(250);
    d->barStatusCenterTransitionAnim->setEasingCurve(QEasingCurve::InOutCubic);
    connect(d->barStatusCenterTransitionAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        double percentage = value.toDouble();
        if (qFuzzyIsNull(percentage)) { //Fully show bar view
            d->statusCenterWidget->setVisible(false);
            d->mainBarWidget->setVisible(true);
            d->statusCenterOpacity->setEnabled(false);
            d->mainBarOpacity->setEnabled(false);
            this->setFixedHeight(d->mainBarWidget->expandedHeight());
            ui->line->setVisible(true);
        } else if (qFuzzyCompare(percentage, 1)) { //Fully show Status Center view
            d->statusCenterWidget->setVisible(true);
            d->mainBarWidget->setVisible(false);
            d->statusCenterOpacity->setEnabled(false);
            d->mainBarOpacity->setEnabled(false);
            this->setFixedHeight(d->statusCenterWidget->height());
            ui->line->setVisible(false);
        } else {
            if (percentage < 0.5) { //Animate the bar
                d->mainBarOpacity->setOpacity(1 - (percentage * 2));
                d->mainBarOpacity->setEnabled(true);
                d->mainBarWidget->setVisible(true);
                d->statusCenterWidget->setVisible(false);
                d->statusCenterOpacity->setEnabled(false);
            } else { //Animate the Status Center
                d->statusCenterOpacity->setOpacity(percentage * 2 - 1);
                d->statusCenterOpacity->setEnabled(true);
                d->statusCenterWidget->setVisible(true);
                d->mainBarWidget->setVisible(false);
                d->mainBarOpacity->setEnabled(false);
            }
            this->setFixedHeight((d->statusCenterWidget->height() - d->mainBarWidget->expandedHeight()) * percentage + d->mainBarWidget->expandedHeight());
            ui->line->setVisible(true);
        }
    });

    //Tell the window manager that this is a "taskbar" type window
    this->setWindowFlag(Qt::FramelessWindowHint);
    DesktopWm::instance()->setSystemWindow(this, DesktopWm::SystemWindowTypeTaskbar);
    DesktopWm::instance()->blurWindow(this);
    this->barHeightChanged();

    connect(qApp, &QApplication::primaryScreenChanged, this, &BarWindow::updatePrimaryScreen);
    updatePrimaryScreen();

    connect(StateManager::statusCenterManager(), &StatusCenterManager::showStatusCenter, this, &BarWindow::showStatusCenter);
    connect(StateManager::statusCenterManager(), &StatusCenterManager::hideStatusCenter, this, &BarWindow::hideStatusCenter);
    connect(StateManager::barManager(), &BarManager::barLockedChanged, this, [ = ](bool isBarLocked) {
        if (!isBarLocked) {
//            showBar();
//        } else {
            if (!this->geometry().contains(QCursor::pos())) hideBar();
        }
    });

    connect(&d->settings, &tSettings::settingChanged, this, [ = ](QString key, QVariant value) {
        if (key == "Appearance/translucent") {
            this->update();
        }
    });
    connect(DesktopWm::instance(), &DesktopWm::windowAdded, this, &BarWindow::trackWindow);
    connect(DesktopWm::instance(), &DesktopWm::windowRemoved, this, [ = ](DesktopWmWindowPtr window) {
        if (d->maximisedWindows.contains(window)) {
            d->maximisedWindows.removeAll(window);
            this->update();
        }
    });
    for (const DesktopWmWindowPtr& window : DesktopWm::openWindows()) {
        trackWindow(window);
    }

    KeyGrab* statusCenterGrab = new KeyGrab(QKeySequence(Qt::MetaModifier | Qt::Key_Tab));
    connect(statusCenterGrab, &KeyGrab::activated, this, [ = ] {
        StateManager::statusCenterManager()->show();
    });

    ui->line->raise();

    //Initialise the Gateway
    Gateway::instance();
}

BarWindow::~BarWindow() {
    delete ui;
    delete d;
}

void BarWindow::resizeEvent(QResizeEvent* event) {
    ui->line->setGeometry(0, this->height() - 1, this->width(), this->height());
    d->mainBarWidget->setFixedWidth(this->width());
    d->mainBarWidget->barHeightChanged(this->height() - 1);
    d->statusCenterWidget->setFixedWidth(this->width());

    StateManager::barManager()->setBarHeight(this->height());
}

void BarWindow::enterEvent(QEnterEvent*event) {
    if (d->lastGesture && d->lastGesture->isActive()) return;
    d->barPendingShow = true;
    if ((this->layoutDirection() == Qt::RightToLeft && mapFromGlobal(QCursor::pos()).x() > d->mainBarWidget->currentAppWidgetX()) ||
        (this->layoutDirection() == Qt::LeftToRight && mapFromGlobal(QCursor::pos()).x() < d->mainBarWidget->currentAppWidgetX())) showBar();
}

void BarWindow::leaveEvent(QEvent* event) {
    if (d->lastGesture && d->lastGesture->isActive()) return;
    hideBar();
}

void BarWindow::paintEvent(QPaintEvent* event) {
    QColor bgCol = this->palette().color(QPalette::Window);
    if (d->settings.value("Appearance/translucent").toBool() && d->maximisedWindows.count() == 0) bgCol.setAlpha(150);

    QPainter painter(this);
    painter.setPen(Qt::transparent);
    painter.setBrush(bgCol);
    painter.drawRect(0, 0, this->width(), this->height());
}

void BarWindow::trackWindow(DesktopWmWindowPtr window) {
    window->disconnect(this);
    DesktopWm::instance()->disconnect(window);
    connect(window, &DesktopWmWindow::geometryChanged, this, [ = ] {
        trackWindow(window);
    });
    connect(window, &DesktopWmWindow::windowStateChanged, this, [ = ] {
        trackWindow(window);
    });
    connect(DesktopWm::instance(), &DesktopWm::currentDesktopChanged, window, [ = ] {
        trackWindow(window);
    });

    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    if (window->isMaximised() && !window->isMinimized() && screenGeometry.contains(window->geometry()) && window->isOnCurrentDesktop()) {
        if (d->maximisedWindows.contains(window)) return;
        d->maximisedWindows.append(window);
        this->update();
    } else if (!window->isMaximised() || window->isMinimized() || !window->isOnCurrentDesktop()) {
        if (!d->maximisedWindows.contains(window)) return;
        d->maximisedWindows.removeAll(window);
        this->update();
    }
}

void BarWindow::updatePrimaryScreen() {
    if (d->oldPrimaryScreen) {
        disconnect(d->oldPrimaryScreen, &QScreen::geometryChanged, this, &BarWindow::updatePrimaryScreen);
        disconnect(d->oldPrimaryScreen, &QScreen::orientationChanged, this, &BarWindow::updatePrimaryScreen);
    }

    QScreen* primaryScreen = qApp->primaryScreen();
    connect(primaryScreen, &QScreen::geometryChanged, this, &BarWindow::updatePrimaryScreen);
    connect(primaryScreen, &QScreen::orientationChanged, this, &BarWindow::updatePrimaryScreen);
    d->oldPrimaryScreen = primaryScreen;

    this->setFixedWidth(primaryScreen->geometry().width());
    this->move(primaryScreen->geometry().topLeft());
    d->statusCenterWidget->setFixedHeight(primaryScreen->geometry().height());

    //Refresh the state of all the windows
    for (const DesktopWmWindowPtr& window : DesktopWm::openWindows()) {
        trackWindow(window);
    }

    barHeightChanged();
}

void BarWindow::barHeightChanged() {
    DesktopWm::setScreenMarginForWindow(this, qApp->primaryScreen(), Qt::TopEdge, d->mainBarWidget->statusBarHeight() + 1);
    d->mainBarWidget->setFixedHeight(d->mainBarWidget->expandedHeight());

    if (!d->statusCenterShown) {
        QSignalBlocker blocker(d->heightAnim);
        d->heightAnim->setStartValue(this->height() - 1);
        d->heightAnim->setEndValue(d->barExpanding ? d->mainBarWidget->expandedHeight() : d->mainBarWidget->statusBarHeight());
        d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);

        d->heightAnim->stop();
        d->heightAnim->start();
    }
}

void BarWindow::showStatusCenter() {
    QSignalBlocker blocker(d->heightAnim);
    d->heightAnim->stop();

    StateManager::statusCenterManager()->setIsShowingStatusCenter(true);
    int time = d->barStatusCenterTransitionAnim->currentTime();
    d->barStatusCenterTransitionAnim->setDirection(tVariantAnimation::Forward);
    d->barStatusCenterTransitionAnim->start();
    d->barStatusCenterTransitionAnim->setCurrentTime(time);

    //Tell the window manager that this is now a standard system window
    DesktopWm::instance()->setSystemWindow(this);
    d->statusCenterWidget->setFocus();
    d->statusCenterShown = true;
}

void BarWindow::hideStatusCenter() {
    StateManager::statusCenterManager()->setIsShowingStatusCenter(false);
    int time = d->barStatusCenterTransitionAnim->currentTime();
    d->barStatusCenterTransitionAnim->setDirection(tVariantAnimation::Backward);
    d->barStatusCenterTransitionAnim->start();
    d->barStatusCenterTransitionAnim->setCurrentTime(time);

    //Tell the window manager that this is now a "taskbar" type window
    DesktopWm::instance()->setSystemWindow(this, DesktopWm::SystemWindowTypeTaskbar);
    d->mainBarWidget->setFocus();
    d->statusCenterShown = false;
}

void BarWindow::showBar() {
    d->barPendingShow = false;
    //If we're showing the status bar, don't touch the height
    if (!StateManager::statusCenterManager()->isShowingStatusCenter()) {
        QSignalBlocker blocker(d->heightAnim);
        d->heightAnim->setStartValue(this->height() - 1);
        d->heightAnim->setEndValue(d->mainBarWidget->expandedHeight());
        d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);

        d->heightAnim->stop();
        d->heightAnim->start();

        d->barExpanding = true;
    }
}

void BarWindow::hideBar() {
    //If we're showing the status bar, don't touch the height
    if (!StateManager::statusCenterManager()->isShowingStatusCenter() && !StateManager::barManager()->isBarLocked()) {
        QSignalBlocker blocker(d->heightAnim);
        d->heightAnim->setStartValue(this->height() - 1);
        d->heightAnim->setEndValue(d->mainBarWidget->statusBarHeight());
        d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);

        d->heightAnim->stop();
        d->heightAnim->start();

        d->barExpanding = false;
    }
}

void BarWindow::trackBarPullDownGesture(GestureInteractionPtr gesture) {
    if (d->statusCenterShown || d->barExpanding) return;

    //Capture this gesture!
    d->lastGesture = gesture;

    QSignalBlocker blocker(d->heightAnim);

    d->heightAnim->setStartValue(this->height() - 1);
    d->heightAnim->setEndValue(d->mainBarWidget->expandedHeight());
    d->heightAnim->setEasingCurve(QEasingCurve::Linear);

    d->heightAnim->stop();
    d->barExpanding = true;

    connect(gesture.data(), &GestureInteraction::interactionUpdated, this, [ = ] {
        d->heightAnim->setCurrentTime(d->heightAnim->totalDuration() * gesture->percentage());
        d->heightAnim->valueChanged(d->heightAnim->currentValue());
    });
    connect(gesture.data(), &GestureInteraction::interactionEnded, this, [ = ] {
        if (gesture->extrapolatePercentage(100) > 0.7) {
            showBar();
            QTimer::singleShot(3000, this, [ = ] {
                if (gesture == d->lastGesture && !this->underMouse()) hideBar();
            });
        } else {
            hideBar();
        }
    });
}

void BarWindow::trackStatusCenterPullDownGesture(GestureInteractionPtr gesture) {
    if (d->statusCenterShown) return;

    //Capture this gesture!
    d->lastGesture = gesture;

    d->barStatusCenterTransitionAnim->setDirection(tVariantAnimation::Forward);
    d->barStatusCenterTransitionAnim->setCurrentTime(0);

    connect(gesture.data(), &GestureInteraction::interactionUpdated, this, [ = ] {
        d->barStatusCenterTransitionAnim->setCurrentTime(d->barStatusCenterTransitionAnim->totalDuration() * gesture->percentage());
        d->barStatusCenterTransitionAnim->valueChanged(d->barStatusCenterTransitionAnim->currentValue());
    });
    connect(gesture.data(), &GestureInteraction::interactionEnded, this, [ = ] {
        if (gesture->extrapolatePercentage(100) > 0.7) {
            showStatusCenter();
        } else {
            hideStatusCenter();
        }
    });
}

void BarWindow::trackStatusCenterPullUpGesture(GestureInteractionPtr gesture) {
    if (!d->statusCenterShown) return;

    //Only accept this gesture if there is not currently a popover active
    QList<QWidget*> childWidgets;
    childWidgets.append(this);
    while (!childWidgets.isEmpty()) {
        QWidget* w = childWidgets.takeFirst();
        if (tPopover::popoverForPopoverWidget(w)) return;
        for (QObject* o : w->children()) {
            if (qobject_cast<QWidget*>(o)) childWidgets.append(qobject_cast<QWidget*>(o));
        }
    }

    //Capture this gesture!
    d->lastGesture = gesture;

    d->barStatusCenterTransitionAnim->setDirection(tVariantAnimation::Forward);
    d->barStatusCenterTransitionAnim->setCurrentTime(d->barStatusCenterTransitionAnim->duration());

    connect(gesture.data(), &GestureInteraction::interactionUpdated, this, [ = ] {
        d->barStatusCenterTransitionAnim->setCurrentTime(d->barStatusCenterTransitionAnim->totalDuration() - d->barStatusCenterTransitionAnim->totalDuration() * gesture->percentage());
        d->barStatusCenterTransitionAnim->valueChanged(d->barStatusCenterTransitionAnim->currentValue());
    });
    connect(gesture.data(), &GestureInteraction::interactionEnded, this, [ = ] {
        if (gesture->extrapolatePercentage(100) > 0.3) {
            hideStatusCenter();
        } else {
            showStatusCenter();
        }
    });
}

void BarWindow::mouseMoveEvent(QMouseEvent* event) {
    if (event->pos().x() < d->mainBarWidget->currentAppWidgetX() && d->barPendingShow) showBar();
}
