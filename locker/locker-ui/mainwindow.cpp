#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoroProcess>
#include <QProcess>
#include <SystemSlide/systemslide.h>

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

    connect(d->slide, &SystemSlide::deactivated, this, [this]() -> QCoro::Task<> {
        if (this->checkPassword("")) {
            QApplication::exit(0);
        }

        //        tPropertyAnimation* opacity = new tPropertyAnimation(passwordFrameOpacity, "opacity");
        //        opacity->setStartValue(0.0);
        //        opacity->setEndValue(1.0);
        //        opacity->setDuration(500);
        //        opacity->setEasingCurve(QEasingCurve::OutCubic);
        //        connect(opacity, SIGNAL(finished()), opacity, SLOT(deleteLater()));
        //        opacity->start();

        //        QTimer::singleShot(100, this, [ = ] {
        //            ui->MouseUnderline->startAnimation();
        //            ui->PasswordUnderline->startAnimation();
        //        });

        //        ui->password->setFocus();
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
    checker.waitForFinished();
    return checker.exitCode() == 0;
}
