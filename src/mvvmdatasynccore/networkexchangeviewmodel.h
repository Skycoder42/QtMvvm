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
class Q_MVVMDATASYNCCORE_EXPORT NetworkExchangeViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
	Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
	Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

	Q_PROPERTY(ExchangeDevicesModel* deviceModel READ deviceModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel* sortedModel READ sortedModel CONSTANT)

public:
	static const QString paramSetup;
	static const QString paramAccountManager;

	static QVariantHash showParams(const QString &setup);
	static QVariantHash showParams(QtDataSync::AccountManager *accountManager);

	Q_INVOKABLE explicit NetworkExchangeViewModel(QObject *parent = nullptr);
	~NetworkExchangeViewModel();

	quint16 port() const;
	QString deviceName() const;
	bool active() const;
	ExchangeDevicesModel* deviceModel() const;
	QSortFilterProxyModel* sortedModel() const;

public Q_SLOTS:
	void exportTo(int sortedIndex);

	void setPort(quint16 port);
	void setDeviceName(QString deviceName);
	void setActive(bool active);

Q_SIGNALS:
	void ready();

	void portChanged(quint16 port);
	void deviceNameChanged(QString deviceName);
	void activeChanged(bool active);

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
