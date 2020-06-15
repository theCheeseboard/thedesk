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
#include "eventhandler.h"

#include "common.h"
#include <QKeySequence>
#include <statemanager.h>
#include <hudmanager.h>
#include <keygrab.h>

#include <Context>
#include <Server>
#include <QIcon>

struct EventHandlerPrivate {
    KeyGrab* volumeUp;
    KeyGrab* volumeDown;
    KeyGrab* volumeMute;

    PulseAudioQt::Sink* defaultSink = nullptr;
};

EventHandler::EventHandler(QObject* parent) : QObject(parent) {
    d = new EventHandlerPrivate();

    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSinkChanged, this, &EventHandler::defaultSinkChanged);

    d->volumeUp = new KeyGrab(QKeySequence(Qt::Key_VolumeUp), "volumeUp");
    d->volumeDown = new KeyGrab(QKeySequence(Qt::Key_VolumeDown), "volumeDown");
    d->volumeMute = new KeyGrab(QKeySequence(Qt::Key_VolumeMute), "volumeMute");
    connect(d->volumeUp, &KeyGrab::activated, this, [ = ] {
        this->adjustVolume(5);
    });
    connect(d->volumeDown, &KeyGrab::activated, this, [ = ] {
        this->adjustVolume(-5);
    });
    connect(d->volumeMute, &KeyGrab::activated, this, [ = ] {

    });
}

EventHandler::~EventHandler() {
    d->volumeUp->deleteLater();
    d->volumeDown->deleteLater();
    d->volumeMute->deleteLater();
    delete d;
}

void EventHandler::adjustVolume(int percentageChange) {
    //Get the default sink and find the widget for this sink
    PulseAudioQt::Sink* sink = PulseAudioQt::Context::instance()->server()->defaultSink();
    if (!sink) {
        StateManager::instance()->hudManager()->showHud({
            {"icon", "audio-volume-muted"},
            {"title", tr("No Audio Devices")}
        });
        return;
    }

    qint64 factor = PulseAudioQt::normalVolume();
    qint64 newVolume = sink->volume() + (factor / 100) * percentageChange;

    sink->setVolume(newVolume);
    showHud(sink);
}

void EventHandler::defaultSinkChanged(PulseAudioQt::Sink* defaultSink) {
    if (d->defaultSink) {
        d->defaultSink->disconnect(this);
    }
    d->defaultSink = defaultSink;
    if (defaultSink) {
        connect(defaultSink, &PulseAudioQt::Sink::activePortIndexChanged, this, [ = ] {
            showHud(defaultSink);
        });
        showHud(defaultSink);
    }
}

void EventHandler::showHud(PulseAudioQt::Sink* sink) {
    QVariantMap hudData;
    hudData.insert("value", sink->volume() / static_cast<double>(PulseAudioQt::normalVolume()));
    switch (Common::portForSink(sink)) {
        case Common::Speakers:
            hudData.insert("icon", "audio-volume-high");
            hudData.insert("title", tr("Speakers"));
            break;
        case Common::Headphones:
            hudData.insert("icon", "audio-headphones");
            hudData.insert("title", tr("Headphones"));
            break;
        case Common::Unknown:
            hudData.insert("icon", "audio-volume-high");
            hudData.insert("title", tr("Volume"));
            break;
    }

    StateManager::instance()->hudManager()->showHud(hudData);
}
