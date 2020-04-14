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
#ifndef STATUSCENTERMANAGER_H
#define STATUSCENTERMANAGER_H

#include <QObject>

class BarWindow;
class StatusCenterPane;
class StatusCenter;
class SystemSettings;
class QuickSwitch;
struct StatusCenterManagerPrivate;
class StatusCenterManager : public QObject {
        Q_OBJECT
    public:
        enum PaneType {
            Informational,
            SystemSettings
        };

        explicit StatusCenterManager(QObject* parent = nullptr);
        ~StatusCenterManager();

        void show();
        void hide();
        bool isShowingStatusCenter();

        bool isHamburgerMenuRequired();
        void showStatusCenterHamburgerMenu();
        void hideStatusCenterHamburgerMenu();

        void returnToRootMenu();

        void addPane(StatusCenterPane* pane, PaneType type = Informational);
        void removePane(StatusCenterPane* pane);

        void addSwitch(QuickSwitch* sw);
        void removeSwitch(QuickSwitch* sw);
        bool isSwitchRegistered(QuickSwitch* sw);

        int preferredContentWidth();

    signals:
        void showStatusCenter();
        void hideStatusCenter();
        void showHamburgerMenu();
        void hideHamburgerMenu();
        void rootMenu();

        void paneAdded(StatusCenterPane* pane, PaneType type);
        void paneRemoved(StatusCenterPane* pane);
        void switchAdded(QuickSwitch* sw);
        void switchRemoved(QuickSwitch* sw);

        void isHamburgerMenuRequiredChanged(bool isHamburgerMenuRequired);

    protected:
        friend BarWindow;
        friend StatusCenter;
        friend class SystemSettings;
        void setIsShowingStatusCenter(bool isShowing);
        void setIsHamburgerMenuRequired(bool isRequired);
        QList<StatusCenterPane*> panes();
        QList<QuickSwitch*> switches();
        PaneType paneType(StatusCenterPane* pane);

    private:
        StatusCenterManagerPrivate* d;
};

#endif // STATUSCENTERMANAGER_H
