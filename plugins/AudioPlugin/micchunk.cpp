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
#include "micchunk.h"

#include <PulseAudioQt/Context>
#include <PulseAudioQt/SourceOutput>
#include <QIcon>
#include <barmanager.h>
#include <libcontemporary_global.h>
#include <statemanager.h>

MicChunk::MicChunk() :
    IconTextChunk("audio-mic") {
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sourceOutputAdded, this, &MicChunk::sourceOutputAdded);
    connect(PulseAudioQt::Context::instance(), &PulseAudioQt::Context::sourceOutputRemoved, this, &MicChunk::updateSourceOutputs);
    for (PulseAudioQt::SourceOutput* output : PulseAudioQt::Context::instance()->sourceOutputs()) sourceOutputAdded(output);
    updateSourceOutputs();
}

void MicChunk::sourceOutputAdded(PulseAudioQt::SourceOutput* sourceOutput) {
    connect(sourceOutput, &PulseAudioQt::SourceOutput::clientChanged, this, &MicChunk::updateSourceOutputs);
    connect(sourceOutput, &PulseAudioQt::SourceOutput::mutedChanged, this, &MicChunk::updateSourceOutputs);
    connect(sourceOutput, &PulseAudioQt::SourceOutput::propertiesChanged, this, &MicChunk::updateSourceOutputs);
    updateSourceOutputs();
}

void MicChunk::updateSourceOutputs() {
    bool micMuted = true;
    QStringList micClients;
    for (PulseAudioQt::SourceOutput* output : PulseAudioQt::Context::instance()->sourceOutputs()) {
        if (!output->isMuted()) micMuted = false;

        micClients.append(output->properties().value("application.name").toString());
        //        if (output->client()) {
        //            micClients.append(output->client()->name());
        //        } else {
        //            micClients.append(this->fontMetrics().elidedText(output->name(), Qt::ElideRight, SC_DPI(100)));
        //        }
    }

    micClients.removeDuplicates();

    if (micClients.isEmpty()) {
        if (StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->removeChunk(this);
    } else {
        this->setIcon(QIcon::fromTheme(micMuted ? "mic-off" : "mic-on"));
        if (micClients.count() == 1) {
            this->setText(micClients.first());
        } else {
            this->setText(tr("%n applications", nullptr, micClients.count()));
        }
        if (!StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->addChunk(this);
    }
}
