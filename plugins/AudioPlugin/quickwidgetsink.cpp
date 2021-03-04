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

#include <the-libs_global.h>
#include <Context>

struct QuickWidgetSinkPrivate {
    PulseAudioQt::Sink* sink;
    bool changingVolume = false;
};

QuickWidgetSink::QuickWidgetSink(PulseAudioQt::Sink* sink, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QuickWidgetSink) {
    ui->setupUi(this);
    d = new QuickWidgetSinkPrivate();
    d->sink = sink;

    ui->nameLabel->setText(this->fontMetrics().elidedText(sink->description(), Qt::ElideRight, SC_DPI(200)));

    connect(sink, &PulseAudioQt::Sink::volumeChanged, this, &QuickWidgetSink::updateVolume);
    updateVolume();

    this->setFixedWidth(SC_DPI(600));
    ui->nameLabel->setFixedWidth(SC_DPI(200));
}

QuickWidgetSink::~QuickWidgetSink() {
    delete d;
    delete ui;
}

void QuickWidgetSink::on_volumeSlider_sliderMoved(int position) {
    qint64 factor = PulseAudioQt::normalVolume() / 100;
    qint64 newVolume = position * factor;
    d->sink->setVolume(newVolume);
}

void QuickWidgetSink::updateVolume() {
    if (d->changingVolume) return;
    int volume = d->sink->volume() / static_cast<double>(PulseAudioQt::normalVolume()) * 100;
    ui->volumeSlider->setValue(volume);
}

void QuickWidgetSink::on_volumeSlider_sliderPressed() {
    d->changingVolume = true;
}

void QuickWidgetSink::on_volumeSlider_sliderReleased() {
    d->changingVolume = false;
}
