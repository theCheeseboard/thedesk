#include "screencastdialog.h"
#include "ui_screencastdialog.h"

#include <Applications/application.h>

ScreencastDialog::ScreencastDialog(QString appId, QWidget* parent) :
    QDialog(parent), ui(new Ui::ScreencastDialog) {
    ui->setupUi(this);

    Application app(appId);
    ui->descriptionLabel->setText(tr("%1 is requesting permission to observe your screen.").arg(QLocale().quoteString(app.getProperty("Name").toString())));
}

ScreencastDialog::~ScreencastDialog() {
    delete ui;
}

void ScreencastDialog::on_titleLabel_backButtonClicked() {
    this->reject();
}
