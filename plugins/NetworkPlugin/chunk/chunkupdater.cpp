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
#include "chunkupdater.h"

struct ChunkUpdaterPrivate {
    QIcon icon;
    QString text;
};

ChunkUpdater::ChunkUpdater(QObject* parent) : QObject(parent) {
    d = new ChunkUpdaterPrivate();
}

ChunkUpdater::~ChunkUpdater() {
    delete d;
}

QIcon ChunkUpdater::icon() {
    return d->icon;
}

QString ChunkUpdater::text() {
    return d->text;
}

void ChunkUpdater::setText(QString text) {
    d->text = text;
    emit textChanged(text);
}

void ChunkUpdater::setIcon(QIcon icon) {
    d->icon = icon;
    emit iconChanged(icon);
}
