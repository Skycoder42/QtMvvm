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
class Q_MVVMDATASYNCCORE_EXPORT DataSyncViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QtDataSync::SyncManager* syncManager READ syncManager NOTIFY syncManagerChanged)
	Q_PROPERTY(QtDataSync::AccountManager* accountManager READ accountManager NOTIFY accountManagerChanged)

	Q_PROPERTY(ColorMap colorMap READ colorMap WRITE setColorMap RESET resetColorMap NOTIFY colorMapChanged)
	Q_PROPERTY(QString statusString READ statusString NOTIFY statusStringChanged)

	Q_PROPERTY(QtMvvm::AccountModel* accountModel READ accountModel CONSTANT)
	Q_PROPERTY(QSortFilterProxyModel* sortedModel READ sortedModel CONSTANT)

public:
	typedef QMap<QtDataSync::SyncManager::SyncState, QColor> ColorMap;

	static const QString paramSetup;
	static const QString paramReplicaNode;

	static QVariantHash showParams(const QString &setup);
	static QVariantHash showParams(QRemoteObjectNode *node);

	Q_INVOKABLE explicit DataSyncViewModel(QObject *parent = nullptr);
	~DataSyncViewModel();

	QtDataSync::SyncManager* syncManager() const;
	QtDataSync::AccountManager* accountManager() const;
	ColorMap colorMap() const;
	QString statusString() const;

	AccountModel *accountModel() const;
	QSortFilterProxyModel* sortedModel() const;

	Q_INVOKABLE static QString formatFingerPrint(const QByteArray &fingerPrint);

public Q_SLOTS:
	void syncOrConnect();
	void showDeviceInfo();
	void startExport();
	void startImport();
	void performReset();
	void changeRemote();
	void startNetworkExchange();

	void removeDevice(int sortedIndex);

	void setColorMap(ColorMap colorMap);
	void resetColorMap();

private Q_SLOTS:
	void showImportDialog(QtDataSync::LoginRequest request);
	void showImportAccepted();
	void showAccessGranted(const QUuid &id);
	void triggerGranted(const QList<QtDataSync::DeviceInfo> &devices);

Q_SIGNALS:
	void syncManagerChanged(QtDataSync::SyncManager* syncManager);
	void accountManagerChanged(QtDataSync::AccountManager* accountManager);
	void colorMapChanged(ColorMap colorMap);
	void statusStringChanged();

	void ready();

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private:
	QScopedPointer<DataSyncViewModelPrivate> d;
};

}

#endif // QTMVVM_DATASYNCVIEWMODEL_H
