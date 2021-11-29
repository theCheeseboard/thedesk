#include "powersettings.h"
#include "ui_powersettings.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <QIcon>
#include <tsettings.h>
#include <DesktopPowerProfiles/desktoppowerprofiles.h>

struct PowerSettingsPrivate {
    tSettings settings;
    DesktopPowerProfiles* profiles;

    const static QStringList timeoutUnits;
    const static QStringList powerActions;
};

const QStringList PowerSettingsPrivate::timeoutUnits = {
    "sec",
    "min",
    "hr",
    "never"
};

const QStringList PowerSettingsPrivate::powerActions = {
    "ask",
    "poweroff",
    "reboot",
    "suspend",
    "hibernate",
    "ignore"
};

PowerSettings::PowerSettings(DesktopPowerProfiles* powerProfiles) :
    StatusCenterPane(),
    ui(new Ui::PowerSettings) {
    ui->setupUi(this);

    d = new PowerSettingsPrivate();
    d->profiles = powerProfiles;

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->powerProfileWidget->setFixedWidth(contentWidth);
    ui->timeoutsWidget->setFixedWidth(contentWidth);
    ui->buttonsWidget->setFixedWidth(contentWidth);

    connect(&d->settings, &tSettings::settingChanged, this, &PowerSettings::settingChanged);
    for (QString setting : {
            "Power/timeouts.screenoff.value",
            "Power/timeouts.screenoff.unit",
            "Power/timeouts.suspend.value",
            "Power/timeouts.suspend.unit",
            "Power/actions.powerbutton"
        }) {
        settingChanged(setting, d->settings.value(setting));
    }

    connect(d->profiles, &DesktopPowerProfiles::powerProfileChanged, this, &PowerSettings::updatePowerProfiles);
    updatePowerProfiles();
}

PowerSettings::~PowerSettings() {
    delete d;
    delete ui;
}

QString PowerSettings::name() {
    return "PowerSettings";
}

QString PowerSettings::displayName() {
    return tr("Power");
}

QIcon PowerSettings::icon() {
    return QIcon::fromTheme("preferences-system-power");
}

QWidget* PowerSettings::leftPane() {
    return nullptr;
}

void PowerSettings::on_titleLabel_backButtonClicked() {
    StateManager::instance()->statusCenterManager()->showStatusCenterHamburgerMenu();
}

void PowerSettings::on_screenOffSpin_valueChanged(int arg1) {
    d->settings.setValue("Power/timeouts.screenoff.value", arg1);
}

void PowerSettings::on_screenOffUnit_currentIndexChanged(int index) {
    d->settings.setValue("Power/timeouts.screenoff.unit", d->timeoutUnits.value(index));
}

void PowerSettings::settingChanged(QString key, QVariant value) {
    if (key == "Power/timeouts.screenoff.value") {
        ui->screenOffSpin->setValue(value.toInt());
    } else if (key == "Power/timeouts.screenoff.unit") {
        QString unit = value.toString();
        ui->screenOffUnit->setCurrentIndex(d->timeoutUnits.indexOf(unit));
        ui->screenOffSpin->setEnabled(unit != "never");
    } else if (key == "Power/timeouts.suspend.value") {
        ui->suspendSpin->setValue(value.toInt());
    } else if (key == "Power/timeouts.suspend.unit") {
        QString unit = value.toString();
        ui->suspendUnit->setCurrentIndex(d->timeoutUnits.indexOf(unit));
        ui->suspendSpin->setEnabled(unit != "never");
    } else if (key == "Power/actions.powerbutton") {
        ui->powerButtonActionBox->setCurrentIndex(d->powerActions.indexOf(value.toString()));
    }
}

void PowerSettings::updatePowerProfiles() {
    bool isPerformanceAvailable = d->profiles->isPerformanceAvailable();
    ui->performanceProfileButton->setVisible(isPerformanceAvailable);
    ui->performanceDescription->setVisible(isPerformanceAvailable);

    switch (d->profiles->currentPowerProfile()) {
        case DesktopPowerProfiles::PowerStretch:
            ui->powerStretchProfileButton->setChecked(true);
            break;
        case DesktopPowerProfiles::Balanced:
            ui->balancedProfileButton->setChecked(true);
            break;
        case DesktopPowerProfiles::Performance:
            ui->performanceProfileButton->setChecked(true);
            break;
        case DesktopPowerProfiles::Unknown:
            break;
    }

    if (!d->profiles->performanceInhibited().isEmpty()) {
        ui->performanceInhibition->setTitle(tr("Performance mode unavailable"));
        ui->performanceInhibition->setText(performanceInhibitionReason(d->profiles->performanceInhibited()).arg(tr("unavailable")));
        ui->performanceInhibition->setState(tStatusFrame::Error);
        ui->performanceProfileButton->setEnabled(false);
        ui->performanceInhibition->setVisible(true);
    } else if (!d->profiles->performanceDegraded().isEmpty()) {
        ui->performanceInhibition->setTitle(tr("Performance mode temporarily unavailable"));
        ui->performanceInhibition->setText(performanceInhibitionReason(d->profiles->performanceDegraded()).arg(tr("temporarily unavailable")));
        ui->performanceInhibition->setState(tStatusFrame::Warning);
        ui->performanceProfileButton->setEnabled(true);
        ui->performanceInhibition->setVisible(true);
    } else {
        ui->performanceInhibition->setVisible(false);
        ui->performanceProfileButton->setEnabled(true);
    }
}

QString PowerSettings::performanceInhibitionReason(QString reason) {
    if (reason == "lap-detected") return tr("Performance mode is %1 because the device is on your lap.");
    if (reason == "high-operating-temperature") return tr("Performance mode is %1 because your device is getting warm.");
    return tr("Performance mode is %1.");
}

void PowerSettings::on_suspendSpin_valueChanged(int arg1) {
    d->settings.setValue("Power/timeouts.suspend.value", arg1);
}

void PowerSettings::on_suspendUnit_currentIndexChanged(int index) {
    d->settings.setValue("Power/timeouts.suspend.unit", d->timeoutUnits.value(index));
}

void PowerSettings::on_powerButtonActionBox_currentIndexChanged(int index) {
    d->settings.setValue("Power/actions.powerbutton", d->powerActions.value(index));
}

void PowerSettings::on_balancedProfileButton_toggled(bool checked) {
    if (checked) d->profiles->setCurrentPowerProfile(DesktopPowerProfiles::Balanced);
}

void PowerSettings::on_powerStretchProfileButton_toggled(bool checked) {
    if (checked) d->profiles->setCurrentPowerProfile(DesktopPowerProfiles::PowerStretch);
}

void PowerSettings::on_performanceProfileButton_toggled(bool checked) {
    if (checked) d->profiles->setCurrentPowerProfile(DesktopPowerProfiles::Performance);
}

