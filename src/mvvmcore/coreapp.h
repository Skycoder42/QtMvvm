#ifndef QTMVVM_COREAPP_H
#define QTMVVM_COREAPP_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qcoreapplication.h>
class QCommandLineParser;

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/viewmodel.h"
#include "QtMvvmCore/ipresenter.h"
#include "QtMvvmCore/message.h"

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

	template <typename TViewModel>
	static inline void show(const QVariantHash &params = {});
	static void show(const char *viewModelName, const QVariantHash &params = {});
	static void show(const QMetaObject *viewMetaObject, const QVariantHash &params = {});

	static MessageResult *showDialog(const MessageConfig &config);

public Q_SLOTS:
	void bootApp();

Q_SIGNALS:
	void appStarted();

protected:
	virtual void performRegistrations();
	virtual int startApp(const QStringList &arguments) = 0;
	virtual void closeApp();

	bool autoParse(QCommandLineParser &parser, const QStringList &arguments);

private:
	friend class QtMvvm::CoreAppPrivate;
	QScopedPointer<CoreAppPrivate> d;

	static void showImp(const QMetaObject *metaObject, const QVariantHash &params);
};

template<typename TViewModel>
inline void CoreApp::show(const QVariantHash &params)
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	ViewModel::showImp(&TViewModel::staticMetaObject, params, nullptr);
}

}

#define QTMVVM_REGISTER_CORE_APP(T) \
	static void _setup_ ## T ## _hook() { \
		auto app = new T(nullptr); \
		app->registerApp(); \
	} \
	Q_COREAPP_STARTUP_FUNCTION(_setup_ ## T ## _hook)

#define coreApp QtMvvm::CoreApp::instance()

#endif // QTMVVM_COREAPP_H
