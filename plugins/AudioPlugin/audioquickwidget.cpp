/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "audioquickwidget.h"
#include "ui_audioquickwidget.h"

#include <statemanager.h>
#include <quietmodemanager.h>
#include <QPushButton>
#include <QScreen>

#include <Context>
#include "quickwidgetsink.h"
#include "quickwidgetsinkinput.h"

struct AudioQuickWidgetPrivate {
    QScreen* primaryScreen = nullptr;

    QMap<PulseAudioQt::Sink*, QuickWidgetSink*> sinkWidgets;
    QMap<PulseAudioQt::SinkInput*, QuickWidgetSinkInput*> sinkInputWidgets;
};

AudioQuickWidget::AudioQuickWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AudioQuickWidget) {
    ui->setupUi(this);
    d = new AudioQuickWidgetPrivate();

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkAdded, this, &AudioQuickWidget::sinkAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkRemoved, this, &AudioQuickWidget::sinkRemoved);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkInputAdded, this, &AudioQuickWidget::sinkInputAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkInputRemoved, this, &AudioQuickWidget::sinkInputRemoved);

    for (PulseAudioQt::Sink* sink : PulseAudioQt::Context::instance()->sinks()) sinkAdded(sink);
    for (PulseAudioQt::SinkInput* sinkInput : PulseAudioQt::Context::instance()->sinkInputs()) sinkInputAdded(sinkInput);
    ui->sinkInputsWidget->setVisible(false);

    for (QuietModeManager::QuietMode mode : StateManager::quietModeManager()->availableQuietModes()) {
        QuietModeManager::QuietMode m = mode;
        QPushButton* button = new QPushButton(this);
        button->setText(StateManager::quietModeManager()->name(m));
        button->setIcon(QIcon::fromTheme(StateManager::quietModeManager()->icon(m)));
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setChecked(StateManager::quietModeManager()->currentMode() == m);
        connect(button, &QPushButton::toggled, this, [ = ](bool checked) {
            if (checked) {
                StateManager::quietModeManager()->setQuietMode(m);
            }
        });
        connect(StateManager::quietModeManager(), &QuietModeManager::quietModeChanged, this, [ = ](QuietModeManager::QuietMode newMode, QuietModeManager::QuietMode oldMode) {
            Q_UNUSED(oldMode);
            button->setChecked(newMode == m);
        });
        ui->quietModesLayout->addWidget(button);
    }

    connect(qApp, &QApplication::primaryScreenChanged, this, &AudioQuickWidget::updatePrimaryScreen);
    updatePrimaryScreen();
}

AudioQuickWidget::~AudioQuickWidget() {
    delete ui;
    delete d;
}

QSize AudioQuickWidget::sizeHint() const {
    QSize sizeHint = ui->scrollAreaWidgetContents->sizeHint();
    if (sizeHint.height() > this->maximumHeight()) sizeHint.setHeight(this->maximumHeight());
    return sizeHint;
}

void AudioQuickWidget::sinkAdded(PulseAudioQt::Sink* sink) {
    QuickWidgetSink* sinkWidget = new QuickWidgetSink(sink);
    ui->sinksLayout->addWidget(sinkWidget);
    d->sinkWidgets.insert(sink, sinkWidget);
}

void AudioQuickWidget::sinkRemoved(PulseAudioQt::Sink* sink) {
    QuickWidgetSink* sinkWidget = d->sinkWidgets.take(sink);
    ui->sinksLayout->removeWidget(sinkWidget);
    sinkWidget->setVisible(false);
    sinkWidget->deleteLater();
}

void AudioQuickWidget::sinkInputAdded(PulseAudioQt::SinkInput* sinkInput) {
    QuickWidgetSinkInput* sinkInputWidget = new QuickWidgetSinkInput(sinkInput);
    ui->sinkInputsLayout->addWidget(sinkInputWidget);
    d->sinkInputWidgets.insert(sinkInput, sinkInputWidget);
    ui->sinkInputsWidget->setVisible(true);
}

void AudioQuickWidget::sinkInputRemoved(PulseAudioQt::SinkInput* sinkInput) {
    QuickWidgetSinkInput* sinkInputWidget = d->sinkInputWidgets.take(sinkInput);
    ui->sinksLayout->removeWidget(sinkInputWidget);
    sinkInputWidget->setVisible(false);
    sinkInputWidget->deleteLater();

    if (d->sinkInputWidgets.isEmpty()) ui->sinkInputsWidget->setVisible(false);
}

void AudioQuickWidget::updatePrimaryScreen() {
    if (d->primaryScreen) d->primaryScreen->disconnect(this);
    d->primaryScreen = qApp->primaryScreen();
    connect(d->primaryScreen, &QScreen::geometryChanged, this, &AudioQuickWidget::updateMaxHeight);
    updateMaxHeight();
}

void AudioQuickWidget::updateMaxHeight() {
    this->setMaximumHeight(d->primaryScreen->geometry().height() * 0.7);
}
