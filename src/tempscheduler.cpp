/**
 * Copyright (C) Francesco Fusco. All rights reserved.
 * License: https://github.com/Fushko/gammy#license
 */

#include "tempscheduler.h"
#include "ui_tempscheduler.h"
#include "cfg.h"
#include "mediator.h"

TempScheduler::TempScheduler(IMediator *m)
        : ui(new Ui::TempScheduler), mediator(m),
          manager(new QNetworkAccessManager())
{
	ui->setupUi(this);

	ui->tempStartBox->setValue(low_temp = cfg["temp_low"]);
	ui->tempEndBox->setValue(high_temp = cfg["temp_high"]);
	ui->doubleSpinBox->setValue(adaptation_time_m = cfg["temp_speed"]);

	const auto sunrise = cfg["temp_sunrise"].get<std::string>();
	sunrise_h = std::stoi(sunrise.substr(0, 2));
	sunrise_m = std::stoi(sunrise.substr(3, 2));

	const auto sunset = cfg["temp_sunset"].get<std::string>();
	sunset_h = std::stoi(sunset.substr(0, 2));
	sunset_m = std::stoi(sunset.substr(3, 2));

	ui->sunsetBox->setTime(QTime(sunset_h, sunset_m));
	ui->sunriseBox->setTime(QTime(sunrise_h, sunrise_m));

	connect(manager, &QNetworkAccessManager::finished, this, [&] (QNetworkReply *reply) {

		const auto tmp      = reply->readAll().toStdString();
		nlohmann::json data = nlohmann::json::parse(tmp);
		const auto status   = data["status"].get<std::string>();

		if (status != "OK") {
			LOGE << "Error fetching sunrise/sunset.";
			return;
		}

		const auto res     = data["results"];
		const auto sunrise = QString::fromStdString(res["sunrise"].get<std::string>());
		const auto sunset  = QString::fromStdString(res["sunset"].get<std::string>());
		ui->sunriseBox->setTime(QTime::fromString(sunrise, api_format));
		ui->sunsetBox->setTime(QTime::fromString(sunset, api_format));
	});

	manager->get(QNetworkRequest(QUrl("https://api.sunrise-sunset.org/json?lat=53.917281&lng=5.819661")));
}

void TempScheduler::on_buttonBox_accepted()
{
	QTime t_sunrise = QTime(sunrise_h, sunrise_m);
	QTime t_sunset  = QTime(sunset_h, sunset_m);
	QTime t_sunset_adaptated = t_sunset.addSecs(-adaptation_time_m * 60);

	if (t_sunrise >= t_sunset_adaptated) {
		LOGW << "Sunrise time is later or equal to sunset - adaptation. Setting to sunset - adaptation.";
		t_sunrise = t_sunset_adaptated;
	}

	cfg["temp_sunset"]  = t_sunset.toString().toStdString();
	cfg["temp_sunrise"] = t_sunrise.toString().toStdString();
	cfg["temp_high"]    = high_temp;
	cfg["temp_low"]     = low_temp;
	cfg["temp_speed"]   = adaptation_time_m;

	config::write();
	mediator->notify(nullptr, Component::AUTO_TEMP_TOGGLED);
}

void TempScheduler::on_sunsetBox_valueChanged(int val)
{
	high_temp = val;
}

void TempScheduler::on_sunriseBox_valueChanged(int val)
{
	low_temp = val;
}

void TempScheduler::on_sunsetBox_timeChanged(const QTime &time)
{
	sunset_h = time.hour();
	sunset_m = time.minute();
}

void TempScheduler::on_sunriseBox_timeChanged(const QTime &time)
{
	sunrise_h = time.hour();
	sunrise_m = time.minute();
}

void TempScheduler::on_doubleSpinBox_valueChanged(double arg1)
{
	adaptation_time_m = arg1;
}

TempScheduler::~TempScheduler()
{
	delete ui;
}
