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
#ifndef LIBTHEDESK_GLOBAL_H
#define LIBTHEDESK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBTHEDESK_LIBRARY)
#  define LIBTHEDESK_EXPORT Q_DECL_EXPORT
#else
#  define LIBTHEDESK_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBTHEDESK_GLOBAL_H
