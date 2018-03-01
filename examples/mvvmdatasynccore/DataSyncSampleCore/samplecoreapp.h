#ifndef SAMPLECOREAPP_H
#define SAMPLECOREAPP_H

#include <QtMvvmCore/CoreApp>

class SampleCoreApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	SampleCoreApp(QObject *parent = nullptr);

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;
};

#undef coreApp
#define coreApp static_cast<SampleCoreApp*>(QtMvvm::CoreApp::instance())

#endif // SAMPLECOREAPP_H
