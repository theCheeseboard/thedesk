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
#include "quickwidgetsinkinput.h"
#include "ui_quickwidgetsinkinput.h"

#include <Context>
#include <the-libs_global.h>
#include <QMenu>
#include "common.h"

struct QuickWidgetSinkInputPrivate {
    bool changingVolume = false;
    PulseAudioQt::SinkInput* sinkInput;

    QString pid;
    static QMultiMap<QString, QuickWidgetSinkInput*> sinkInputsByPid;

    QMenu* menu;
    QMenu* devicesMenu;
    QMap<PulseAudioQt::Sink*, QAction*> sinkActions;
    QActionGroup* devicesGroup;
};

QMultiMap<QString, QuickWidgetSinkInput*> QuickWidgetSinkInputPrivate::sinkInputsByPid = QMultiMap<QString, QuickWidgetSinkInput*>();

QuickWidgetSinkInput::QuickWidgetSinkInput(PulseAudioQt::SinkInput* sinkInput, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::QuickWidgetSinkInput) {
    ui->setupUi(this);
    d = new QuickWidgetSinkInputPrivate();
    d->sinkInput = sinkInput;

    connect(sinkInput, &PulseAudioQt::SinkInput::nameChanged, this, &QuickWidgetSinkInput::updateClient);
    connect(sinkInput, &PulseAudioQt::SinkInput::clientChanged, this, &QuickWidgetSinkInput::updateClient);
    connect(sinkInput, &PulseAudioQt::SinkInput::volumeChanged, this, &QuickWidgetSinkInput::updateVolume);
    connect(sinkInput, &PulseAudioQt::SinkInput::propertiesChanged, this, &QuickWidgetSinkInput::updateProperties);
    updateClient();
    updateVolume();
    updateProperties();

    d->devicesGroup = new QActionGroup(this);
    d->devicesGroup->setExclusive(true);

    d->devicesMenu = new QMenu();
    d->devicesMenu->setTitle(tr("Play on"));
    d->devicesMenu->setIcon(QIcon::fromTheme("audio-headphones"));

    d->menu = new QMenu();
    d->menu->addMenu(d->devicesMenu);
    ui->menuButton->setMenu(d->menu);

    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkAdded, this, &QuickWidgetSinkInput::sinkAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sinkRemoved, this, &QuickWidgetSinkInput::sinkRemoved);
    for (PulseAudioQt::Sink* sink : PulseAudioQt::Context::instance()->sinks()) sinkAdded(sink);

    this->setFixedWidth(SC_DPI(600));
    ui->nameLabel->setFixedWidth(SC_DPI(200));
}

QuickWidgetSinkInput::~QuickWidgetSinkInput() {
    d->sinkInputsByPid.remove(d->pid, this);
    for (QuickWidgetSinkInput* sinkInputWidget : d->sinkInputsByPid.values(d->pid)) sinkInputWidget->updateVisibility();
    delete ui;
}

void QuickWidgetSinkInput::updateVolume() {
    if (d->changingVolume) return;
    int volume = d->sinkInput->volume() / static_cast<double>(PulseAudioQt::normalVolume()) * 100;
    QSignalBlocker blocker(ui->volumeSlider);
    ui->volumeSlider->setValue(volume);
}

void QuickWidgetSinkInput::updateClient() {
    QString name = d->sinkInput->properties().value("application.name", d->sinkInput->name()).toString();
//    if (d->sinkInput->client()) {
//        name = d->sinkInput->client()->name();
//    } else {
//        name = d->sinkInput->name();
//    }
    ui->nameLabel->setText(this->fontMetrics().elidedText(name, Qt::ElideRight, SC_DPI(200)));
}

void QuickWidgetSinkInput::updateProperties() {
    d->sinkInputsByPid.remove(d->pid, this);
    for (QuickWidgetSinkInput* sinkInputWidget : d->sinkInputsByPid.values(d->pid)) sinkInputWidget->updateVisibility();

    d->pid = d->sinkInput->properties().value("application.process.id").toString();
    d->sinkInputsByPid.insert(d->pid, this);
    for (QuickWidgetSinkInput* sinkInputWidget : d->sinkInputsByPid.values(d->pid)) sinkInputWidget->updateVisibility();
}

void QuickWidgetSinkInput::updateVisibility() {
    this->setVisible(d->sinkInputsByPid.values(d->pid).first() == this);
}

void QuickWidgetSinkInput::on_volumeSlider_sliderPressed() {
    d->changingVolume = true;
}

void QuickWidgetSinkInput::on_volumeSlider_sliderReleased() {
    d->changingVolume = false;
}

void QuickWidgetSinkInput::on_volumeSlider_valueChanged(int value) {
    qint64 factor = PulseAudioQt::normalVolume() / 100;
    qint64 newVolume = value * factor;

    for (QuickWidgetSinkInput* sinkInputWidget : d->sinkInputsByPid.values(d->pid)) sinkInputWidget->d->sinkInput->setVolume(newVolume);
}

void QuickWidgetSinkInput::sinkAdded(PulseAudioQt::Sink* sink) {
    QAction* action = new QAction(this);
    action->setCheckable(true);

    connect(sink, &PulseAudioQt::Sink::propertiesChanged, this, [ = ] {
        action->setText(Common::nameForSink(sink));
    });
    action->setText(Common::nameForSink(sink));

    connect(d->sinkInput, &PulseAudioQt::SinkInput::deviceIndexChanged, this, [ = ] {
        action->setChecked(sink->index() == d->sinkInput->deviceIndex());
    });
    action->setChecked(sink->index() == d->sinkInput->deviceIndex());

    connect(action, &QAction::toggled, this, [ = ](bool checked) {
        if (checked) {
            for (QuickWidgetSinkInput* sinkInputWidget : d->sinkInputsByPid.values(d->pid)) sinkInputWidget->d->sinkInput->setDeviceIndex(sink->index());
        }
    });

    d->devicesMenu->addAction(action);
    d->devicesGroup->addAction(action);
    d->sinkActions.insert(sink, action);
}

void QuickWidgetSinkInput::sinkRemoved(PulseAudioQt::Sink* sink) {
    QAction* action = d->sinkActions.take(sink);
    d->devicesMenu->removeAction(action);
    d->devicesGroup->removeAction(action);
    action->deleteLater();
}
