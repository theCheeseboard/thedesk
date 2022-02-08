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
#ifndef ICONLOADERENGINE_H
#define ICONLOADERENGINE_H

#include <QIconEngine>

struct IconLoaderEnginePrivate;
class IconLoaderEngine : public QIconEngine {
    public:
        explicit IconLoaderEngine(QIconEngine* parentEngine, QIconEngine* rtlParentEngine);
        ~IconLoaderEngine();

    private:
        IconLoaderEnginePrivate* d;

        // QIconEngine interface
    public:
        QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state);
        void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state);
        QSize actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state);
        void addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state);
        void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state);
        QString key() const;
        QIconEngine* clone() const;
        bool read(QDataStream& in);
        bool write(QDataStream& out) const;
        QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) const;
        QString iconName() const;
        void virtual_hook(int id, void* data);
};

#endif // ICONLOADERENGINE_H
