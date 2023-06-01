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

    private:
        Ui::MainWindow* ui;
        MainWindowPrivate* d;

        bool checkPassword(QString password);
};

#endif // MAINWINDOW_H
