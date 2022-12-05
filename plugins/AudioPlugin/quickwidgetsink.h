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
#ifndef QUICKWIDGETSINK_H
#define QUICKWIDGETSINK_H

#include <PulseAudioQt/Sink>
#include <PulseAudioQt/SinkInput>
#include <QWidget>

namespace Ui {
    class QuickWidgetSink;
}

struct QuickWidgetSinkPrivate;
class QuickWidgetSink : public QWidget {
        Q_OBJECT

    public:
        explicit QuickWidgetSink(PulseAudioQt::Sink* sink, QWidget* parent = nullptr);
        ~QuickWidgetSink();

    private slots:
        void on_volumeSlider_sliderPressed();

        void on_volumeSlider_sliderReleased();

        void on_volumeSlider_valueChanged(int value);

        void on_actionMake_Default_triggered();

        void on_actionMove_All_Applications_triggered();

    private:
        Ui::QuickWidgetSink* ui;
        QuickWidgetSinkPrivate* d;

        void sinkInputAdded(PulseAudioQt::SinkInput* sinkInput);
        void updateVisibility();
        void updateName();
        void updateVolume();
        void updateDefault();
};

#endif // QUICKWIDGETSINK_H
