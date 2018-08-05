#ifndef QTMVVM_COREAPP_H
#define QTMVVM_COREAPP_H

#include <type_traits>

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
//! A logicaly application object to drive the mvvm application from the core part
class Q_MVVMCORE_EXPORT CoreApp : public QObject
{
	Q_OBJECT

public:
	//! Default Constructor
	explicit CoreApp(QObject *parent = nullptr);
	~CoreApp() override;

	//! Returns the current CoreApp instance
	static CoreApp *instance();
	//! Disables the automatic startup of the CoreApp
	static void disableAutoBoot();

	//! Registers this instance as the CoreApp instance
	void registerApp();

	//! Show a new ViewModel by its type
	template <typename TViewModel>
	static inline void show(const QVariantHash &params = {}, QPointer<ViewModel> parentViewModel = nullptr);
	//! @copydoc CoreApp::show(const char *, const QVariantHash &, QPointer<ViewModel>);
	static void show(const char *viewModelName, const QVariantHash &params = {}); //MAJOR merge methods
	//! Show a new ViewModel by its name
	static void show(const char *viewModelName, const QVariantHash &params, QPointer<ViewModel> parentViewModel);
	//! @copydoc CoreApp::show(const QMetaObject *, const QVariantHash &, QPointer<ViewModel>);
	static void show(const QMetaObject *viewModelMetaObject, const QVariantHash &params = {}); //MAJOR merge methods
	//! Show a new ViewModel by its metaobject
	static void show(const QMetaObject *viewModelMetaObject, const QVariantHash &params, QPointer<ViewModel> parentViewModel);

	//! Show a basic dialog
	static MessageResult *showDialog(const MessageConfig &config);

	//! Safely casts a value of the given edit type to the corresponding variant type
	static QVariant safeCastInputType(const QByteArray &type, const QVariant &value);
	//! Register a type to be used as variant type for the given edit type
	template <typename T>
	static void registerInputTypeMapping(const QByteArray &type);
	//! @copybrief CoreApp::registerInputTypeMapping(const QByteArray &)
	static void registerInputTypeMapping(const QByteArray &type, int targetType);

	//! Returns the currently used presenter
	static IPresenter *presenter();

public Q_SLOTS:
	//! Boots up the app and starts the mvvm presenting
	void bootApp();

Q_SIGNALS:
	//! Is emitted once the app has successfully started
	void appStarted(QPrivateSignal);

protected:
	//! Is called as part of the app registration, before any view setups
	virtual void performRegistrations();
	//! Is called as part of the app start/boot, after all the view setups to start the app
	virtual int startApp(const QStringList &arguments) = 0;
	//! Is called when the app is about to quit
	virtual void closeApp();

	//! Can be used to automatically handle help and version commandline arguments
	bool autoParse(QCommandLineParser &parser, const QStringList &arguments);

private:
	friend class QtMvvm::CoreAppPrivate;
	QScopedPointer<CoreAppPrivate> d;

	Q_DECL_DEPRECATED static void showImp(const QMetaObject *metaObject, const QVariantHash &params);
	static void showImp(const QMetaObject *metaObject, const QVariantHash &params, QPointer<ViewModel> parentViewModel);
};

template<typename TViewModel>
inline void CoreApp::show(const QVariantHash &params, QPointer<ViewModel> parentViewModel)
{
	static_assert(std::is_base_of<ViewModel, TViewModel>::value, "TViewModel must extend QtMvvm::ViewModel");
	showImp(&TViewModel::staticMetaObject, params, std::move(parentViewModel));
}

template<typename T>
void CoreApp::registerInputTypeMapping(const QByteArray &type)
{
	registerInputTypeMapping(type, qMetaTypeId<T>());
}

}

//! Registers you custom CoreApp class as CoreApp to be used
#define QTMVVM_REGISTER_CORE_APP(T) namespace {\
	void __setup_ ## T ## _hook() { \
		static_assert(std::is_base_of<QtMvvm::CoreApp, T>::value, "QTMVVM_REGISTER_CORE_APP must be used with a class that extends QtMvvm::CoreApp"); \
		auto app = new T(nullptr); \
		app->registerApp(); \
	} \
	} \
	Q_COREAPP_STARTUP_FUNCTION(__setup_ ## T ## _hook)

//! A define as shortcut to the CoreApp. Can be redefined to cast to your custom class type
#define coreApp QtMvvm::CoreApp::instance()

//!@file coreapp.h The Header that defines the QtMvvm::CoreApp class
#endif // QTMVVM_COREAPP_H
