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
#include "audiochunk.h"

#include <statemanager.h>
#include <quietmodemanager.h>
#include <QIcon>
#include "audioquickwidget.h"

struct AudioChunkPrivate {
    AudioQuickWidget* qw = new AudioQuickWidget();
};

AudioChunk::AudioChunk() : IconTextChunk("audio") {
    d = new AudioChunkPrivate();

    connect(StateManager::quietModeManager(), &QuietModeManager::quietModeChanged, this, &AudioChunk::updateData);
    updateData();

    d->qw = new AudioQuickWidget();
    this->setQuickWidget(d->qw);
}

AudioChunk::~AudioChunk() {
    d->qw->deleteLater();
    delete d;
}

void AudioChunk::updateData() {
    this->setIcon(QIcon::fromTheme(QuietModeManager::icon(StateManager::quietModeManager()->currentMode())));
    this->setText(QuietModeManager::name(StateManager::quietModeManager()->currentMode()));
}
