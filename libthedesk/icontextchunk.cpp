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
#include "icontextchunk.h"
#include "ui_icontextchunk.h"

#include "barmanager.h"
#include "statemanager.h"
#include <QIcon>
#include <libcontemporary_global.h>

struct IconTextChunkPrivate {
        QString name;
        QWidget* quickWidget = nullptr;
};

IconTextChunk::IconTextChunk(QString name) :
    Chunk(),
    ui(new Ui::IconTextChunk) {
    ui->setupUi(this);
    d = new IconTextChunkPrivate();
    d->name = name;

    connect(StateManager::barManager(), &BarManager::barHeightTransitioning, this, [=](qreal percentage) {
        int padding = 3 + 6 * percentage;
        this->layout()->setContentsMargins(6, padding, 6, padding);

        if (qFuzzyCompare(percentage, 1)) {
            ui->textLabel->setFixedWidth(QWIDGETSIZE_MAX);
        } else {
            ui->textLabel->setFixedWidth(ui->textLabel->sizeHint().width() * percentage);
        }
    });

    connect(this, &IconTextChunk::clicked, this, [=] {
        showQuickWidget();
    });
}

IconTextChunk::~IconTextChunk() {
    delete d;
    delete ui;
}

void IconTextChunk::setIcon(QIcon icon) {
    ui->iconLabel->setPixmap(icon.pixmap(SC_DPI_T(QSize(16, 16), QSize)));
}

void IconTextChunk::setText(QString text) {
    ui->textLabel->setText(text);
}

void IconTextChunk::setQuickWidget(QWidget* widget) {
    d->quickWidget = widget;
}

void IconTextChunk::deleteLater() {
    Chunk::deleteLater();
}

QString IconTextChunk::name() {
    return d->name;
}

int IconTextChunk::expandedHeight() {
    return qMax(SC_DPI(16), this->fontMetrics().height()) + 18;
}

int IconTextChunk::statusBarHeight() {
    return qMax(SC_DPI(16), this->fontMetrics().height()) + 6;
}

QWidget* IconTextChunk::quickWidget() {
    return d->quickWidget;
}
