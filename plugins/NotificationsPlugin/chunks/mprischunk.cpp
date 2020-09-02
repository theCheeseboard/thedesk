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
#include "mprischunk.h"
#include "ui_mprischunk.h"

#include <statemanager.h>
#include <barmanager.h>
#include <mpris/mprisengine.h>
#include <mpris/mprisplayer.h>
#include <Applications/application.h>
#include <QMenu>
#include <QActionGroup>
#include <the-libs_global.h>

struct MprisChunkPrivate {
    MprisPlayerPtr currentPlayer;
    QString currentPlayerService;
    ApplicationPointer currentPlayerApplication;

    QMenu* playersMenu;
    QActionGroup* playersGroup;
    QMap<QString, QAction*> playerActions;
};

MprisChunk::MprisChunk() :
    Chunk(),
    ui(new Ui::MprisChunk) {
    ui->setupUi(this);

    d = new MprisChunkPrivate();
    d->playersGroup = new QActionGroup(this);

    d->playersMenu = new QMenu(this);
    d->playersMenu->addSection(tr("Media Players"));
    ui->playersButton->setMenu(d->playersMenu);
    ui->playersButton->setVisible(MprisEngine::players().count() > 1);

    connect(MprisEngine::instance(), &MprisEngine::newPlayer, this, [ = ](QString service, MprisPlayerPtr player) {
        Q_UNUSED(player)
        setupPlayer(service);

        if (d->currentPlayer == nullptr) setCurrentPlayer(service);
    });
    connect(MprisEngine::instance(), &MprisEngine::playerGone, this, [ = ](QString service) {
        QAction* action = d->playerActions.take(service);
        d->playersMenu->removeAction(action);
        action->deleteLater();
        ui->playersButton->setVisible(MprisEngine::players().count() > 1);

        if (d->currentPlayerService == service) {
            //Find another player
            if (!MprisEngine::players().isEmpty()) {
                setCurrentPlayer(MprisEngine::players().first()->service());
            } else {
                setCurrentPlayer("");
            }
        }
    });

    if (!MprisEngine::players().isEmpty()) {
        for (MprisPlayerPtr player : MprisEngine::players()) {
            setupPlayer(player->service());
        }
        setCurrentPlayer(MprisEngine::players().first()->service());
    } else {
        setCurrentPlayer("");
    }

    connect(StateManager::barManager(), &BarManager::barHeightTransitioning, this, [ = ](qreal percentage) {
        if (qFuzzyCompare(percentage, 1)) {
            ui->buttonWidget->setFixedWidth(QWIDGETSIZE_MAX);
            ui->playersButton->setFixedWidth(QWIDGETSIZE_MAX);
            ui->stateIcon->setFixedWidth(0);
        } else if (qFuzzyIsNull(percentage)) {
            ui->buttonWidget->setFixedWidth(0);
            ui->playersButton->setFixedWidth(0);
            ui->stateIcon->setFixedWidth(QWIDGETSIZE_MAX);
        } else {
            ui->buttonWidget->setFixedWidth(static_cast<int>(ui->buttonWidget->sizeHint().width() * percentage));
            ui->playersButton->setFixedWidth(static_cast<int>(ui->playersButton->sizeHint().width() * percentage));
            ui->stateIcon->setFixedWidth(static_cast<int>(ui->stateIcon->sizeHint().width() * (1 - percentage)));
        }
    });

    ui->stateIcon->setFixedWidth(0);
    ui->stateIcon->setPixmap(QIcon::fromTheme("media-playback-start").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
}

MprisChunk::~MprisChunk() {
    if (StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->removeChunk(this);
    delete d;
    delete ui;
}

void MprisChunk::setupPlayer(QString service) {
    MprisPlayerPtr player = MprisEngine::instance()->playerForInterface(service);
    QAction* action = new QAction();
    action->setText(player->identity());
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, [ = ] {
        setCurrentPlayer(service);
    });
    d->playerActions.insert(service, action);
    d->playersGroup->addAction(action);
    d->playersMenu->addAction(action);
    ui->playersButton->setVisible(MprisEngine::players().count() > 1);
}

void MprisChunk::setCurrentPlayer(QString player) {
    d->currentPlayer->disconnect(this);
    d->currentPlayerService = player;
    if (player == "") {
        d->currentPlayer = nullptr;
        d->currentPlayerApplication = nullptr;
        if (StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->removeChunk(this);
    } else {
        d->currentPlayer = MprisEngine::playerForInterface(player);

        connect(d->currentPlayer.data(), &MprisPlayerInterface::desktopEntryChanged, this, &MprisChunk::updateApplication);
        connect(d->currentPlayer.data(), &MprisPlayerInterface::metadataChanged, this, &MprisChunk::updateMetadata);
        connect(d->currentPlayer.data(), &MprisPlayerInterface::playbackStatusChanged, this, &MprisChunk::updateState);

        updateApplication();
        updateMetadata();
        updateState();

        if (!StateManager::barManager()->isChunkRegistered(this)) StateManager::barManager()->addChunk(this);
        d->playerActions.value(player)->setChecked(true);
    }
}

void MprisChunk::updateMetadata() {
    MetadataMap meta = d->currentPlayer->metadata();
    QStringList parts;

    if (meta.contains("xesam:title")) parts.append(meta.value("xesam:title").toString());
    if (meta.contains("xesam:artist")) parts.append(QLocale().createSeparatedList(meta.value("xesam:artist").toStringList()));

    if (parts.isEmpty()) {
        if (d->currentPlayerApplication) {
            parts.append(d->currentPlayerApplication->getProperty("Name").toString());
        } else {
            parts.append(d->currentPlayer->identity());
        }
    }
    ui->metadataLabel->setText(parts.join(" · "));
}

void MprisChunk::updateApplication() {
    if (d->currentPlayer) {
        d->currentPlayerApplication = ApplicationPointer(new Application(d->currentPlayer->desktopEntry()));
    } else {
        d->currentPlayerApplication = nullptr;
    }
}

void MprisChunk::updateState() {
    switch (d->currentPlayer->playbackStatus()) {
        case MprisPlayerInterface::Playing:
            ui->stateIcon->setPixmap(QIcon::fromTheme("media-playback-start").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            ui->playPauseButton->setIcon(QIcon::fromTheme("media-playback-pause"));
            break;
        case MprisPlayerInterface::Paused:
            ui->stateIcon->setPixmap(QIcon::fromTheme("media-playback-pause").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            ui->playPauseButton->setIcon(QIcon::fromTheme("media-playback-start"));
            break;
        case MprisPlayerInterface::Stopped:
            ui->stateIcon->setPixmap(QIcon::fromTheme("media-playback-stop").pixmap(SC_DPI_T(QSize(16, 16), QSize)));
            ui->playPauseButton->setIcon(QIcon::fromTheme("media-playback-start"));
            break;
    }
}

QString MprisChunk::name() {
    return "mpris";
}

int MprisChunk::expandedHeight() {
    return this->sizeHint().height();
}

int MprisChunk::statusBarHeight() {
    return ui->metadataLabel->sizeHint().height();
}

void MprisChunk::on_playPauseButton_clicked() {
    if (d->currentPlayer) d->currentPlayer->playPause();
}

void MprisChunk::on_backButton_clicked() {
    if (d->currentPlayer) d->currentPlayer->previous();
}

void MprisChunk::on_forwardButton_clicked() {
    if (d->currentPlayer) d->currentPlayer->next();
}
