#ifndef QTMVVM_DATASYNCVIEWMODEL_H
#define QTMVVM_DATASYNCVIEWMODEL_H

#include <QtCore/qmap.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qsortfilterproxymodel.h>

#include <QtGui/qcolor.h>

#include <QtMvvmCore/viewmodel.h>

#include <QtDataSync/syncmanager.h>
#include <QtDataSync/accountmanager.h>

#include "QtMvvmDataSyncCore/qtmvvmdatasynccore_global.h"
#include "QtMvvmDataSyncCore/accountmodel.h"

namespace QtMvvm {

class DataSyncViewModelPrivate;
//! A viewmodel to show a overview of the datasync status and account
class Q_MVVMDATASYNCCORE_EXPORT DataSyncViewModel : public ViewModel
{
	Q_OBJECT

	//! The sync manager the viewmodel operates on
	Q_PROPERTY(QtDataSync::SyncManager* syncManager READ syncManager NOTIFY syncManagerChanged)
	//! The account manager the viewmodel operates on
	Q_PROPERTY(QtDataSync::AccountManager* accountManager READ accountManager NOTIFY accountManagerChanged)

	//! A mapping of sync states to text colors
	Q_PROPERTY(ColorMap colorMap READ colorMap WRITE setColorMap RESET resetColorMap NOTIFY colorMapChanged)
	//! A styles string describing the current sync status
	Q_PROPERTY(QString statusString READ statusString NOTIFY statusStringChanged)

	//! The account model to be used to display account devices
	Q_PROPERTY(QtMvvm::AccountModel* accountModel READ accountModel CONSTANT)
	//! A sorted version of the account model
	Q_PROPERTY(QSortFilterProxyModel* sortedModel READ sortedModel CONSTANT)

public:
	//! A mapping from sync states to a text color
	using ColorMap = QMap<QtDataSync::SyncManager::SyncState, QColor>;

	//! The parameter for a setup name for the onInit() method
	static const QString paramSetup;
	//! The parameter for a QRemoteObjectNode for the onInit() method
	static const QString paramReplicaNode;

	//! Generates show parameter to show a datasync viewmodel via ViewModel::show
	static QVariantHash showParams(const QString &setup);
	//! @copybrief showParams(const QString &)
	static QVariantHash showParams(QRemoteObjectNode *node);

	//! Invokable constructor
	Q_INVOKABLE explicit DataSyncViewModel(QObject *parent = nullptr);
	~DataSyncViewModel() override;

	//! @readAcFn{DataSyncViewModel::syncManager}
	QtDataSync::SyncManager* syncManager() const;
	//! @readAcFn{DataSyncViewModel::accountManager}
	QtDataSync::AccountManager* accountManager() const;
	//! @readAcFn{DataSyncViewModel::colorMap}
	ColorMap colorMap() const;
	//! @readAcFn{DataSyncViewModel::statusString}
	QString statusString() const;
	//! @readAcFn{DataSyncViewModel::accountModel}
	AccountModel *accountModel() const;
	//! @readAcFn{DataSyncViewModel::sortedModel}
	QSortFilterProxyModel* sortedModel() const;

	//! Converts a binary fingerprint to a human readable string
	Q_INVOKABLE static QString formatFingerPrint(const QByteArray &fingerPrint);

public Q_SLOTS:
	//! Synchronizes or reconnects with the server
	void syncOrConnect();
	//! Show and edit information about this current device
	void showDeviceInfo();
	//! Start an export to a file
	void startExport();
	//! Start an import from a file
	void startImport();
	//! Show a confirmation dialog to reset the account (and then reset)
	void performReset();
	//! Show a dialog to change the remote server connected to
	void changeRemote();
	//! Start a network exchange by showing the NetworkExchangeViewModel
	void startNetworkExchange();

	//! Remove the with the given index in the sorted model from the account
	void removeDevice(int sortedIndex);

	//! @writeAcFn{DataSyncViewModel::colorMap}
	void setColorMap(ColorMap colorMap);
	//! @resetAcFn{DataSyncViewModel::colorMap}
	void resetColorMap();

Q_SIGNALS:
	//! @notifyAcFn{DataSyncViewModel::syncManager}
	void syncManagerChanged(QtDataSync::SyncManager* syncManager, QPrivateSignal);
	//! @notifyAcFn{DataSyncViewModel::accountManager}
	void accountManagerChanged(QtDataSync::AccountManager* accountManager, QPrivateSignal);
	//! @notifyAcFn{DataSyncViewModel::colorMap}
	void colorMapChanged(ColorMap colorMap, QPrivateSignal);
	//! @notifyAcFn{DataSyncViewModel::statusString}
	void statusStringChanged(QPrivateSignal);

	//! Is emitted as soon as all the datasync managers have been initialized
	void ready(QPrivateSignal);

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private Q_SLOTS:
	void showImportDialog(QtDataSync::LoginRequest request);
	void showImportAccepted();
	void showAccessGranted(const QUuid &id);
	void triggerGranted(const QList<QtDataSync::DeviceInfo> &devices);

private:
	QScopedPointer<DataSyncViewModelPrivate> d;
};

}

#endif // QTMVVM_DATASYNCVIEWMODEL_H
