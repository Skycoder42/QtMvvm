#ifndef QTMVVM_EXPORTSETUPVIEWMODEL_P_H
#define QTMVVM_EXPORTSETUPVIEWMODEL_P_H

#include <tuple>

#include <QtMvvmCore/ViewModel>

#include "qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class Q_MVVMDATASYNCCORE_EXPORT ExportSetupViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString label READ label NOTIFY labelChanged)
	Q_PROPERTY(bool trusted READ trusted WRITE setTrusted NOTIFY trustedChanged)
	Q_PROPERTY(bool includeServer READ includeServer WRITE setIncludeServer NOTIFY includeServerChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

	Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
	static QVariantHash showParams(const QString &label);
	static std::tuple<bool, bool, QString> result(const QVariant &data); //(trusted, includeServer, password)

	Q_INVOKABLE explicit ExportSetupViewModel(QObject *parent = nullptr);

	QString label() const;
	bool trusted() const;
	bool includeServer() const;
	QString password() const;
	bool isValid() const;

public Q_SLOTS:
	bool completeSetup();

	void setTrusted(bool trusted);
	void setIncludeServer(bool includeServer);
	void setPassword(QString password);

Q_SIGNALS:
	void labelChanged(QString label);
	void trustedChanged(bool trusted);
	void includeServerChanged(bool includeServer);
	void passwordChanged(QString password);
	void validChanged();

protected:
	void onInit(const QVariantHash &params) override;

private:
	QString _label;
	bool _trusted = false;
	QString _password;
	bool _includeServer = false;
};

}

#endif // QTMVVM_EXPORTSETUPVIEWMODEL_P_H
