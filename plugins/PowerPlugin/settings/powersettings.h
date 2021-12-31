#ifndef POWERSETTINGS_H
#define POWERSETTINGS_H

#include <QWidget>

#include <statuscenterpane.h>

namespace Ui {
    class PowerSettings;
}

class DesktopPowerProfiles;
struct PowerSettingsPrivate;
class PowerSettings : public StatusCenterPane {
        Q_OBJECT

    public:
        explicit PowerSettings(DesktopPowerProfiles* powerProfiles);
        ~PowerSettings();

    private slots:
        void on_titleLabel_backButtonClicked();
        void on_screenOffSpin_valueChanged(int arg1);
        void on_screenOffUnit_currentIndexChanged(int index);

        void on_suspendSpin_valueChanged(int arg1);

        void on_suspendUnit_currentIndexChanged(int index);

        void on_powerButtonActionBox_currentIndexChanged(int index);

        void on_balancedProfileButton_toggled(bool checked);

        void on_powerStretchProfileButton_toggled(bool checked);

        void on_performanceProfileButton_toggled(bool checked);

        void on_lockScreenAfterSuspendSwitch_toggled(bool checked);

private:
        Ui::PowerSettings* ui;
        PowerSettingsPrivate* d;

        void settingChanged(QString key, QVariant value);

        void updatePowerProfiles();
        QString performanceInhibitionReason(QString reason);

        // StatusCenterPane interface
    public:
        QString name();
        QString displayName();
        QIcon icon();
        QWidget* leftPane();
};

#endif // POWERSETTINGS_H
