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
#include "quickwidgetsink.h"
#include "ui_quickwidgetsink.h"

#include "common.h"
#include <PulseAudioQt/Context>
#include <PulseAudioQt/Server>
#include <QMenu>
#include <QTimer>
#include <libcontemporary_global.h>

struct QuickWidgetSinkPrivate {
        PulseAudioQt::Sink* sink;
        bool changingVolume = false;

        QMenu* menu;
};

QuickWidgetSink::QuickWidgetSink(PulseAudioQt::Sink* sink, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QuickWidgetSink) {
    ui->setupUi(this);
    d = new QuickWidgetSinkPrivate();
    d->sink = sink;

    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSinkChanged, this, &QuickWidgetSink::updateDefault, Qt::QueuedConnection);
    updateDefault();

    connect(sink, &PulseAudioQt::Sink::volumeChanged, this, &QuickWidgetSink::updateVolume);
    connect(sink, &PulseAudioQt::Sink::propertiesChanged, this, &QuickWidgetSink::updateName);
    updateVolume();
    updateName();

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkInputAdded, this, &QuickWidgetSink::sinkInputAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkInputRemoved, this, &QuickWidgetSink::updateVisibility);
    for (PulseAudioQt::SinkInput* sinkInput : PulseAudioQt::Context::instance()->sinkInputs()) sinkInputAdded(sinkInput);
    updateVisibility();

    this->setFixedWidth(SC_DPI(600));
    ui->nameLabel->setFixedWidth(SC_DPI(200));

    d->menu = new QMenu();
    d->menu->addAction(ui->actionMake_Default);
    d->menu->addAction(ui->actionMove_All_Applications);
    ui->menuButton->setMenu(d->menu);
}

QuickWidgetSink::~QuickWidgetSink() {
    delete d;
    delete ui;
}

void QuickWidgetSink::updateVolume() {
    if (d->changingVolume) return;
    int volume = d->sink->volume() / static_cast<double>(PulseAudioQt::normalVolume()) * 100;
    QSignalBlocker blocker(ui->volumeSlider);
    ui->volumeSlider->setValue(volume);
}

void QuickWidgetSink::updateDefault() {
    ui->actionMake_Default->setVisible(PulseAudioQt::Context::instance()->server()->defaultSink() != d->sink);
    updateVisibility();
}

void QuickWidgetSink::on_volumeSlider_sliderPressed() {
    d->changingVolume = true;
}

void QuickWidgetSink::on_volumeSlider_sliderReleased() {
    d->changingVolume = false;
}

void QuickWidgetSink::sinkInputAdded(PulseAudioQt::SinkInput* sinkInput) {
    connect(sinkInput, &PulseAudioQt::SinkInput::deviceIndexChanged, this, &QuickWidgetSink::updateVisibility);
    updateVisibility();
}

void QuickWidgetSink::updateVisibility() {
    QTimer::singleShot(0, this, [this] {
        if (PulseAudioQt::Context::instance()->server()->defaultSink() == d->sink) {
            this->setVisible(true);
            return;
        }

        for (PulseAudioQt::SinkInput* sinkInput : PulseAudioQt::Context::instance()->sinkInputs()) {
            if (sinkInput->deviceIndex() == d->sink->index()) {
                this->setVisible(true);
                return;
            }
        }

        this->setVisible(false);
    });
}

void QuickWidgetSink::updateName() {
    ui->nameLabel->setText(this->fontMetrics().elidedText(Common::nameForSink(d->sink), Qt::ElideRight, SC_DPI(200)));
}

void QuickWidgetSink::on_volumeSlider_valueChanged(int value) {
    qint64 factor = PulseAudioQt::normalVolume() / 100;
    qint64 newVolume = value * factor;
    d->sink->setVolume(newVolume);
}

void QuickWidgetSink::on_actionMake_Default_triggered() {
    PulseAudioQt::Context::instance()->server()->setDefaultSink(d->sink);
}

void QuickWidgetSink::on_actionMove_All_Applications_triggered() {
    for (PulseAudioQt::SinkInput* sinkInput : PulseAudioQt::Context::instance()->sinkInputs()) {
        sinkInput->setDeviceIndex(d->sink->index());
    }
}
