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
#include "onboarding.h"
#include "ui_onboarding.h"

#include "onboardingbar.h"
#include "onboardingstepper.h"
#include <QAudioDecoder>
#include <QAudioOutput>
#include <QKeyEvent>
#include <QPainter>
#include <common.h>
#include <onboardingmanager.h>
#include <onboardingpage.h>
#include <powermanager.h>
#include <statemanager.h>
#include <tscrim.h>
#include <tsettings.h>
#include <tvariantanimation.h>

#include <QAudioSink>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QShortcut>

#include <private/onboardingmanager_p.h>

struct OnboardingPrivate {
        QStringList preferredOnboardingOrder;
        QList<QPair<QString, OnboardingPage*>> steps;
        QList<QPair<OnboardingStepper*, OnboardingPage*>> steppers;

        QByteArray singleAudioData;
        QByteArray loopAudioData;
        QAudioSink* audioOutput = nullptr;
        QIODevice* audioOutputDevice;
        int audioPointer = 0;
        bool audioEnabled = true;

        OnboardingBar* bar;
        bool barVisible = false;

        tVariantAnimation* contentAnim;
        tVariantAnimation* barAnim;
        tVariantAnimation* fadeAnim;

        bool onboardingStarted = false;
        tSettings settings;
};

Onboarding::Onboarding(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Onboarding) {
    ui->setupUi(this);

    Q_INIT_RESOURCE(libthedesk_resources);

    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowFlag(Qt::FramelessWindowHint);

    this->setCursor(QCursor(Qt::BlankCursor));

    d = new OnboardingPrivate();
    d->preferredOnboardingOrder = d->settings.delimitedList("Onboarding/order");

    ui->stepperFrame->setFixedSize(SC_DPI_T(QSize(200, 600), QSize));
    ui->stackedWidget->setFixedSize(SC_DPI_T(QSize(800, 600), QSize));
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->contentWrapper->setFixedHeight(0);
    ui->contentWrapper->setVisible(false);

    OnboardingManager* manager = StateManager::onboardingManager();
    manager->d->onboardingRunning = true;
    connect(manager, &OnboardingManager::nextStep, this, [=] {
        if (ui->stackedWidget->count() == ui->stackedWidget->currentIndex() + 1) {
            completeOnboarding();
        } else {
            ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
        }
    });
    connect(manager, &OnboardingManager::previousStep, this, [=] {
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
    });

    // Populate the stacked widget with all the onboarding items
    QStringList currentItems;
    for (OnboardingPage* step : qAsConst(manager->d->steps)) {
        OnboardingStepper* stepper = new OnboardingStepper(this);
        stepper->setText(step->displayName());
        connect(ui->stackedWidget, &tStackedWidget::switchingFrame, this, [=](int step) {
            stepper->setCurrentStep(step + 1);
        });

        int index = Common::getInsertionIndex(d->preferredOnboardingOrder, currentItems, step->name());

        // Make sure this pane is actually configured to be in the onboarding
        if (index != -1) {
            // Add this pane at the correct index
            ui->stackedWidget->insertWidget(index, step);
            currentItems.insert(index, step->name());
            d->steps.insert(index, {step->name(), step});

            ui->stepperLayout->insertWidget(index, stepper);
            d->steppers.insert(index, {stepper, step});
        } else {
            // Hide the stepper so it doesn't show up in the background
            stepper->setVisible(false);
        }
    }

    for (int i = 0; i < d->steppers.count(); i++) {
        d->steppers.at(i).first->setStep(i + 1);
        if (i == d->steppers.count() - 1) d->steppers.at(i).first->setIsFinal(true);
    }

    d->bar = new OnboardingBar(this);
    d->bar->setParent(this);
    connect(d->bar, &OnboardingBar::muteToggled, this, [=](bool mute) {
        if (d->audioOutput) d->audioOutput->setVolume(mute ? 0 : 100);
    });
    connect(d->bar, &OnboardingBar::closeClicked, this, [=] {
        StateManager::instance()->powerManager()->showPowerOffConfirmation();
    });
    d->bar->setVisible(false);
    d->bar->installEventFilter(this);

    d->contentAnim = new tVariantAnimation(this);
    d->contentAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->contentAnim->setDuration(500);
    connect(d->contentAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        ui->contentWrapper->setFixedHeight(value.toInt());
    });

    d->barAnim = new tVariantAnimation(this);
    d->barAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->barAnim->setDuration(500);
    connect(d->barAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        d->bar->move(0, value.toInt());
    });

    d->fadeAnim = new tVariantAnimation(this);
    d->fadeAnim->setStartValue(0);
    d->fadeAnim->setEndValue(255);
    d->fadeAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->fadeAnim->setDuration(500);
    connect(d->fadeAnim, &tVariantAnimation::valueChanged, this, [=] {
        this->update();
    });

    if (d->settings.value("Onboarding/onboardingAudio").toBool()) {
        auto format = QMediaDevices::defaultAudioOutput().preferredFormat();

        QEventLoop* loop = new QEventLoop();
        for (int i = 0; i < 2; i++) {
            QByteArray* array = (i == 0 ? &d->singleAudioData : &d->loopAudioData);
            QString setting = (i == 0 ? "Onboarding/audio.start" : "Onboarding/audio.loop");
            auto file = QUrl::fromLocalFile(d->settings.value(setting).toString());

            QEventLoopLocker* locker = new QEventLoopLocker(loop);
            QAudioDecoder* audioDecoder = new QAudioDecoder();
            audioDecoder->setSource(file);
            audioDecoder->setAudioFormat(format);
            connect(audioDecoder, &QAudioDecoder::bufferReady, this, [=] {
                QAudioBuffer buf = audioDecoder->read();
                array->append(buf.constData<char>(), buf.byteCount());
            });
            connect(audioDecoder, &QAudioDecoder::finished, this, [=] {
                delete locker;
                audioDecoder->deleteLater();
            });
            connect(audioDecoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), this, [=](QAudioDecoder::Error error) {
                delete locker;
                audioDecoder->deleteLater();
            });
            audioDecoder->start();
        }
        loop->exec();
        loop->deleteLater();

        d->audioOutput = new QAudioSink(format);
        d->audioOutput->setBufferSize(176400);
        d->audioOutputDevice = d->audioOutput->start();

        QTimer* audioTimer = new QTimer(d->audioOutput);
        audioTimer->setInterval(100);
        connect(audioTimer, &QTimer::timeout, this, &Onboarding::writeAudio);
        audioTimer->start();

        writeAudio();
    }

    if (d->settings.value("Onboarding/onboardingVideo").toBool()) {
        this->setAttribute(Qt::WA_TranslucentBackground);
    } else {
        this->startOnboarding();
    }

    auto* terminalShortcut = new QShortcut(QKeySequence(Qt::ControlModifier | Qt::AltModifier | Qt::Key_T), this);
    connect(terminalShortcut, &QShortcut::activated, this, [this] {
        this->hide();

        auto env = QProcessEnvironment::systemEnvironment();
        env.remove("LD_LIBRARY_PATH");

        auto* proc = new QProcess(this);
        proc->setProgram("theterminal");
        proc->setArguments({"-e", "/bin/bash"});
        proc->setProcessEnvironment(env);
        proc->start();
        connect(proc, &QProcess::finished, this, [proc, this] {
            proc->deleteLater();
            this->show();
        });
    });
}

Onboarding::~Onboarding() {
    if (d->audioOutput) {
        d->audioOutput->stop();
        d->audioOutput->deleteLater();
    }
    delete d;
    delete ui;
}

void Onboarding::on_stackedWidget_currentChanged(int arg1) {
    if (d->steps.count() > arg1) d->steps.at(arg1).second->pageActivated();
}

void Onboarding::on_bar_closeClicked() {
    StateManager::instance()->powerManager()->showPowerOffConfirmation();
}

void Onboarding::resizeEvent(QResizeEvent* event) {
    if (this->geometry().width() <= SC_DPI(800)) {
        ui->contentWrapper->setFrameShape(QFrame::NoFrame);
        ui->stepperFrame->setVisible(false);
        ui->line->setVisible(false);
    } else if (this->geometry().width() < SC_DPI(1000)) {
        ui->contentWrapper->setFrameShape(QFrame::StyledPanel);
        ui->stepperFrame->setVisible(false);
        ui->line->setVisible(false);
    } else {
        ui->contentWrapper->setFrameShape(QFrame::StyledPanel);
        ui->stepperFrame->setVisible(true);
        ui->line->setVisible(true);
    }
    updateBarVisiblity();
}

void Onboarding::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QLinearGradient grad;
    grad.setStart(0, 0);
    grad.setFinalStop(0, d->bar->height() * 2);
    grad.setColorAt(0, QColor(0, 0, 0, 127));
    grad.setColorAt(1, QColor(0, 0, 0, 0));

    painter.setPen(Qt::transparent);
    painter.setBrush(grad);
    painter.drawRect(0, 0, this->width(), this->height());

    int opacity = d->fadeAnim->currentValue().toInt();
    if (opacity > 0) {
        QColor col = this->palette().color(QPalette::Window);
        col.setAlpha(opacity);
        painter.setBrush(col);
        painter.drawRect(0, 0, this->width(), this->height());
    }
}

void Onboarding::startOnboarding() {
    if (ui->contentWrapper->isVisible()) return;

    this->setCursor(QCursor(Qt::ArrowCursor));

    d->contentAnim->setStartValue(0);
    d->contentAnim->setEndValue(ui->contentWrapper->sizeHint().height());

    QMetaObject::Connection* c = new QMetaObject::Connection();
    *c = connect(d->contentAnim, &tVariantAnimation::finished, this, [=] {
        disconnect(*c);
        delete c;

        // Animate in the bar
        if (this->geometry().height() >= SC_DPI(600) + 2 * d->bar->height()) {
            QMetaObject::Connection* c = new QMetaObject::Connection();
            *c = connect(d->barAnim, &tVariantAnimation::finished, this, [=] {
                disconnect(*c);
                delete c;

                d->onboardingStarted = true;
                updateBarVisiblity();
            });
            showBar();
        } else {
            d->onboardingStarted = true;
            d->barVisible = true;
            updateBarVisiblity();
        }

        d->bar->setVisible(true);
    });
    d->contentAnim->start();
    ui->contentWrapper->setVisible(true);
}

void Onboarding::writeAudio() {
    if (!d->audioEnabled) return;
    if (!d->audioOutputDevice) return;

    int bytesFree = d->audioOutput->bytesFree();
    if (d->audioPointer < d->singleAudioData.length()) {
        QByteArray chunk = d->singleAudioData.mid(d->audioPointer, bytesFree);
        d->audioPointer += chunk.length();
        bytesFree -= chunk.length();
        d->audioOutputDevice->write(chunk);
    }

    // Prevent ourselves from locking up in this loop
    QDeadlineTimer lockTimer(50);
    while (bytesFree != 0 && d->audioPointer >= d->singleAudioData.length() && !lockTimer.hasExpired()) {
        if (d->audioPointer >= d->singleAudioData.length() + d->loopAudioData.length()) d->audioPointer -= d->loopAudioData.length();
        QByteArray chunk = d->loopAudioData.mid(d->audioPointer - d->singleAudioData.length(), bytesFree);
        d->audioPointer += chunk.length();
        bytesFree -= chunk.length();
        d->audioOutputDevice->write(chunk);
    }

    // Disable the audio
    if (lockTimer.hasExpired()) {
        d->audioEnabled = false;
    }
}

void Onboarding::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

        for (QPair<OnboardingStepper*, OnboardingPage*> stepper : d->steppers) {
            stepper.first->setText(stepper.second->displayName());
        }
    }
}

void Onboarding::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        if (!ui->contentWrapper->isVisible()) {
            // Skip the intro sequence
            startOnboarding();
            event->accept();
        }
    } else if (event->key() == Qt::Key_Escape) {
        StateManager::instance()->powerManager()->showPowerOffConfirmation();
        event->accept();
    }
}

bool Onboarding::eventFilter(QObject* watched, QEvent* event) {
    if (watched == d->bar) {
        if (event->type() == QEvent::Enter || event->type() == QEvent::Leave) {
            updateBarVisiblity();
        }
    }
    return false;
}

void Onboarding::completeOnboarding() {
    d->contentAnim->setStartValue(ui->contentWrapper->sizeHint().height());
    d->contentAnim->setEndValue(0);
    d->contentAnim->start();

    // Animate out the bar
    QMetaObject::Connection* c = new QMetaObject::Connection();
    *c = connect(d->barAnim, &tVariantAnimation::finished, this, [=] {
        disconnect(*c);
        delete c;

        StateManager::onboardingManager()->d->onboardingRunning = false;
        emit onboardingCompleted(true);
        this->accept();
    });
    hideBar();

    // Fade out the music
    if (d->audioOutput) {
        tVariantAnimation* volAnim = new tVariantAnimation(this);
        volAnim->setStartValue(d->audioOutput->volume());
        volAnim->setEndValue(0);
        volAnim->setEasingCurve(QEasingCurve::OutCubic);
        volAnim->setDuration(500);
        connect(volAnim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
            d->audioOutput->setVolume(value.toInt());
        });
        connect(volAnim, &tVariantAnimation::finished, this, [=] {
            volAnim->deleteLater();
        });
        volAnim->start();
    }

    // Fade out the background
    d->fadeAnim->start();
}

void Onboarding::showBar() {
    if (d->barVisible) return;
    d->barAnim->setStartValue(-d->bar->height());
    d->barAnim->setEndValue(0);
    d->barAnim->start();
    d->barVisible = true;
}

void Onboarding::hideBar() {
    if (!d->barVisible) return;
    d->barAnim->setStartValue(0);
    d->barAnim->setEndValue(-d->bar->height());
    d->barAnim->start();
    d->barVisible = false;
}

void Onboarding::updateBarVisiblity() {
    if (d->onboardingStarted) {
        if (this->geometry().height() < SC_DPI(600) + 2 * d->bar->height()) {
            if (d->bar->underMouse()) {
                d->bar->move(0, d->barAnim->currentValue().toInt());
            } else {
                d->bar->move(0, -d->bar->height() + 1);
            }
            d->bar->setAutoFillBackground(true);
            tScrim::scrimForWidget(this)->setBlurEnabled(true);
        } else {
            d->bar->move(0, d->barAnim->currentValue().toInt());
            d->bar->setAutoFillBackground(false);
            tScrim::scrimForWidget(this)->setBlurEnabled(false);
        }
    } else {
        d->bar->move(0, -d->bar->height());
    }

    d->bar->setFixedWidth(this->width());
    d->bar->setFixedHeight(d->bar->height());
}
