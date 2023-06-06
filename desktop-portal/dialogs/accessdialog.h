#ifndef ACCESSDIALOG_H
#define ACCESSDIALOG_H

#include <QWidget>

namespace Ui {
    class AccessDialog;
}

class AccessDialog : public QWidget {
        Q_OBJECT

    public:
        explicit AccessDialog(QWidget* parent = nullptr);
        ~AccessDialog();

        void setTitle(QString title);
        void setSubtitle(QString subtitle);
        void setBody(QString body);

        void setDenyButtonText(QString text);
        void setAllowButtonText(QString text);

    signals:
        void reject();
        void accept();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_acceptButton_clicked();

        void on_rejectButton_clicked();

    private:
        Ui::AccessDialog* ui;
};

#endif // ACCESSDIALOG_H
