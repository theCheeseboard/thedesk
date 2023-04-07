#include "jobwidget.h"
#include "ui_jobwidget.h"

struct JobWidgetPrivate {
        bool loadingData = true;
        SystemJobPtr job;
};

JobWidget::JobWidget(SystemJobPtr job, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::JobWidget) {
    ui->setupUi(this);

    d = new JobWidgetPrivate();
    d->job = job;

    connect(job.data(), &SystemJob::titleChanged, ui->summaryLabel, &QLabel::setText);
    connect(job.data(), &SystemJob::statusChanged, ui->bodyLabel, &QLabel::setText);
    connect(job.data(), &SystemJob::totalProgressChanged, this, [this](quint64 totalProgress) {
        ui->progressBar->setMaximum(totalProgress);
    });
    connect(job.data(), &SystemJob::progressChanged, ui->progressBar, &QProgressBar::setValue);

    connect(job.data(), &SystemJob::stateChanged, this, &JobWidget::updateState);
    connect(job.data(), &SystemJob::validChanged, this, [this](bool valid) {
        if (!valid && !d->loadingData) this->deleteLater();
    });

    loadData();
}

JobWidget::~JobWidget() {
    delete ui;
    delete d;
}

void JobWidget::setIsLast(bool isLast) {
    ui->line->setVisible(!isLast);
}

QCoro::Task<> JobWidget::loadData() {
    d->loadingData = true;
    ui->summaryLabel->setText(co_await d->job->title());
    ui->bodyLabel->setText(co_await d->job->status());
    ui->progressBar->setMaximum(co_await d->job->totalProgress());
    ui->progressBar->setValue(co_await d->job->progress());

    updateState(co_await d->job->state());
    if (!d->job->valid()) this->deleteLater();
    d->loadingData = false;
}

void JobWidget::updateState(QString state) {
    if (state == QStringLiteral("Processing")) {
    } else if (state == QStringLiteral("Finished")) {
        this->deleteLater();
    } else if (state == QStringLiteral("Failed")) {
        this->deleteLater();
    } else if (state == QStringLiteral("RequiresAttention")) {
    }
}
