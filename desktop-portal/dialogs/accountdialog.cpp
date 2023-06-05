#include "accountdialog.h"
#include "ui_accountdialog.h"

#include <Applications/application.h>
#include <QTemporaryFile>
#include <Wm/desktopwm.h>
#include <tcontentsizer.h>

AccountDialog::AccountDialog(QString appId, QString reason, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AccountDialog) {
    ui->setupUi(this);

    new tContentSizer(ui->appInfoWidget);
    new tContentSizer(ui->userInfoWidget);
    new tContentSizer(ui->okButton);

    Application app(appId);

    ui->appInfoDescriptionLabel->setText(tr("%1 is requesting information about your profile.").arg(QLocale().quoteString(app.getProperty("Name").toString())));
    if (reason.isEmpty()) {
        ui->reasonLabel->setVisible(false);
    } else {
        ui->reasonLabel->setText(reason);
    }

    ui->usernameLabel->setText(DesktopWm::userUserName());
    ui->realNameLabel->setText(DesktopWm::userDisplayName());
}

AccountDialog::~AccountDialog() {
    delete ui;
}

QVariantMap AccountDialog::results() {
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    tempFile.open();
    QIcon::fromTheme("user").pixmap(QSize(512, 512)).save(&tempFile, "PNG");
    tempFile.close();

    return {
        {"id",    DesktopWm::userUserName()                          },
        {"name",  DesktopWm::userDisplayName()                       },
        {"image", QUrl::fromLocalFile(tempFile.fileName()).toString()}
    };
}

void AccountDialog::on_okButton_clicked() {
    this->accept();
}

void AccountDialog::on_titleLabel_backButtonClicked() {
    this->reject();
}
