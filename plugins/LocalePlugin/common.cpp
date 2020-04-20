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
#include "common.h"

#include <statemanager.h>
#include <localemanager.h>

using namespace Common;

QList<Country> Common::countries() {
    QLocale::Country current = StateManager::localeManager()->formatCountry();
    QList<Country> countries;
    for (int i = QLocale::Afghanistan + 1; i < QLocale::LastCountry; i++) {
        if (i == QLocale::World) continue;

        countries.append({
            static_cast<QLocale::Country>(i),
            QLocale::countryToString(static_cast<QLocale::Country>(i)),
            current == i
        });
    }

    std::sort(countries.begin(), countries.end(), [ = ](const Country & first, const Country & second) {
        return first.text.localeAwareCompare(second.text) < 0;
    });

    return countries;
}
