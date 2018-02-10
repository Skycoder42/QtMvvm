#ifndef SAMPLECOREAPP_H
#define SAMPLECOREAPP_H

#include <QtMvvmCore/CoreApp>

class SampleCoreApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	SampleCoreApp(QObject *parent = nullptr);

protected:
	int startApp(const QStringList &arguments) override;
};

#endif // SAMPLECOREAPP_H
