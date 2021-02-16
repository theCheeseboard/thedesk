/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#ifndef PAIRPOPOVER_H
#define PAIRPOPOVER_H

#include <QWidget>
#include <tpromise.h>
#include <BluezQt/Manager>

namespace Ui {
    class PairPopover;
}

struct PairPopoverPrivate;
class BtAgent;
class PairPopover : public QWidget {
        Q_OBJECT

    public:
        explicit PairPopover(BluezQt::Manager* manager, BtAgent* agent, QWidget* parent = nullptr);
        ~PairPopover();

        enum PairConfirmationType {
            ConfirmPinCode,
            KeyPinCode
        };

        tPromise<QString>* triggerPairConfirmation(PairConfirmationType pairType, QString pinCode);
        void cancelPendingPairing();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_pairList_activated(const QModelIndex& index);

        void on_confirmCodeAcceptButton_clicked();

        void on_confirmCodeTitleLabel_backButtonClicked();

    private:
        Ui::PairPopover* ui;
        PairPopoverPrivate* d;
};

#endif // PAIRPOPOVER_H
