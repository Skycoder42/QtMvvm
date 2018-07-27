#include "qtmvvmcore_global.h"
#include "qtmvvm_logging_p.h"
#include "serviceregistry.h"
#include "settingsconfigloader_p.h"

#include <QtCore/QCoreApplication>

namespace {

void qtMvvmCoreStartup()
{
	using namespace QtMvvm;
	registerInterfaceConverter<ISettingsSetupLoader>();
	try {
		ServiceRegistry::instance()->registerInterface<ISettingsSetupLoader, SettingsConfigLoader>(ServiceRegistry::DestroyOnAppDestroy, true);
	} catch(ServiceExistsException &e) {
		logDebug() << "Unable to register default ISettingsSetupLoader with error:" << e.what();
	}
}

}
Q_COREAPP_STARTUP_FUNCTION(qtMvvmCoreStartup)



namespace QtMvvm {

Q_LOGGING_CATEGORY(mvvmLoggingCat, "qtmvvm", QtInfoMsg)

}
