#ifndef JOBWIDGET_H
#define JOBWIDGET_H

#include <QWidget>
#include <SystemJob/systemjob.h>

namespace Ui {
    class JobWidget;
}

struct JobWidgetPrivate;
class JobWidget : public QWidget {
        Q_OBJECT

    public:
        explicit JobWidget(SystemJobPtr job, QWidget* parent = nullptr);
        ~JobWidget();

        void setIsLast(bool isLast);

    private:
        Ui::JobWidget* ui;
        JobWidgetPrivate* d;

        QCoro::Task<> loadData();
        void updateState(QString state);
};

#endif // JOBWIDGET_H
