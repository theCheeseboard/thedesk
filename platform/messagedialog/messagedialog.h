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
#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QWidget>
#include <qpa/qplatformdialoghelper.h>

namespace Ui {
    class MessageDialog;
}

struct MessageDialogPrivate;
class MessageDialog : public QWidget {
        Q_OBJECT

    public:
        explicit MessageDialog(QWidget* parent = nullptr);
        ~MessageDialog();
        
        void extracted(QVector<QMessageDialogOptions::CustomButton> &buttons);
        void setOptions(const QSharedPointer<QMessageDialogOptions> &options);
        void setParent(QWidget* parent);

        void animateIn();
        void animateOut();

        QRect frameGeometry();
        void updateGeometry();

    signals:
        void clicked(QPlatformDialogHelper::StandardButton button, QPlatformDialogHelper::ButtonRole role);

    private:
        Ui::MessageDialog* ui;
        MessageDialogPrivate* d;

        // QWidget interface
    protected:
        void resizeEvent(QResizeEvent* event);

    private slots:
        void on_backButton_clicked();
        void on_detailsButton_clicked();

        // QObject interface
    public:
        bool eventFilter(QObject* watched, QEvent* event);
};

#endif // MESSAGEDIALOG_H
