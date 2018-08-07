#ifndef QTMVVM_SETTINGSVIEWMODEL_P_H
#define QTMVVM_SETTINGSVIEWMODEL_P_H

#include "qtmvvmcore_global.h"
#include "settingsviewmodel.h"

namespace QtMvvm {

class SettingsViewModelPrivate
{
	Q_DISABLE_COPY(SettingsViewModelPrivate)

public:
	SettingsViewModelPrivate() = default;

	ISettingsSetupLoader *setupLoader = nullptr;
	ISettingsAccessor *accessor = nullptr;

	QString setupFile;

	SettingsElements::Setup currentSetup;
};

}

#endif // QTMVVM_SETTINGSVIEWMODEL_P_H
