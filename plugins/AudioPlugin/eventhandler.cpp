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
#include <hudmanager.h>
#include <keygrab.h>
#include <quietmodemanager.h>
#include <statemanager.h>

#include <PulseAudioQt/Context>
#include <PulseAudioQt/Server>
#include <QIcon>

struct EventHandlerPrivate {
        KeyGrab* volumeUp;
        KeyGrab* volumeDown;
        KeyGrab* volumeMute;

        PulseAudioQt::Sink* defaultSink = nullptr;
        bool firstSink = true;

        quint32 oldActivePortIndex;
};

EventHandler::EventHandler(QObject* parent) :
    QObject(parent) {
    d = new EventHandlerPrivate();

    connect(PulseAudioQt::Context::instance()->server(), &PulseAudioQt::Server::defaultSinkChanged, this, &EventHandler::defaultSinkChanged);
    defaultSinkChanged(PulseAudioQt::Context::instance()->server()->defaultSink());

    d->volumeUp = new KeyGrab(QKeySequence(Qt::Key_VolumeUp), "volumeUp");
    d->volumeDown = new KeyGrab(QKeySequence(Qt::Key_VolumeDown), "volumeDown");
    d->volumeMute = new KeyGrab(QKeySequence(Qt::Key_VolumeMute), "volumeMute");
    connect(d->volumeUp, &KeyGrab::activated, this, [=] {
        this->adjustVolume(5);
    });
    connect(d->volumeDown, &KeyGrab::activated, this, [=] {
        this->adjustVolume(-5);
    });
    connect(d->volumeMute, &KeyGrab::activated, this, [=] {
        QuietModeManagerTd::QuietMode newMode = StateManager::quietModeManager()->nextQuietMode();
        StateManager::quietModeManager()->setQuietMode(newMode);

        StateManager::instance()->hudManager()->showHud({
            {"icon",  StateManager::quietModeManager()->icon(newMode)       },
            {"title", StateManager::quietModeManager()->name(newMode)       },
            {"text",  StateManager::quietModeManager()->description(newMode)}
        });
    });

    connect(StateManager::quietModeManager(), &QuietModeManagerTd::quietModeChanged, this, &EventHandler::quietModeChanged);
    quietModeChanged();
}

EventHandler::~EventHandler() {
    d->volumeUp->deleteLater();
    d->volumeDown->deleteLater();
    d->volumeMute->deleteLater();
    delete d;
}

void EventHandler::adjustVolume(int percentageChange) {
    if (StateManager::quietModeManager()->currentMode() == QuietModeManagerTd::Mute) {
        showHud(nullptr);
        return;
    }

    // Get the default sink and find the widget for this sink
    PulseAudioQt::Sink* sink = PulseAudioQt::Context::instance()->server()->defaultSink();
    if (!sink) {
        StateManager::instance()->hudManager()->showHud({
            {"icon",  "audio-volume-muted"  },
            {"title", tr("No Audio Devices")}
        });
        return;
    }

    qint64 factor = PulseAudioQt::normalVolume();
    qint64 newVolume = sink->volume() + (factor / 100) * percentageChange;
    if (newVolume < PulseAudioQt::minimumVolume()) newVolume = PulseAudioQt::minimumVolume();

    sink->setVolume(newVolume);
    showHud(sink, newVolume);
}

void EventHandler::defaultSinkChanged(PulseAudioQt::Sink* defaultSink) {
    if (d->defaultSink == defaultSink) return;

    if (d->defaultSink) {
        d->defaultSink->disconnect(this);
    }
    d->defaultSink = defaultSink;
    if (defaultSink) {
        connect(defaultSink, &PulseAudioQt::Sink::activePortIndexChanged, this, [=] {
            if (d->oldActivePortIndex != defaultSink->activePortIndex()) {
                d->oldActivePortIndex = defaultSink->activePortIndex();
                showHud(defaultSink);
            }
        });
        d->oldActivePortIndex = defaultSink->activePortIndex();

        if (d->firstSink) {
            d->firstSink = false;
        } else {
            showHud(defaultSink);
        }
    }
}

void EventHandler::showHud(PulseAudioQt::Sink* sink, qint64 volume) {
    if (StateManager::quietModeManager()->currentMode() == QuietModeManagerTd::Mute) {
        StateManager::instance()->hudManager()->showHud({
            {"icon",  StateManager::quietModeManager()->icon(QuietModeManagerTd::Mute)},
            {"title", tr("Mute")                                                      },
            {"text",  tr("Unmute Quiet Mode before changing the volume")              }
        });
        return;
    }

    double displayVolume;
    if (volume == -1) {
        displayVolume = sink->volume() / static_cast<double>(PulseAudioQt::normalVolume());
    } else {
        displayVolume = volume / static_cast<double>(PulseAudioQt::normalVolume());
    }
    QVariantMap hudData;
    hudData.insert("value", displayVolume);
    switch (Common::portForSink(sink)) {
        case Common::Speakers:
            hudData.insert("icon", "audio-volume-high");
            hudData.insert("title", tr("Speakers"));
            break;
        case Common::Headphones:
            hudData.insert("icon", "audio-headphones");
            hudData.insert("title", tr("Headphones"));
            break;
        case Common::LineOut:
            hudData.insert("icon", "audio-lineout");
            hudData.insert("title", tr("Line Out"));
            break;
        case Common::Bluetooth:
            hudData.insert("icon", "bluetooth");
            hudData.insert("title", Common::nameForSink(sink));
            break;
        case Common::Unknown:
            hudData.insert("icon", "audio-volume-high");
            hudData.insert("title", tr("Volume"));
            break;
    }

    StateManager::instance()->hudManager()->showHud(hudData);
}

void EventHandler::quietModeChanged() {
    QuietModeManagerTd::QuietMode mode = StateManager::quietModeManager()->currentMode();
    QVector<PulseAudioQt::Sink*> sinks = PulseAudioQt::Context::instance()->sinks();
    for (PulseAudioQt::Sink* sink : sinks) {
        sink->setMuted(mode == QuietModeManagerTd::Mute);
    }
}
