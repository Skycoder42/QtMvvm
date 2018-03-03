#include "qtmvvmdatasynccore_plugin.h"

#include <QtQml>

#include <QtMvvmDataSyncCore/AccountModel>
#include <QtMvvmDataSyncCore/ExchangeDevicesModel>
#include <QtMvvmDataSyncCore/DataSyncViewModel>
#include <QtMvvmDataSyncCore/NetworkExchangeViewModel>

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

	// Check to make shure no module update is forgotten
	static_assert(VERSION_MAJOR == 1 && VERSION_MINOR == 0, "QML module version needs to be updated");
}
