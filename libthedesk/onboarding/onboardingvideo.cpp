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
// #include <QMediaPlaylist>
#include <QQmlContext>
// #include <QVideoWidget>
#include <QPainter>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>
#include <tsettings.h>

struct OnboardingVideoPrivate {
        QMediaPlayer* videoPlayer;
        //        QMediaPlaylist* videoPlaylist;
        //        QVideoWidget* videoWidget;
        QPixmap pixmap;
};

OnboardingVideo::OnboardingVideo(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::OnboardingVideo) {
    ui->setupUi(this);

    d = new OnboardingVideoPrivate();
    this->setCursor(QCursor(Qt::BlankCursor));

    this->setWindowFlag(Qt::FramelessWindowHint);

    ui->videoWidget->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);

    tSettings settings;
    auto startPlayer = mediaPlayerForFile(settings.value("Onboarding/videos.start").toString());
    auto middlePlayer = mediaPlayerForFile(settings.value("Onboarding/videos.middle").toString());
    auto loopPlayer = mediaPlayerForFile(settings.value("Onboarding/videos.loop").toString());

    //        d->videoPlaylist = new QMediaPlaylist(this);
    //        d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.start").toString()));
    //        d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.middle").toString()));
    //        d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.loop").toString()));
    //        d->videoPlaylist->setCurrentIndex(0);
    //        connect(d->videoPlaylist, &QMediaPlaylist::currentIndexChanged, this, [=](int index) {
    //            if (index == 1) {
    //                emit startOnboarding();
    //                this->setCursor(QCursor(Qt::ArrowCursor));
    //            } else if (index == 2) {
    ////                 Start looping
    //                d->videoPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    //            }
    //        });

    //        d->videoPlayer = new QMediaPlayer(this);
    //        d->videoPlayer->setPlaylist(d->videoPlaylist);
    //        d->videoPlayer->setVolume(0);
    //        connect(d->videoPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus state) {
    //            if (state == QMediaPlayer::BufferedMedia || state == QMediaPlayer::BufferingMedia) {
    //                emit playAudio();
    //            }
    //        });
    //        d->videoPlayer->play();

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
