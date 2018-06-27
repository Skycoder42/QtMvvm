#ifndef QTMVVM_QQMLCOREAPP_H
#define QTMVVM_QQMLCOREAPP_H

#include <QtCore/QObject>

#include <QtMvvmCore/CoreApp>

namespace QtMvvm {

class QQmlCoreApp : public QObject
{
	Q_OBJECT

public:
	explicit QQmlCoreApp(QObject *parent = nullptr);

	Q_INVOKABLE QtMvvm::MessageResult *showDialog(const QtMvvm::MessageConfig &config);
	Q_INVOKABLE QVariant safeCastInputType(const QString &type, const QVariant &value);

public Q_SLOTS:
	void show(const QString &viewModelName, const QVariantHash &params = {}, QtMvvm::ViewModel *parentViewModel = nullptr);

Q_SIGNALS:
	void appStarted();
};

}

#endif // QTMVVM_QQMLCOREAPP_H
