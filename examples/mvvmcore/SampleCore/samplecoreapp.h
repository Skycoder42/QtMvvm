#ifndef SAMPLECOREAPP_H
#define SAMPLECOREAPP_H

#include <QtMvvmCore/CoreApp>

#include "samplecore_global.h"

class SAMPLECORESHARED_EXPORT SampleCoreApp : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	SampleCoreApp(QObject *parent = nullptr);

protected:
	int startApp(const QStringList &arguments) override;
};

#endif // SAMPLECOREAPP_H
