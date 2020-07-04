#include "powersettings.h"
#include "ui_powersettings.h"

#include <statemanager.h>
#include <statuscentermanager.h>
#include <QIcon>
#include <tsettings.h>

struct PowerSettingsPrivate {
    tSettings settings;

    const static QStringList timeoutUnits;
};

const QStringList PowerSettingsPrivate::timeoutUnits = {
    "sec",
    "min",
    "hr",
    "never"
};

PowerSettings::PowerSettings() :
    StatusCenterPane(),
    ui(new Ui::PowerSettings) {
    ui->setupUi(this);

    d = new PowerSettingsPrivate();

    ui->titleLabel->setBackButtonIsMenu(true);
    ui->titleLabel->setBackButtonShown(StateManager::instance()->statusCenterManager()->isHamburgerMenuRequired());
    connect(StateManager::instance()->statusCenterManager(), &StatusCenterManager::isHamburgerMenuRequiredChanged, ui->titleLabel, &tTitleLabel::setBackButtonShown);

    const int contentWidth = StateManager::instance()->statusCenterManager()->preferredContentWidth();
    ui->timeoutsWidget->setFixedWidth(contentWidth);

    connect(&d->settings, &tSettings::settingChanged, this, &PowerSettings::settingChanged);
    for (QString setting : {
            "Power/timeouts.screenoff.value",
            "Power/timeouts.screenoff.unit",
            "Power/timeouts.suspend.value",
            "Power/timeouts.suspend.unit"
        }) {
        settingChanged(setting, d->settings.value(setting));
    }
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
    }
}

void PowerSettings::on_suspendSpin_valueChanged(int arg1) {
    d->settings.setValue("Power/timeouts.suspend.value", arg1);
}

void PowerSettings::on_suspendUnit_currentIndexChanged(int index) {
    d->settings.setValue("Power/timeouts.suspend.unit", d->timeoutUnits.value(index));
}
