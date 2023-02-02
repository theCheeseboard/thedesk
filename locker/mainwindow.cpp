#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <SystemSlide/systemslide.h>

struct MainWindowPrivate {
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
}

MainWindow::~MainWindow() {
    delete d;
    delete ui;
}
