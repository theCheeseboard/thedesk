/****************************************
 *
 *INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *Copyright (C) 2020 Victor Tran
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "messagedialog.h"
#include "ui_messagedialog.h"

#include <QGraphicsOpacityEffect>
#include <QPushButton>
#include <tvariantanimation.h>

struct MessageDialogPrivate {
        QList<QPushButton*> buttons;
        QGraphicsOpacityEffect* opacity;
        bool showingDetails = false;
};

MessageDialog::MessageDialog(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MessageDialog) {
    ui->setupUi(this);

    d = new MessageDialogPrivate();

    //    this->setWindowFlags(Qt::Widget);
    ui->frame->setMinimumWidth(400);

    d->opacity = new QGraphicsOpacityEffect(this);
    d->opacity->setEnabled(false);
    ui->frame->setGraphicsEffect(d->opacity);

    ui->detailsWidget->setVisible(false);

    ui->dialogTypeWidget->setFixedWidth(3);
}

MessageDialog::~MessageDialog() {
    delete d;
    delete ui;
}

void MessageDialog::setOptions(const QSharedPointer<QMessageDialogOptions>& options) {
    for (QPushButton* btn : d->buttons) {
        ui->buttonLayout->removeWidget(btn);
        btn->deleteLater();
    }
    d->buttons.clear();

    this->setWindowTitle(options->windowTitle());
    ui->titleLabel->setText(options->windowTitle().toUpper());
    ui->messageLabel->setText(options->text());
    if (options->informativeText().isEmpty()) {
        ui->informativeTextLabel->setVisible(false);
    } else {
        ui->informativeTextLabel->setVisible(true);
        ui->informativeTextLabel->setText(options->informativeText());
    }
    if (options->detailedText().isEmpty()) {
        ui->detailsButton->setVisible(false);
    } else {
        ui->detailsButton->setVisible(true);
        ui->detailsLabel->setText(options->detailedText());
    }

    QPalette pal = ui->dialogTypeWidget->palette();
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    switch (options->standardIcon()) {
#else
    switch (options->icon()) {
#endif
        case QMessageDialogOptions::NoIcon:
        case QMessageDialogOptions::Information:
            pal.setColor(QPalette::Window, Qt::transparent);
            break;
        case QMessageDialogOptions::Question:
            pal.setColor(QPalette::Window, QColor(0, 150, 255));
            break;
        case QMessageDialogOptions::Warning:
            pal.setColor(QPalette::Window, QColor(255, 150, 0));
            break;
        case QMessageDialogOptions::Critical:
            pal.setColor(QPalette::Window, QColor(200, 0, 0));
            break;
    }
    ui->dialogTypeWidget->setPalette(pal);

    QVector<QMessageDialogOptions::CustomButton> buttons;

    if (options->standardButtons() & QPlatformDialogHelper::Ok) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ok, tr("OK"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Save) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Save, tr("Save"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::SaveAll) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::SaveAll, tr("Save All"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Open) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Open, tr("Open"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Yes) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Yes, tr("Yes"), QPlatformDialogHelper::YesRole));
    if (options->standardButtons() & QPlatformDialogHelper::YesToAll) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::YesToAll, tr("Yes to All"), QPlatformDialogHelper::YesRole));
    if (options->standardButtons() & QPlatformDialogHelper::No) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::No, tr("No"), QPlatformDialogHelper::NoRole));
    if (options->standardButtons() & QPlatformDialogHelper::NoToAll) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::NoToAll, tr("No to All"), QPlatformDialogHelper::NoRole));
    if (options->standardButtons() & QPlatformDialogHelper::Abort) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Abort, tr("Abort"), QPlatformDialogHelper::DestructiveRole));
    if (options->standardButtons() & QPlatformDialogHelper::Retry) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Retry, tr("Retry"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Ignore) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ignore, tr("Ignore"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Close) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Close, tr("Close"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Cancel) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Cancel, tr("Cancel"), QPlatformDialogHelper::RejectRole));
    if (options->standardButtons() & QPlatformDialogHelper::Discard) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Discard, tr("Discard"), QPlatformDialogHelper::DestructiveRole));
    if (options->standardButtons() & QPlatformDialogHelper::Help) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Help, tr("Help"), QPlatformDialogHelper::ActionRole));
    if (options->standardButtons() & QPlatformDialogHelper::Apply) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Apply, tr("Apply"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Reset) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Reset, tr("Reset"), QPlatformDialogHelper::ResetRole));
    if (options->standardButtons() & QPlatformDialogHelper::RestoreDefaults) buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::RestoreDefaults, tr("Restore Defaults"), QPlatformDialogHelper::ResetRole));

    buttons.append(options->customButtons());

    for (int i = 0; i < buttons.length(); i++) {
        if (buttons.at(i).label == "Show Details...") {
            buttons.removeAt(i);
        }
    }

    if (buttons.isEmpty()) {
        // Add a failsafe button
        buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ok, tr("OK"), QPlatformDialogHelper::AcceptRole));
    }

    for (const QMessageDialogOptions::CustomButton& btn : buttons) {
        QPushButton* button = new QPushButton(this);
        button->setText(btn.label);
        if (btn.role == QPlatformDialogHelper::DestructiveRole) button->setProperty("type", "destructive");
        connect(button, &QPushButton::clicked, this, [=] {
            emit clicked(static_cast<QPlatformDialogHelper::StandardButton>(btn.id), btn.role);
        });
        ui->buttonLayout->addWidget(button);
        d->buttons.append(button);
    }

    if (options->checkBoxLabel().isEmpty()) {
        ui->checkBox->setVisible(false);
    } else {
        ui->checkBox->setVisible(true);
        ui->checkBox->setText(options->checkBoxLabel());
        ui->checkBox->setCheckState(options->checkBoxState());
    }
}

void MessageDialog::setParent(QWidget* parent) {
    parent->installEventFilter(this);
    QWidget::setParent(parent);
}

void MessageDialog::animateIn() {
    d->opacity->setEnabled(true);
    d->opacity->setOpacity(0);

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(400);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        d->opacity->setOpacity(value.toReal());

        QRect geometry = frameGeometry();
        geometry.moveTop(geometry.top() - 10 * (1.0 - value.toReal()));
        this->setFixedSize(QSize(0, 0));
        this->setFixedSize(geometry.size());
        this->setGeometry(geometry);
    });
    connect(anim, &tVariantAnimation::finished, this, [=] {
        d->opacity->setEnabled(false);
        anim->deleteLater();
    });
    anim->start();
}

void MessageDialog::animateOut() {
    d->opacity->setEnabled(true);

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setDuration(400);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        d->opacity->setOpacity(value.toReal());

        QRect geometry = frameGeometry();
        geometry.moveTop(geometry.top() + 10 * (1.0 - value.toReal()));
        this->setFixedSize(QSize(0, 0));
        this->setFixedSize(geometry.size());
        this->setGeometry(geometry);
    });
    connect(anim, &tVariantAnimation::finished, this, [=] {
        anim->deleteLater();
        this->hide();
    });
    anim->start();
}

QRect MessageDialog::frameGeometry() {
    QRect geometry = this->geometry();
    if (d->showingDetails) {
        if (ui->detailsContents->sizeHint().width() > 400) {
            geometry.setSize(ui->detailsContents->sizeHint());
        } else {
            geometry.setSize(QSize(400, ui->detailsContents->heightForWidth(400)));
        }
        if (this->parentWidget() && geometry.height() > this->parentWidget()->height()) geometry.setHeight(this->parentWidget()->height());
    } else {
        if (ui->frame->sizeHint().width() > 400) {
            geometry.setSize(ui->frame->sizeHint());
        } else {
            geometry.setSize(QSize(400, ui->frame->heightForWidth(400)));
        }
    }
    if (this->parentWidget()) {
        geometry.moveCenter(QRect(0, 0, this->parentWidget()->width(), this->parentWidget()->height()).center());
    }

    return geometry;
}

void MessageDialog::updateGeometry() {
    QRect geometry = frameGeometry();
    this->setFixedSize(geometry.size());
    this->setGeometry(geometry);
}

void MessageDialog::resizeEvent(QResizeEvent* event) {
    updateGeometry();
}

void MessageDialog::on_backButton_clicked() {
    ui->detailsWidget->setVisible(false);
    d->showingDetails = false;
    updateGeometry();
    ui->frame->setVisible(true);
}

void MessageDialog::on_detailsButton_clicked() {
    ui->frame->setVisible(false);
    d->showingDetails = true;
    updateGeometry();
    ui->detailsWidget->setVisible(true);
}

bool MessageDialog::eventFilter(QObject* watched, QEvent* event) {
    if (watched == this->parentWidget() && event->type() == QEvent::Resize) {
        updateGeometry();
    }
    return false;
}

void MessageDialog::on_checkBox_stateChanged(int arg1) {
    emit checkBoxStateChanged(static_cast<Qt::CheckState>(arg1));
}
