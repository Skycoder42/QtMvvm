#ifndef QTMVVM_DATASYNCVIEWMODEL_P_H
#define QTMVVM_DATASYNCVIEWMODEL_P_H

#include <QtCore/QSet>

#include "qtmvvmdatasynccore_global.h"
#include "datasyncviewmodel.h"

namespace QtMvvm {

class DataSyncViewModelPrivate
{
	Q_DISABLE_COPY(DataSyncViewModelPrivate)

public:
	static const quint32 ExportRequestCode = 0xb201;
	static const quint32 ChangeRemoteRequestCode = 0xb202;

	DataSyncViewModelPrivate(DataSyncViewModel *q_ptr);

	DataSyncViewModel *q;
	QtDataSync::SyncManager *syncManager = nullptr;
	QtDataSync::AccountManager *accountManager = nullptr;
	DataSyncViewModel::ColorMap colorMap;
	AccountModel *accountModel;
	QSortFilterProxyModel *sortedModel;

	QSet<QUuid> pendingGrants;

	void performExport(bool trusted, bool includeServer, const QString &password);
	void performImport(bool trusted, const QString &password, const QByteArray &data, bool keepData);
};

}

#endif // QTMVVM_DATASYNCVIEWMODEL_P_H
