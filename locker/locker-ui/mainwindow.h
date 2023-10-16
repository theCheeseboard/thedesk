#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

struct MainWindowPrivate;
class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_unlockButton_clicked();

        void on_password_returnPressed();

    private:
        Ui::MainWindow* ui;
        MainWindowPrivate* d;

        bool checkPassword(QString password);

        // QWidget interface
    protected:
        void changeEvent(QEvent* event);

        // QWidget interface
    protected:
        void showEvent(QShowEvent *event);
};

#endif // MAINWINDOW_H
