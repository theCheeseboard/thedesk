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
#include "iconloaderengine.h"

#include <libcontemporary_global.h>

struct IconLoaderEnginePrivate {
        QIconEngine* parentEngine;
        QIconEngine* rtlParentEngine;
};

IconLoaderEngine::IconLoaderEngine(QIconEngine* parentEngine, QIconEngine* rtlParentEngine) {
    d = new IconLoaderEnginePrivate();
    d->parentEngine = parentEngine;
    d->rtlParentEngine = rtlParentEngine;
}

IconLoaderEngine::~IconLoaderEngine() {
    delete d->parentEngine;
    delete d;
}

QPixmap IconLoaderEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QImage image;
    if (QApplication::layoutDirection() == Qt::RightToLeft && d->rtlParentEngine->availableSizes(mode, state).length() > 0) {
        image = d->rtlParentEngine->pixmap(size, mode, state).toImage();
    } else {
        image = d->parentEngine->pixmap(size, mode, state).toImage();
    }
    libContemporaryCommon::tintImage(image, QApplication::palette().color(QPalette::WindowText));
    return QPixmap::fromImage(image);
}

void IconLoaderEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) {
    if (QApplication::layoutDirection() == Qt::RightToLeft && d->rtlParentEngine->availableSizes(mode, state).length() > 0) {
        d->rtlParentEngine->paint(painter, rect, mode, state);
    } else {
        d->parentEngine->paint(painter, rect, mode, state);
    }
}

QSize IconLoaderEngine::actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    if (QApplication::layoutDirection() == Qt::RightToLeft) {
        return d->rtlParentEngine->actualSize(size, mode, state);
    } else {
        return d->parentEngine->actualSize(size, mode, state);
    }
}

void IconLoaderEngine::addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state) {
    d->parentEngine->addPixmap(pixmap, mode, state);
    d->rtlParentEngine->addPixmap(pixmap, mode, state);
}

void IconLoaderEngine::addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) {
    d->parentEngine->addFile(fileName, size, mode, state);
    d->rtlParentEngine->addFile(fileName, size, mode, state);
}

QString IconLoaderEngine::key() const {
    return d->parentEngine->key();
}

QIconEngine* IconLoaderEngine::clone() const {
    return new IconLoaderEngine(d->parentEngine->clone(), d->rtlParentEngine->clone());
}

bool IconLoaderEngine::read(QDataStream& in) {
    return d->parentEngine->read(in);
}

bool IconLoaderEngine::write(QDataStream& out) const {
    return d->parentEngine->write(out);
}

QList<QSize> IconLoaderEngine::availableSizes(QIcon::Mode mode, QIcon::State state) const {
    if (QApplication::layoutDirection() == Qt::RightToLeft) {
        return d->rtlParentEngine->availableSizes(mode, state);
    } else {
        return d->parentEngine->availableSizes(mode, state);
    }
}

QString IconLoaderEngine::iconName() const {
    return d->parentEngine->iconName();
}

void IconLoaderEngine::virtual_hook(int id, void* data) {
    d->parentEngine->virtual_hook(id, data);
}
