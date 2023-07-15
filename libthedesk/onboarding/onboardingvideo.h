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
#ifndef ONBOARDINGVIDEO_H
#define ONBOARDINGVIDEO_H

#include <QDialog>
#include <QMediaPlayer>

namespace Ui {
    class OnboardingVideo;
}

struct OnboardingVideoPrivate;
class OnboardingVideo : public QDialog {
        Q_OBJECT
        Q_PROPERTY(QMediaPlayer* mediaObject READ mediaObject NOTIFY mediaObjectChanged)

    public:
        explicit OnboardingVideo(QWidget* parent = nullptr);
        ~OnboardingVideo();

        QMediaPlayer* mediaObject() const;

    signals:
        void startOnboarding();
        void mediaObjectChanged();
        void playAudio();

    private:
        Ui::OnboardingVideo* ui;
        OnboardingVideoPrivate* d;

        QMediaPlayer* mediaPlayerForFile(QString file);
};

#endif // ONBOARDINGVIDEO_H
