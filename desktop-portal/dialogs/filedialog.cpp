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
#include "filedialog.h"
#include "ui_filedialog.h"

#include <tinputdialog.h>
#include <QDBusMetaType>
#include <resourcemanager.h>
#include <QMessageBox>

struct FileDialogPrivate {
    QStringList urls;
};

struct Filter {
    uint type;
    QString pattern;
};
Q_DECLARE_METATYPE(Filter)

QDBusArgument& operator<<(QDBusArgument& argument, const Filter& filter) {
    argument.beginStructure();
    argument << filter.type << filter.pattern;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, Filter& filter) {
    argument.beginStructure();
    argument >> filter.type >> filter.pattern;
    argument.endStructure();
    return argument;
}

struct FilterCategory {
    QString name;
    QList<Filter> filters;
};
Q_DECLARE_METATYPE(FilterCategory)

QDBusArgument& operator<<(QDBusArgument& argument, const FilterCategory& filter) {
    argument.beginStructure();
    argument << filter.name << filter.filters;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, FilterCategory& filter) {
    argument.beginStructure();
    argument >> filter.name >> filter.filters;
    argument.endStructure();
    return argument;
}

FileDialog::FileDialog(bool isSave, QVariantMap options, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::FileDialog) {
    ui->setupUi(this);

    d = new FileDialogPrivate();

    ui->titleLabel->setBackButtonShown(true);

    bool setOpenButton = true;
    QList<FileTab::ColumnAction> columnActions;

    if (isSave) {
        columnActions.append({
            tr("Save Here"),
            tr("Create New File Here"),
            [ = ](DirectoryPtr directory) {
                QUrl directoryUrl = directory->url();
                if (!directoryUrl.path().endsWith("/")) directoryUrl.setPath(directoryUrl.path() + "/");

                bool saved = false;
                QString filename;

                while (!saved) {
                    bool ok;
                    filename = tInputDialog::getText(this, tr("Name this file"), tr("What do you want to name the file?"), QLineEdit::Normal, filename, &ok);
                    if (!ok) return;

                    saved = acceptSave(directoryUrl.resolved(filename));
                }
            }
        });
    } else {
        if (options.value("directory", false).toBool()) {
            columnActions.append({
                tr("Open Folder"),
                tr("Choose This Folder"),
                [ = ](DirectoryPtr directory) {
                    d->urls.append(directory->url().toString());
                    this->accept();
                }
            });
            setOpenButton = false;
        }
    }

    ui->filePicker->setColumnActions(columnActions);

    if (setOpenButton) {
        QString openButtonText = options.value("accept_label", isSave ? tr("Overwrite") : tr("Open")).toString();
        QIcon openButtonIcon = QIcon::fromTheme(isSave ? "document-save" : "document-open");

        ui->filePicker->setOpenFileButtons({
            {
                openButtonText,
                openButtonIcon,
                [ = ](QList<QUrl> selected) {
                    if (isSave) {
                        acceptSave(selected.first());
                    } else {
                        for (QUrl url : selected) {
                            d->urls.append(url.toString());
                        }
                        this->accept();
                    }
                }
            }
        });
    }

    //Set up filters
    qDBusRegisterMetaType<Filter>();
    qDBusRegisterMetaType<FilterCategory>();
    qDBusRegisterMetaType<QList<Filter>>();
    qDBusRegisterMetaType<QList<FilterCategory>>();

    QDBusArgument filtersArgument = options.value("filters").value<QDBusArgument>();

    QList<FilterCategory> filters;
    filtersArgument >> filters;

    QList<FileTab::Filter> pickerFilters;
    for (FilterCategory filterCatg : filters) {
        for (Filter filter : filterCatg.filters) {
            pickerFilters.append(FileTab::Filter{filter.type == 1, filter.pattern});
        }
    }
    ui->filePicker->setFilters(pickerFilters);

    this->resize(SC_DPI_T(this->size(), QSize));
}

FileDialog::~FileDialog() {
    delete d;
    delete ui;
}

QStringList FileDialog::uris() {
    return d->urls;
}

QVariant FileDialog::choices() {
    return QVariant();
}

bool FileDialog::isWritable() {
    return true;
}

void FileDialog::setWindowTitle(QString windowTitle) {
    QDialog::setWindowTitle(windowTitle);
    ui->titleLabel->setText(windowTitle);
}

void FileDialog::on_titleLabel_backButtonClicked() {
    this->reject();
}

bool FileDialog::acceptSave(QUrl url) {
    if (ResourceManager::parentDirectoryForUrl(url)->isFile(url.fileName())) {
        QMessageBox box(this);
        box.setWindowTitle(tr("Overwrite?"));
        box.setText(tr("A file named %1 already exists. Do you want to overwrite it?").arg(QLocale().quoteString(url.fileName())));
        box.setInformativeText(tr("Overwriting the file will replace its contents."));
        box.setIcon(QMessageBox::Warning);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (box.exec() == QMessageBox::No) return false;
    }

    d->urls.append(url.toString());
    this->accept();
    return true;
}

