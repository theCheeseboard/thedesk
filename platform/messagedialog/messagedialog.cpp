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

#include <QPushButton>

struct MessageDialogPrivate {
    QList<QPushButton*> buttons;
};

MessageDialog::MessageDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog) {
    ui->setupUi(this);

    d = new MessageDialogPrivate();
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

    QVector<QMessageDialogOptions::CustomButton> buttons;

    if (options->standardButtons() & QPlatformDialogHelper::Ok)                 buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ok, tr("OK"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Save)               buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Save, tr("Save"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::SaveAll)            buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::SaveAll, tr("Save All"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Open)               buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Open, tr("Open"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Yes)                buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Yes, tr("Yes"), QPlatformDialogHelper::YesRole));
    if (options->standardButtons() & QPlatformDialogHelper::YesToAll)           buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::YesToAll, tr("Yes to All"), QPlatformDialogHelper::YesRole));
    if (options->standardButtons() & QPlatformDialogHelper::No)                 buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::No, tr("No"), QPlatformDialogHelper::NoRole));
    if (options->standardButtons() & QPlatformDialogHelper::NoToAll)            buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::NoToAll, tr("No to All"), QPlatformDialogHelper::NoRole));
    if (options->standardButtons() & QPlatformDialogHelper::Abort)              buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Abort, tr("Abort"), QPlatformDialogHelper::DestructiveRole));
    if (options->standardButtons() & QPlatformDialogHelper::Retry)              buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Retry, tr("Retry"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Ignore)             buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ignore, tr("Ignore"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Close)              buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Close, tr("Close"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Cancel)             buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Cancel, tr("Cancel"), QPlatformDialogHelper::RejectRole));
    if (options->standardButtons() & QPlatformDialogHelper::Discard)            buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Discard, tr("Discard"), QPlatformDialogHelper::DestructiveRole));
    if (options->standardButtons() & QPlatformDialogHelper::Help)               buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Help, tr("Help"), QPlatformDialogHelper::ActionRole));
    if (options->standardButtons() & QPlatformDialogHelper::Apply)              buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Apply, tr("Apply"), QPlatformDialogHelper::AcceptRole));
    if (options->standardButtons() & QPlatformDialogHelper::Reset)              buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Reset, tr("Reset"), QPlatformDialogHelper::ResetRole));
    if (options->standardButtons() & QPlatformDialogHelper::RestoreDefaults)    buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::RestoreDefaults, tr("Restore Defaults"), QPlatformDialogHelper::ResetRole));

    buttons.append(options->customButtons());

    if (buttons.isEmpty()) {
        //Add a failsafe button
        buttons.append(QMessageDialogOptions::CustomButton(QPlatformDialogHelper::Ok, tr("OK"), QPlatformDialogHelper::AcceptRole));
    }

    for (QMessageDialogOptions::CustomButton btn : buttons) {
        QPushButton* button = new QPushButton(this);
        button->setText(btn.label);
        if (btn.role == QPlatformDialogHelper::DestructiveRole) button->setProperty("type", "destructive");
        connect(button, &QPushButton::clicked, this, [ = ] {
            emit clicked(static_cast<QPlatformDialogHelper::StandardButton>(btn.id), btn.role);
            this->close();
        });
        ui->buttonLayout->addWidget(button);
        d->buttons.append(button);
    }

    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}
