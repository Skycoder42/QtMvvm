#ifndef QTMVVM_COREAPP_H
#define QTMVVM_COREAPP_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qcoreapplication.h>
class QCommandLineParser;

#include "QtMvvmCore/qtmvvmcore_global.h"
#include "QtMvvmCore/viewmodel.h"
#include "QtMvvmCore/ipresenter.h"

namespace QtMvvm {

class CoreAppPrivate;
class Q_MVVMCORE_EXPORT CoreApp : public QObject
{
	Q_OBJECT

public:
	explicit CoreApp(QObject *parent = nullptr);
	~CoreApp();

	static CoreApp *instance();
	static void setMainPresenter(IPresenter *presenter);
	static void disableAutoBoot();

	void registerApp();

public Q_SLOTS:
	void bootApp();

protected:
	virtual void performRegistrations();
	virtual int startApp(const QStringList &arguments) = 0;
	virtual void closeApp();

	bool autoParse(QCommandLineParser &parser, const QStringList &arguments);
	template <typename TViewModel>
	inline void show(const QVariantHash &params = {}) const;

private:
	friend class QtMvvm::CoreAppPrivate;
	QScopedPointer<CoreAppPrivate> d;
};

template<typename TViewModel>
inline void CoreApp::show(const QVariantHash &params) const
{
	ViewModel::show<TViewModel>(params);
}

}

#define QTMVVM_REGISTER_CORE_APP(T) \
	static void _setup_ ## T ## _hook() { \
		auto app = new T(nullptr); \
		app->registerApp(); \
	} \
	Q_COREAPP_STARTUP_FUNCTION(_setup_ ## T ## _hook)

#define coreApp CoreApp::instance()

#endif // QTMVVM_COREAPP_H
