#ifndef QTMVVM_COREAPP_H
#define QTMVVM_COREAPP_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
class QCommandLineParser;

#include "QtMvvmCore/qtmvvmcore_global.h"

namespace QtMvvm {

class CoreAppPrivate;
class Q_MVVMCORE_EXPORT CoreApp : public QObject
{
	Q_OBJECT

public:
	explicit CoreApp(QObject *parent = nullptr);
	~CoreApp();

	static CoreApp *instance();
	static void disableAutoBoot();

	void registerApp();

public Q_SLOTS:
	void bootApp();

protected:
	virtual int startApp(const QStringList &arguments) = 0;
	virtual void closeApp();

	bool autoParse(QCommandLineParser &parser, const QStringList &arguments);

private:
	QScopedPointer<CoreAppPrivate> d;
};

}

#define QTMVVM_REGISTER_CORE_APP(T) \
	static void _setup_ ## T ## _hook() { \
		auto app = new T(nullptr); \
		app->registerApp(); \
	} \
	Q_COREAPP_STARTUP_FUNCTION(_setup_ ## T ## _hook)

#define coreApp CoreApp::instance()

#endif // QTMVVM_COREAPP_H
