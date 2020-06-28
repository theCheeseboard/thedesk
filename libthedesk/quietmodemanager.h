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
#ifndef QUIETMODEMANAGER_H
#define QUIETMODEMANAGER_H

#include <QObject>

struct QuietModeManagerPrivate;
class QuietModeManager : public QObject {
        Q_OBJECT
    public:
        enum QuietMode {
            Sound = 0,
            CriticalOnly,
            NoNotifications,
            Mute,

            LastQuietMode
        };

        explicit QuietModeManager(QObject* parent = nullptr);
        ~QuietModeManager();

        void setQuietMode(QuietMode quietMode);
        QuietMode currentMode();

        QuietMode nextQuietMode() const;

        QString name(QuietMode quietMode) const;
        QString description(QuietMode quietMode) const;
        QString icon(QuietMode quietMode) const;

        QList<QuietMode> availableQuietModes() const;

    signals:
        void quietModeChanged(QuietMode newQuietMode, QuietMode oldQuietMode);

    private:
        QuietModeManagerPrivate* d;
};

#endif // QUIETMODEMANAGER_H
