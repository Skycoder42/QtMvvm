#ifndef QTMVVM_QQMLCOREAPP_H
#define QTMVVM_QQMLCOREAPP_H

#include <QtCore/QObject>

#include <QtMvvmCore/CoreApp>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Core {

/*! @brief A QML singleton to access some QtMvvm::CoreApp functionality
 *
 * @extends QtQml.QtObject
 * @since 1.1
 *
 * @sa QtMvvm::CoreApp
 */
class CoreApp
#else
namespace QtMvvm {

class QQmlCoreApp : public QObject
#endif
{
	Q_OBJECT

public:
	//! @private
	explicit QQmlCoreApp(QObject *parent = nullptr);

	//! @copydoc QtMvvm::CoreApp::showDialog
	Q_INVOKABLE QtMvvm::MessageResult *showDialog(const QtMvvm::MessageConfig &config);
	//! @copydoc QtMvvm::CoreApp::safeCastInputType
	Q_INVOKABLE QVariant safeCastInputType(const QString &type, const QVariant &value);

public Q_SLOTS:
	//! @copydoc QtMvvm::CoreApp::show(const char *, const QVariantHash &, QPointer<ViewModel>);
	void show(const QString &viewModelName, const QVariantHash &params = {}, QtMvvm::ViewModel *parentViewModel = nullptr);

Q_SIGNALS:
	//! @copydoc QtMvvm::CoreApp::appStarted
	void appStarted();
};

}

#endif // QTMVVM_QQMLCOREAPP_H
