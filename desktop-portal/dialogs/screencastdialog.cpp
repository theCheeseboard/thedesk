#include "screencastdialog.h"
#include "ui_screencastdialog.h"

#include "backend/abstractportalbackend.h"
#include "private/screencast/screencastsourceselectionitem.h"
#include <Applications/application.h>

struct ScreencastDialogPrivate {
};

ScreencastDialog::ScreencastDialog(QString appId, QWidget* parent) :
    QDialog(parent), ui(new Ui::ScreencastDialog) {
    ui->setupUi(this);
    d = new ScreencastDialogPrivate();

    Application app(appId);
    ui->descriptionLabel->setText(tr("%1 is requesting permission to observe your screen.").arg(QLocale().quoteString(app.getProperty("Name").toString())));

    for (auto source : AbstractPortalBackend::instance()->screenCopySources()) {
        auto w = new ScreencastSourceSelectionItem(source);
        ui->outputsListLayout->addWidget(w);
    }
}

ScreencastDialog::~ScreencastDialog() {
    delete d;
    delete ui;
}

void ScreencastDialog::on_titleLabel_backButtonClicked() {
    this->reject();
}
