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
#include "gatewaymanager.h"

struct GatewayManagerPrivate {
    QList<GatewaySearchProvider*> providers;
    int gatewayWidth = 0;
};

GatewayManager::GatewayManager(QObject* parent) : QObject(parent) {
    d = new GatewayManagerPrivate();
}

GatewayManager::~GatewayManager() {
    delete d;
}

void GatewayManager::registerSearchProvider(GatewaySearchProvider* provider) {
    if (!d->providers.contains(provider)) {
        d->providers.append(provider);
        emit searchProviderRegistered(provider);
    }
}

void GatewayManager::deregisterSearchProvider(GatewaySearchProvider* provider) {
    if (d->providers.contains(provider)) {
        d->providers.removeOne(provider);
        emit searchProviderDeregistered(provider);
    }
}

bool GatewayManager::isSearchProviderRegistered(GatewaySearchProvider* provider) {
    return d->providers.contains(provider);
}

QList<GatewaySearchProvider*> GatewayManager::searchProviders() {
    return d->providers;
}

int GatewayManager::gatewayWidth() {
    return d->gatewayWidth;
}

void GatewayManager::setGatewayWidth(int width) {
    d->gatewayWidth = width;
    emit gatewayWidthChanged(width);
}
