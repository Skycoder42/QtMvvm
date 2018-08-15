#include "qtmvvmdatasynccore_plugin.h"

#include <QtQml>

#include <QtMvvmDataSyncCore/AccountModel>
#include <QtMvvmDataSyncCore/ExchangeDevicesModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>
#include <QtMvvmDataSyncCore/NetworkExchangeViewModel>
#include <QtMvvmDataSyncCore/DataSyncSettingsViewModel>
#include <QtMvvmDataSyncCore/DataSyncSettingsEntry>

#include <QtMvvmDataSyncCore/private/identityeditviewmodel_p.h>
#include <QtMvvmDataSyncCore/private/changeremoteviewmodel_p.h>
#include <QtMvvmDataSyncCore/private/exportsetupviewmodel_p.h>

QtMvvmDataSyncCoreDeclarativeModule::QtMvvmDataSyncCoreDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin(parent)
{}

void QtMvvmDataSyncCoreDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtMvvm.DataSync.Core") == 0);

	//Version 1.0
	qmlRegisterType<QtMvvm::AccountModel>(uri, 1, 0, "AccountModel");
	qmlRegisterType<QtMvvm::ExchangeDevicesModel>(uri, 1, 0, "ExchangeDevicesModel");

	qmlRegisterUncreatableType<QtMvvm::DataSyncViewModel>(uri, 1, 0, "DataSyncViewModel", QStringLiteral("ViewModels cannot be created from QML"));
	qmlRegisterUncreatableType<QtMvvm::NetworkExchangeViewModel>(uri, 1, 0, "NetworkExchangeViewModel", QStringLiteral("ViewModels cannot be created from QML"));

	//private models
	qmlRegisterUncreatableType<QtMvvm::IdentityEditViewModel>(uri, 1, 0, "PIdentityEditViewModel", QStringLiteral("ViewModels cannot be created from QML"));
	qmlRegisterUncreatableType<QtMvvm::ChangeRemoteViewModel>(uri, 1, 0, "PChangeRemoteViewModel", QStringLiteral("ViewModels cannot be created from QML"));
	qmlRegisterUncreatableType<QtMvvm::ExportSetupViewModel>(uri, 1, 0, "PExportSetupViewModel", QStringLiteral("ViewModels cannot be created from QML"));

	//Version 1.1
	qmlRegisterUncreatableType<QtMvvm::DataSyncSettingsEntry>(uri, 1, 1, "DataSyncSettingsEntry", QStringLiteral("Q_GADGETs cannot be created from QML"));
	qmlRegisterUncreatableType<QtMvvm::DataSyncSettingsViewModel>(uri, 1, 1, "DataSyncSettingsViewModel", QStringLiteral("ViewModels cannot be created from QML"));

	// Check to make sure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 1, "QML module version needs to be updated");
}
