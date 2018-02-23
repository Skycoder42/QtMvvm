#include "qtmvvmsettingscore_global.h"
#include <QtCore/QCoreApplication>
#include <QtMvvmCore/ServiceRegistry>
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include "settingssetuploader_p.h"

using namespace QtMvvm;

namespace {

void qtMvvmSettingsCoreStartup()
{
	QtMvvm::registerInterfaceConverter<ISettingsSetupLoader>();
	try {
		ServiceRegistry::instance()->registerInterface<ISettingsSetupLoader, SettingsSetupLoader>(true);
	} catch(ServiceExistsException &e) {
		logDebug() << "Unable to register default ISettingsSetupLoader with error:" << e.what();
	}
}

}

Q_COREAPP_STARTUP_FUNCTION(qtMvvmSettingsCoreStartup)
