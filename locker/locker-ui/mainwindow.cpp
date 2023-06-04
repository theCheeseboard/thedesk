#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoroProcess>
#include <QProcess>
#include <SystemSlide/systemslide.h>
#include <Wm/desktopwm.h>
#include <lockmanager.h>
#include <terrorflash.h>

struct MainWindowPrivate {
        QString lockCheckProcess;

        SystemSlide* slide;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d = new MainWindowPrivate();
    d->slide = new SystemSlide(this);
    d->slide->setBackgroundMode(SystemSlide::DesktopBackground);
    d->slide->setAction(tr("Unlock"), tr("Resume your session, continuing where you left off"));
    d->slide->setActionIcon(QIcon::fromTheme("go-up"));
    d->slide->setDragResultWidget(this->centralWidget());

    if (qEnvironmentVariableIsSet("TD_LOCK_CHECKER")) {
        d->lockCheckProcess = qEnvironmentVariable("TD_LOCK_CHECKER");
    } else {
        d->lockCheckProcess = "/usr/lib/td-locker-checker"; // TODO: Don't hardcode
    }

    ui->usernameLabel->setText(DesktopWm::userDisplayName());
    ui->capsLockOn->setVisible(false);

    connect(d->slide, &SystemSlide::deactivated, this, [this] {
        if (this->checkPassword("")) {
            QApplication::exit(0);
            return;
        }

        ui->password->setFocus();

        QTimer::singleShot(100, this, [this] {
            ui->PasswordUnderline->startAnimation();
        });
    });
}

MainWindow::~MainWindow() {
    delete d;
    delete ui;
}

void MainWindow::on_titleLabel_backButtonClicked() {
    d->slide->activate();
}

bool MainWindow::checkPassword(QString password) {
    QString name = qgetenv("USER");
    if (name.isEmpty()) {
        name = qgetenv("USERNAME");
    }

    QProcess checker;
    checker.start(d->lockCheckProcess, {name});
    checker.write(password.toUtf8());
    checker.write("\n");
    checker.closeWriteChannel();
    checker.waitForFinished();
    return checker.exitCode() == 0;
}

void MainWindow::on_unlockButton_clicked() {
    this->setEnabled(false);
    if (checkPassword(ui->password->text())) {
        QApplication::exit(0);
        return;
    }

    this->setEnabled(true);
    ui->password->setText("");
    ui->password->setFocus();

    tErrorFlash::flashError(ui->password);
}

void MainWindow::on_password_returnPressed() {
    ui->unlockButton->click();
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::ActivationChange) {
        LockManager::instance()->raiseAll();
    }
}
