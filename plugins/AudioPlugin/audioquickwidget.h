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
#ifndef AUDIOQUICKWIDGET_H
#define AUDIOQUICKWIDGET_H

#include <PulseAudioQt/Sink>
#include <PulseAudioQt/SinkInput>
#include <QWidget>

namespace Ui {
    class AudioQuickWidget;
}

struct AudioQuickWidgetPrivate;
class AudioQuickWidget : public QWidget {
        Q_OBJECT

    public:
        explicit AudioQuickWidget(QWidget* parent = nullptr);
        ~AudioQuickWidget();

        QSize sizeHint() const;

        void sinkAdded(PulseAudioQt::Sink* sink);
        void sinkRemoved(PulseAudioQt::Sink* sink);

        void sinkInputAdded(PulseAudioQt::SinkInput* sinkInput);
        void sinkInputRemoved(PulseAudioQt::SinkInput* sinkInput);

    private:
        Ui::AudioQuickWidget* ui;
        AudioQuickWidgetPrivate* d;

        bool eventFilter(QObject* watched, QEvent* event);

        void updatePrimaryScreen();
        void updateMaxHeight();
};

#endif // AUDIOQUICKWIDGET_H
