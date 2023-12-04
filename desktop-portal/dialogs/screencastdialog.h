#ifndef SCREENCASTDIALOG_H
#define SCREENCASTDIALOG_H

#include <QDialog>

namespace Ui {
    class ScreencastDialog;
}

struct ScreencastDialogPrivate;
class ScreencastDialog : public QDialog {
        Q_OBJECT

    public:
        explicit ScreencastDialog(QString appId, QWidget* parent = nullptr);
        ~ScreencastDialog();

    private slots:
        void on_titleLabel_backButtonClicked();

    private:
        Ui::ScreencastDialog* ui;
        ScreencastDialogPrivate* d;
};

#endif // SCREENCASTDIALOG_H
