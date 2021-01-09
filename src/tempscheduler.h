/**
 * Copyright (C) Francesco Fusco. All rights reserved.
 * License: https://github.com/Fushko/gammy#license
 */

#ifndef TEMPSCHEDULER_H
#define TEMPSCHEDULER_H

#include <QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "utils.h"
#include "defs.h"
#include "component.h"

class Mediator;

namespace Ui {
class TempScheduler;
}

class TempScheduler : public QDialog
{
	Q_OBJECT

public:
	explicit TempScheduler(IMediator *mediator = nullptr);
	~TempScheduler();

private slots:
	void on_buttonBox_accepted();
	void on_sunriseBox_valueChanged(int);
	void on_sunsetBox_valueChanged(int);
	void on_sunriseBox_timeChanged(const QTime &time);
	void on_sunsetBox_timeChanged(const QTime &time);
	void on_doubleSpinBox_valueChanged(double arg1);

private:
	Ui::TempScheduler *ui;
	int sunrise_h, sunrise_m;
	int sunset_h, sunset_m;
	int high_temp;
	int low_temp;
	double adaptation_time_m;

	static constexpr auto api_url    = "https://api.sunrise-sunset.org/?json";
	static constexpr auto api_format = "h:mm:ss AP";

	IMediator *mediator;
	QNetworkAccessManager *manager;

	void setDates();
};

#endif // TEMPSCHEDULER_H
