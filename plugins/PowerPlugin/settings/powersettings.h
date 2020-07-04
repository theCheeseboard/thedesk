#ifndef POWERSETTINGS_H
#define POWERSETTINGS_H

#include <QWidget>

#include <statuscenterpane.h>

namespace Ui {
    class PowerSettings;
}

struct PowerSettingsPrivate;
class PowerSettings : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit PowerSettings();
        ~PowerSettings();

    private slots:
        void on_titleLabel_backButtonClicked();
        void on_screenOffSpin_valueChanged(int arg1);
        void on_screenOffUnit_currentIndexChanged(int index);

        void on_suspendSpin_valueChanged(int arg1);

        void on_suspendUnit_currentIndexChanged(int index);

        void on_powerButtonActionBox_currentIndexChanged(int index);

    private:
        Ui::PowerSettings* ui;
        PowerSettingsPrivate* d;

        void settingChanged(QString key, QVariant value);

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();
};

#endif // POWERSETTINGS_H
