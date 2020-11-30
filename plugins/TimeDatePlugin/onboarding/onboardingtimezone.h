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
#ifndef ONBOARDINGTIMEZONE_H
#define ONBOARDINGTIMEZONE_H

#include <QWidget>
#include <onboardingpage.h>

namespace Ui {
    class OnboardingTimeZone;
}

struct OnboardingTimeZonePrivate;
class OnboardingTimeZone : public OnboardingPage {
        Q_OBJECT

    public:
        explicit OnboardingTimeZone(QWidget* parent = nullptr);
        ~OnboardingTimeZone();

    private:
        Ui::OnboardingTimeZone* ui;
        OnboardingTimeZonePrivate* d;

        // OnboardingPage interface
    public:
        QString name();
        QString displayName();
    private slots:
        void on_titleLabel_backButtonClicked();
        void on_nextButton_clicked();
        void on_searchBox_textChanged(const QString& arg1);
        void on_listView_activated(const QModelIndex& index);
        void on_ntpCheckbox_toggled(bool checked);
};

#endif // ONBOARDINGTIMEZONE_H
