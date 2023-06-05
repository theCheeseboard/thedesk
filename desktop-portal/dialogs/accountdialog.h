#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
    class AccountDialog;
}

class AccountDialog : public QDialog {
        Q_OBJECT

    public:
        explicit AccountDialog(QString appId, QString reason, QWidget* parent = nullptr);
        ~AccountDialog();

        QVariantMap results();

    private slots:
        void on_okButton_clicked();

        void on_titleLabel_backButtonClicked();

    private:
        Ui::AccountDialog* ui;
};

#endif // ACCOUNTDIALOG_H
