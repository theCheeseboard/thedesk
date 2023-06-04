/****************************************
 *
 *   theShell - Desktop Environment
 *   Copyright (C) 2019 Victor Tran
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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <Background/backgroundcontroller.h>
#include <QCoroTask>
#include <QDialog>

class ChooseBackground;

namespace Ui {
    class Background;
}

struct BackgroundPrivate;
class Background : public QDialog {
        Q_OBJECT

    public:
        explicit Background();
        ~Background();

        void show();

        static void reconfigureBackgrounds();

    public slots:
        QCoro::Task<> changeBackground();

        void toggleChangeBackground();

        void showCommunityBackgroundSettings(bool shown);

    signals:
        void setAllBackgrounds(QString background);

        void reloadBackground();

    private slots:
        void on_actionChange_Background_triggered();

        void on_Background_customContextMenuRequested(const QPoint& pos);

        void on_tryReloadBackgroundButton_clicked();

        void on_backgroundList_clicked(const QModelIndex& index);

        void on_backButton_clicked();

        void on_showImageInformationBox_toggled(bool checked);

        void on_stretchFitButton_toggled(bool checked);

        void on_zoomCropButton_toggled(bool checked);

        void on_centerButton_toggled(bool checked);

        void on_tileButton_toggled(bool checked);

        void on_zoomFitButton_toggled(bool checked);

        void on_actionEndSession_triggered();

    private:
        Ui::Background* ui;
        BackgroundPrivate* d;

        void reject();

        bool eventFilter(QObject* watched, QEvent* event);
        void paintEvent(QPaintEvent* event);
        void resizeEvent(QResizeEvent* event);

        void resizeToScreen(int screen);

        void changeBackground(QString background, QList<BackgroundController::BackgroundType> types);
};

#endif // BACKGROUND_H
