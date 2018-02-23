#ifndef QTMVVM_SETTINGSVIEWMODEL_P_H
#define QTMVVM_SETTINGSVIEWMODEL_P_H

#include "qtmvvmsettingscore_global.h"
#include "settingsviewmodel.h"

namespace QtMvvm {

class SettingsViewModelPrivate
{
public:
	ISettingsSetupLoader *setupLoader = nullptr;

	QSettings *settings = nullptr;
	QString setupFile;

	SettingsElements::Setup currentSetup;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_P_H
