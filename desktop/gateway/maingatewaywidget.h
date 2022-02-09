/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#ifndef MAINGATEWAYWIDGET_H
#define MAINGATEWAYWIDGET_H

#include <QWidget>

namespace Ui {
    class MainGatewayWidget;
}

struct MainGatewayWidgetPrivate;
class MainGatewayWidget : public QWidget {
        Q_OBJECT

    public:
        explicit MainGatewayWidget(QWidget* parent = nullptr);
        ~MainGatewayWidget();

        QSize sizeHint() const;

        void clearState();

    private slots:
        void on_searchBox_textEdited(const QString& arg1);

        void on_appsList_clicked(const QModelIndex& index);

        void on_searchBox_returnPressed();

        void on_endSessionButton_clicked();

        void on_allAppsButton_toggled(bool checked);

        void on_titleLabel_backButtonClicked();

        void on_statusCenterButton_clicked();

        void on_systemSettingsButton_clicked();

    signals:
        void closeGateway();

    private:
        Ui::MainGatewayWidget* ui;
        MainGatewayWidgetPrivate* d;

        void launch(QModelIndex applicationIndex);

        void changeEvent(QEvent* event);
        void resizeEvent(QResizeEvent* event);

        // QObject interface
    public:
        bool eventFilter(QObject*watched, QEvent*event);
};

#endif // MAINGATEWAYWIDGET_H
