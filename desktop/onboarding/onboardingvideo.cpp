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
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QQmlContext>
#include <tsettings.h>

struct OnboardingVideoPrivate {
    QMediaPlayer* videoPlayer;
    QMediaPlaylist* videoPlaylist;
    QVideoWidget* videoWidget;
};

OnboardingVideo::OnboardingVideo(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::OnboardingVideo) {
    ui->setupUi(this);

    d = new OnboardingVideoPrivate();
    this->setCursor(QCursor(Qt::BlankCursor));

    this->setWindowFlag(Qt::FramelessWindowHint);

    tSettings settings;
    d->videoPlaylist = new QMediaPlaylist(this);
    d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.start").toString()));
    d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.middle").toString()));
    d->videoPlaylist->addMedia(QUrl::fromLocalFile(settings.value("Onboarding/videos.loop").toString()));
    d->videoPlaylist->setCurrentIndex(0);
    connect(d->videoPlaylist, &QMediaPlaylist::currentIndexChanged, this, [ = ](int index) {
        if (index == 1) {
            emit startOnboarding();
            this->setCursor(QCursor(Qt::ArrowCursor));
        } else if (index == 2) {
            //Start looping
            d->videoPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        }
    });

    d->videoPlayer = new QMediaPlayer(this);
    d->videoPlayer->setVideoOutput(d->videoWidget);
    d->videoPlayer->setPlaylist(d->videoPlaylist);
    d->videoPlayer->setVolume(0);
    d->videoPlayer->play();

    QQmlContext* qml = ui->quickWidget->rootContext();
    qml->setContextProperty("sourceVideo", this);
}

OnboardingVideo::~OnboardingVideo() {
    delete d;
    delete ui;
}

QMediaObject* OnboardingVideo::mediaObject() const {
    return d->videoPlayer;
}
