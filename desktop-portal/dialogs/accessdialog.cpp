#include "accessdialog.h"
#include "ui_accessdialog.h"

AccessDialog::AccessDialog(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AccessDialog) {
    ui->setupUi(this);

    ui->rejectButton->setProperty("type", "destructive");
}

AccessDialog::~AccessDialog() {
    delete ui;
}

void AccessDialog::setTitle(QString title) {
    ui->titleLabel->setText(title);
}

void AccessDialog::setSubtitle(QString subtitle) {
    ui->subtitleLabel->setText(subtitle);
}

void AccessDialog::setBody(QString body) {
    ui->bodyLabel->setText(body);
}

void AccessDialog::setDenyButtonText(QString text) {
    ui->rejectButton->setText(text);
}

void AccessDialog::setAllowButtonText(QString text) {
    ui->acceptButton->setText(text);
}

void AccessDialog::on_titleLabel_backButtonClicked() {
    emit reject();
}

void AccessDialog::on_acceptButton_clicked() {
    emit accept();
}

void AccessDialog::on_rejectButton_clicked() {
    emit reject();
}
