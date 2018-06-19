#ifndef QTMVVM_NETWORKEXCHANGEVIEWMODEL_H
#define QTMVVM_NETWORKEXCHANGEVIEWMODEL_H

#include <QtCore/qscopedpointer.h>
#include <QtCore/qsortfilterproxymodel.h>

#include <QtMvvmCore/viewmodel.h>

#include <QtDataSync/userexchangemanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"
#include "QtMvvmDataSyncCore/exchangedevicesmodel.h"

namespace QtMvvm {

class NetworkExchangeViewModelPrivate;
//! A viewmodel to show a dialog to exchange account data via the QtDataSync::UserExchangeManager
class Q_MVVMDATASYNCCORE_EXPORT NetworkExchangeViewModel : public ViewModel
{
	Q_OBJECT

	//! The user exchange manager the viewmodel operates on
	Q_PROPERTY(QtDataSync::UserExchangeManager* userExchangeManager READ userExchangeManager NOTIFY userExchangeManagerChanged)
	//! The port to run the exchange on
	Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
	//! The name of your current device
	Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
	//! Start or stop the network exchange
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

	//! The devices model to be used to display found exchange devices
	Q_PROPERTY(ExchangeDevicesModel* deviceModel READ deviceModel CONSTANT)
	//! A sorted version of the devices model
	Q_PROPERTY(QSortFilterProxyModel* sortedModel READ sortedModel CONSTANT)

public:
	//! The parameter for a setup name for the onInit() method
	static const QString paramSetup;
	//! The parameter for an QtDataSync::AccountManager for the onInit() method
	static const QString paramAccountManager;

	//! Generates show parameter to show a network exchange viewmodel via ViewModel::show
	static QVariantHash showParams(const QString &setup);
	//! @copybrief showParams(const QString &)
	static QVariantHash showParams(QtDataSync::AccountManager *accountManager);

	//! Invokable constructor
	Q_INVOKABLE explicit NetworkExchangeViewModel(QObject *parent = nullptr);
	~NetworkExchangeViewModel() override;

	//! @readAcFn{NetworkExchangeViewModel::userExchangeManager}
	QtDataSync::UserExchangeManager* userExchangeManager() const;
	//! @readAcFn{NetworkExchangeViewModel::port}
	quint16 port() const;
	//! @readAcFn{NetworkExchangeViewModel::deviceName}
	QString deviceName() const;
	//! @readAcFn{NetworkExchangeViewModel::active}
	bool isActive() const;
	Q_DECL_DEPRECATED bool active() const;
	//! @readAcFn{NetworkExchangeViewModel::deviceModel}
	ExchangeDevicesModel* deviceModel() const;
	//! @readAcFn{NetworkExchangeViewModel::sortedModel}
	QSortFilterProxyModel* sortedModel() const;

public Q_SLOTS:
	//! Start en export of user data to the device with the given index in the sorted model
	void exportTo(int sortedIndex);

	//! @writeAcFn{NetworkExchangeViewModel::port}
	void setPort(quint16 port);
	//! @writeAcFn{NetworkExchangeViewModel::deviceName}
	void setDeviceName(QString deviceName);
	//! @writeAcFn{NetworkExchangeViewModel::active}
	void setActive(bool active);

Q_SIGNALS:
	//! Is emitted as soon as all the user exchange manager has been initialized
	void ready(QPrivateSignal);

	//! @notifyAcFn{NetworkExchangeViewModel::userExchangeManager}
	void userExchangeManagerChanged(QtDataSync::UserExchangeManager* userExchangeManager, QPrivateSignal);
	//! @notifyAcFn{NetworkExchangeViewModel::port}
	void portChanged(quint16 port, QPrivateSignal);
	//! @notifyAcFn{NetworkExchangeViewModel::deviceName}
	void deviceNameChanged(QString deviceName, QPrivateSignal);
	//! @notifyAcFn{NetworkExchangeViewModel::active}
	void activeChanged(bool active, QPrivateSignal);

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private Q_SLOTS:
	void exchangeError(const QString &errorString);
	void newUserData(const QtDataSync::UserInfo &userInfo, bool trusted);

private:
	QScopedPointer<NetworkExchangeViewModelPrivate> d;
};

}

#endif // QTMVVM_NETWORKEXCHANGEVIEWMODEL_H
