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
#ifndef ONBOARDINGREGION_H
#define ONBOARDINGREGION_H

#include <onboardingpage.h>
#include <QListWidgetItem>

namespace Ui {
    class OnboardingRegion;
}

class OnboardingRegion : public OnboardingPage {
        Q_OBJECT

    public:
        explicit OnboardingRegion(QWidget* parent = nullptr);
        ~OnboardingRegion();

    private slots:
        void on_nextButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_countriesWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

        void on_searchEdit_textChanged(const QString& arg1);

    private:
        Ui::OnboardingRegion* ui;

        void search(QString query);

        // OnboardingPage interface
    public:
        QString name();
        QString displayName();
};

#endif // ONBOARDINGREGION_H
