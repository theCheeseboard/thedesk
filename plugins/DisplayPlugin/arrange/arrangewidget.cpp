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
#include "arrangewidget.h"
#include "ui_arrangewidget.h"

#include "overlaywindow.h"
#include <Screens/screendaemon.h>
#include <Screens/systemscreen.h>
#include <tpopover.h>
#include <QTimer>
#include <tscrim.h>

struct ArrangeWidgetPrivate {
    OverlayWindow* overlay;
    SystemScreen* screen = nullptr;
    tPopover* popover;

    QMap<SystemScreen*, QPushButton*> buttons;

    bool init = true;
};

ArrangeWidget::ArrangeWidget(SystemScreen* screen, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ArrangeWidget) {
    ui->setupUi(this);

    d = new ArrangeWidgetPrivate();

    ui->titleLabel->setBackButtonShown(true);
    ui->screensWidget->setFixedWidth(SC_DPI(600));
    ui->displayPropertiesWidget->setFixedWidth(SC_DPI(600));
    ui->applyButton->setFixedWidth(SC_DPI(600));

    d->overlay = new OverlayWindow();
    d->overlay->show();

    tScrim::scrimForWidget(d->overlay)->setBlurEnabled(false);

    d->popover = new tPopover(this);
    d->popover->setDismissable(false);
    d->popover->setPopoverSide(tPopover::Bottom);
    d->popover->setPopoverWidth(SC_DPI(600));
    connect(d->popover, &tPopover::dismissed, this, &ArrangeWidget::deleteLater);
    connect(d->popover, &tPopover::dismissed, d->popover, &tPopover::deleteLater);
    QTimer::singleShot(500, [ = ] {
        d->popover->show(d->overlay);
    });

    updateScreenList();
    setScreen(screen);
}

ArrangeWidget::~ArrangeWidget() {
    d->overlay->deleteLater();
    delete d;
    delete ui;
}

void ArrangeWidget::dismiss() {
    d->popover->dismiss();
}

void ArrangeWidget::on_titleLabel_backButtonClicked() {
    emit reject();
}

void ArrangeWidget::updateScreenGeometry() {
    d->overlay->setGeometry(d->screen->geometry());
    d->overlay->showFullScreen();
}

void ArrangeWidget::updateAvailableModes() {
    QSignalBlocker blocker(ui->resolutionBox);
    ui->resolutionBox->clear();

    QList<QSize> resolutions;
    for (SystemScreen::Mode mode : d->screen->availableModes()) {
        QSize res = QSize(mode.width, mode.height);
        if (!resolutions.contains(res)) {
            resolutions.append(res);
            ui->resolutionBox->addItem(QStringLiteral("%1 × %2").arg(res.width()).arg(res.height()), res);
        }
        if (d->init && d->screen->currentMode() == mode.id) ui->resolutionBox->setCurrentIndex(resolutions.indexOf(res));
    }

    updateRefreshRateBox();
}

void ArrangeWidget::updateRefreshRateBox() {
    QSignalBlocker blocker(ui->refreshRateBox);
    QSize selected = ui->resolutionBox->currentData().toSize();

    ui->refreshRateBox->clear();
    int select = -1;
    for (SystemScreen::Mode mode : d->screen->availableModes()) {
        QSize res = QSize(mode.width, mode.height);
        if (res == selected) {
            ui->refreshRateBox->addItem(tr("%1 hz").arg(mode.framerate, 0, 'f', 2), mode.id);
            if (d->init && d->screen->currentMode() == mode.id) select = ui->refreshRateBox->count() - 1;
        }
    }

    if (select != -1) ui->refreshRateBox->setCurrentIndex(select);
}

void ArrangeWidget::updateOrientationBox() {
    QSignalBlocker blocker(ui->orientationBox);
    ui->orientationBox->setCurrentIndex(d->screen->currentRotation());
}

void ArrangeWidget::updateIsPrimary() {
    ui->primaryDisplaySwitch->setChecked(d->screen->isPrimary());
    ui->primaryDisplaySwitch->setEnabled(!d->screen->isPrimary());
}

void ArrangeWidget::updatePowered() {
    QSignalBlocker blocker(ui->enableDisplaySwitch);
    ui->enableDisplaySwitch->setChecked(d->screen->powered());
    ui->displaySettingsWidget->setExpanded(d->screen->powered());
}

void ArrangeWidget::updateScreenList() {
    for (QPushButton* button : d->buttons.values()) {
        ui->screensLayout->removeWidget(button);
        button->deleteLater();
    }
    d->buttons.clear();

    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        QPushButton* button = new QPushButton(this);
        button->setText(screen->displayName());
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setChecked(d->screen == screen);
        connect(button, &QPushButton::toggled, this, [ = ](bool checked) {
            if (checked) setScreen(screen);
        });
        ui->screensLayout->addWidget(button);
        d->buttons.insert(screen, button);
    }
}

void ArrangeWidget::setScreen(SystemScreen* screen) {
    d->init = true;
    if (d->screen) {
        d->screen->disconnect(this);
    }

    d->screen = screen;

    connect(screen, &SystemScreen::rotationChanged, this, &ArrangeWidget::updateOrientationBox);
    connect(screen, &SystemScreen::geometryChanged, this, &ArrangeWidget::updateScreenGeometry);
    connect(screen, &SystemScreen::availableModesChanged, this, &ArrangeWidget::updateAvailableModes);
    connect(screen, &SystemScreen::currentModeChanged, this, &ArrangeWidget::updateAvailableModes);
    connect(screen, &SystemScreen::isPrimaryChanged, this, &ArrangeWidget::updateIsPrimary);
    connect(screen, &SystemScreen::poweredChanged, this, &ArrangeWidget::updatePowered);
    updateScreenGeometry();
    updateAvailableModes();
    updateOrientationBox();
    updateIsPrimary();
    updatePowered();

    d->buttons.value(screen)->setChecked(true);

    ui->titleLabel->setText(d->screen->displayName());
    d->init = false;
}

void ArrangeWidget::on_refreshRateBox_currentIndexChanged(int index) {
    d->screen->setCurrentMode(ui->refreshRateBox->currentData().toInt());
}

void ArrangeWidget::on_resolutionBox_currentIndexChanged(int index) {
    updateRefreshRateBox();
}

void ArrangeWidget::on_applyButton_clicked() {
    for (SystemScreen* screen : ScreenDaemon::instance()->screens()) {
        screen->set();
    }
//    d->screen->set();
}

void ArrangeWidget::on_primaryDisplaySwitch_toggled(bool checked) {
    if (checked) {
        d->screen->setAsPrimary();
    }
}

void ArrangeWidget::on_orientationBox_currentIndexChanged(int index) {
    d->screen->setRotation(static_cast<SystemScreen::Rotation>(index));
}

void ArrangeWidget::on_enableDisplaySwitch_toggled(bool checked) {
    d->screen->setPowered(checked);
}
