#include "samplecoreapp.h"
#include "sampleviewmodel.h"
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtMvvmCore/QtMvvmCoreVersion>
#include <QtDataSync/Setup>

SampleCoreApp::SampleCoreApp(QObject *parent) :
	CoreApp(parent)
{
	QCoreApplication::setApplicationName(QStringLiteral("QtMvvmDataSyncSample"));
	QCoreApplication::setApplicationVersion(QStringLiteral(QTMVVMCORE_VERSION_STR));
	QCoreApplication::setOrganizationName(QStringLiteral("Skycoder42"));
}

void SampleCoreApp::performRegistrations()
{
	qRegisterMetaType<SampleData>();
	QLoggingCategory::setFilterRules(QStringLiteral("qtmvvm.debug=true"));
}

int SampleCoreApp::startApp(const QStringList &arguments)
{
	auto setup = QtDataSync::DefaultSetup;
	if(arguments.size() > 1)
		setup = arguments.value(1);

	try {
		QtDataSync::Setup()
				.setRemoteConfiguration(QUrl(QStringLiteral("ws://localhost:4242")))
				.create(setup);
		show<SampleViewModel>();
		return EXIT_SUCCESS;
	} catch (QException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}
}
