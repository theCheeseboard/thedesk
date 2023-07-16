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
#include "onboardingvideo.h"
#include "ui_onboardingvideo.h"

#include <QMediaPlayer>
#include <QPainter>
#include <QQmlContext>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>
#include <tsettings.h>

struct OnboardingVideoPrivate {
        QMediaPlayer* videoPlayer;
        QPixmap pixmap;
};

OnboardingVideo::OnboardingVideo(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::OnboardingVideo) {
    ui->setupUi(this);

    d = new OnboardingVideoPrivate();
    this->setCursor(QCursor(Qt::BlankCursor));

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowStaysOnBottomHint);

    ui->videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);

    tSettings settings;
    auto startPlayer = mediaPlayerForFile(settings.value("Onboarding/videos.start").toString());
    auto middlePlayer = mediaPlayerForFile(settings.value("Onboarding/videos.middle").toString());
    auto loopPlayer = mediaPlayerForFile(settings.value("Onboarding/videos.loop").toString());

    startPlayer->setVideoOutput(ui->videoWidget);
    connect(startPlayer, &QMediaPlayer::mediaStatusChanged, this, [this, middlePlayer](QMediaPlayer::MediaStatus state) {
        if (state == QMediaPlayer::BufferedMedia || state == QMediaPlayer::BufferingMedia) {
            emit playAudio();
        } else if (state == QMediaPlayer::EndOfMedia) {
            middlePlayer->play();
            middlePlayer->setVideoOutput(ui->videoWidget);
            QTimer::singleShot(0, this, &OnboardingVideo::startOnboarding);
        }
    });
    connect(middlePlayer, &QMediaPlayer::mediaStatusChanged, this, [this, loopPlayer](QMediaPlayer::MediaStatus state) {
        if (state == QMediaPlayer::EndOfMedia) {
            loopPlayer->play();
            loopPlayer->setVideoOutput(ui->videoWidget);
        }
    });
    loopPlayer->setLoops(QMediaPlayer::Infinite);

    QVideoFrame::PaintOptions framePaintOptions;
    framePaintOptions.aspectRatioMode = Qt::KeepAspectRatioByExpanding;
    framePaintOptions.backgroundColor = Qt::black;

    startPlayer->play();
}

OnboardingVideo::~OnboardingVideo() {
    delete d;
    delete ui;
}

QMediaPlayer* OnboardingVideo::mediaObject() const {
    return d->videoPlayer;
}

QMediaPlayer* OnboardingVideo::mediaPlayerForFile(QString file) {
    auto player = new QMediaPlayer(this);
    player->setSource(QUrl::fromLocalFile(file));
    return player;
}
