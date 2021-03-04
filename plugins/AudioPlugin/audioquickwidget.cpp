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

#include <Context>
#include "quickwidgetsink.h"

struct AudioQuickWidgetPrivate {
    QMap<PulseAudioQt::Sink*, QuickWidgetSink*> sinkWidgets;
};

AudioQuickWidget::AudioQuickWidget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AudioQuickWidget) {
    ui->setupUi(this);
    d = new AudioQuickWidgetPrivate();

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkAdded, this, &AudioQuickWidget::sinkAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkRemoved, this, &AudioQuickWidget::sinkRemoved);

    for (PulseAudioQt::Sink* sink : PulseAudioQt::Context::instance()->sinks()) sinkAdded(sink);


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
}

AudioQuickWidget::~AudioQuickWidget() {
    delete ui;
    delete d;
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
