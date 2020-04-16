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
#ifndef LOCALESELECTOR_H
#define LOCALESELECTOR_H

#include <QWidget>
#include <QLocale>
#include <QListWidgetItem>

namespace Ui {
    class LocaleSelector;
}

struct LocaleSelectorPrivate;
class LocaleSelector : public QWidget {
        Q_OBJECT

    public:
        explicit LocaleSelector(QWidget* parent = nullptr);
        ~LocaleSelector();

    signals:
        void rejected();
        void accepted(QLocale locale);

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_languageSelection_itemActivated(QListWidgetItem* item);

        void on_localeSelection_itemActivated(QListWidgetItem* item);

        void on_languageTitle_backButtonClicked();

    private:
        Ui::LocaleSelector* ui;
        LocaleSelectorPrivate* d;
};

#endif // LOCALESELECTOR_H
