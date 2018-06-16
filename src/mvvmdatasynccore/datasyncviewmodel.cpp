#include "datasyncviewmodel.h"
#include "datasyncviewmodel_p.h"
#include "networkexchangeviewmodel.h"
#include "exportsetupviewmodel_p.h"
#include "changeremoteviewmodel_p.h"
#include "identityeditviewmodel_p.h"

#include <QtCore/QStandardPaths>
#include <QtCore/QFile>

#include <QtRemoteObjects/QRemoteObjectNode>

#include <QtMvvmCore/CoreApp>
#include <QtMvvmCore/Messages>

#include <QtDataSync/SetupDoesNotExistException>

#ifdef Q_OS_ANDROID
#include <contentdevice.h>
#endif

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#define PSIG(x) [this]() { \
	(this->*(x))(QPrivateSignal{}); \
}


using namespace QtMvvm;
using namespace QtDataSync;

const QString DataSyncViewModel::paramSetup(QStringLiteral("setup"));
const QString DataSyncViewModel::paramReplicaNode(QStringLiteral("node"));

QVariantHash DataSyncViewModel::showParams(const QString &setup)
{
	return {
		{paramSetup, setup}
	};
}

QVariantHash DataSyncViewModel::showParams(QRemoteObjectNode *node)
{
	return {
		{paramReplicaNode, QVariant::fromValue(node)}
	};
}

DataSyncViewModel::DataSyncViewModel(QObject *parent) :
	ViewModel(parent),
	d(new DataSyncViewModelPrivate(this))
{
	d->sortedModel->setSourceModel(d->accountModel);
	resetColorMap();
}

DataSyncViewModel::~DataSyncViewModel() = default;

SyncManager *DataSyncViewModel::syncManager() const
{
	return d->syncManager;
}

AccountManager *DataSyncViewModel::accountManager() const
{
	return d->accountManager;
}

DataSyncViewModel::ColorMap DataSyncViewModel::colorMap() const
{
	return d->colorMap;
}

QString DataSyncViewModel::statusString() const
{
	if(!d->syncManager)
		return {};
	auto state = d->syncManager->syncState();

	QString baseText = QStringLiteral("<font color=\"%1\">%2</font>")
					   .arg(d->colorMap.value(state).name());
	switch (state) {
	case SyncManager::Initializing:
		return baseText.arg(tr("Connecting…"));
	case SyncManager::Downloading:
		return baseText.arg(tr("Downloading…"));
	case SyncManager::Uploading:
		return baseText.arg(tr("Uploading…"));
	case SyncManager::Synchronized:
		return baseText.arg(tr("Synchronized"));
	case SyncManager::Error:
		return baseText.arg(tr("Error!"));
	case SyncManager::Disconnected:
		return baseText.arg(tr("Disconnected"));
	default:
		Q_UNREACHABLE();
		return QString();
	}
}

AccountModel *DataSyncViewModel::accountModel() const
{
	return d->accountModel;
}

QSortFilterProxyModel *DataSyncViewModel::sortedModel() const
{
	return d->sortedModel;
}

QString DataSyncViewModel::formatFingerPrint(const QByteArray &fingerPrint)
{
	QByteArrayList res;
	res.reserve(fingerPrint.size());
	for(char c : fingerPrint)
		res.append(QByteArray(1, c).toHex().toUpper());
	return QString::fromUtf8(res.join(':'));
}

void DataSyncViewModel::syncOrConnect()
{
	if(d->syncManager->syncState() == SyncManager::Disconnected ||
	   d->syncManager->syncState() == SyncManager::Error) {
		logDebug() << "Reconnecting to remote";
		d->syncManager->reconnect();
	} else {
		logDebug() << "Synchronizing with remote";
		d->syncManager->synchronize();
	}
}

void DataSyncViewModel::showDeviceInfo()
{
	if(!d->accountManager)
		return;
	show<IdentityEditViewModel>(IdentityEditViewModel::showParams(d->accountManager));
}

void DataSyncViewModel::startExport()
{
	showForResult<ExportSetupViewModel>(DataSyncViewModelPrivate::ExportRequestCode,
										ExportSetupViewModel::showParams(tr("Export account data to file:")));
}

void DataSyncViewModel::startImport()
{
	auto home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	getOpenFile(this, [this](const QUrl &url) {
		if(url.isValid()) {
			logDebug() << "Importing from URL" << url;

			QSharedPointer<QIODevice> device;
#ifdef Q_OS_ANDROID
			if(url.scheme() == QStringLiteral("content"))
				device.reset(new ContentDevice(url));
			else
#endif
			if(url.isLocalFile())
				device.reset(new QFile(url.toLocalFile()));
			else {
				critical(tr("Import failed"), tr("Unsupported URL Scheme: %1").arg(url.scheme()));
				return;
			}

			if(!device->open(QIODevice::ReadOnly | QIODevice::Text)) {
				critical(tr("Import failed"),
						 tr("Failed to open URL \"%1\" with error: %2")
						 .arg(url.toString(), device->errorString()));
				return;
			}
			auto data = device->readAll();
			device->close();

			if(AccountManager::isTrustedImport(data)) {
				MessageConfig config{MessageConfig::TypeInputDialog, QMetaType::typeName(QMetaType::QString)};
				config.setTitle(tr("Import account data"))
						.setText(tr("Enter the password to decrypt the account data. "
									"Then choose whether you want to keep you local data or not:"))
						.setButtons(MessageConfig::YesToAll | MessageConfig::Yes | MessageConfig::Cancel)
						.setButtonText(MessageConfig::YesToAll, tr("Reset data"))
						.setButtonText(MessageConfig::Yes, tr("Keep data"))
						.setViewProperties({
											   {QStringLiteral("echoMode"), 2} //QLineEdit::Password
										   });
				auto res = CoreApp::showDialog(config);
				connect(res, &MessageResult::dialogDone, this, [this, res, data](MessageConfig::StandardButton btn) {
					switch (btn) {
					case MessageConfig::YesToAll:
						d->performImport(true, res->result().toString(), data, false);
						break;
					case MessageConfig::Yes:
						d->performImport(true, res->result().toString(), data, true);
						break;
					default:
						break;
					}
				});
			} else {
				MessageConfig config{MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion};
				config.setTitle(tr("Import account data"))
						.setText(tr("Keep the local data after changing the account?"))
						.setButtons(MessageConfig::YesToAll | MessageConfig::Yes | MessageConfig::Cancel)
						.setButtonText(MessageConfig::YesToAll, tr("Reset data"))
						.setButtonText(MessageConfig::Yes, tr("Keep data"));
				auto res = CoreApp::showDialog(config);
				connect(res, &MessageResult::dialogDone, this, [this, data](MessageConfig::StandardButton btn) {
					switch (btn) {
					case MessageConfig::YesToAll:
						d->performImport(false, {}, data, false);
						break;
					case MessageConfig::Yes:
						d->performImport(false, {}, data, true);
						break;
					default:
						break;
					}
				});
			}
		}
	}, tr("Import account data"),
	{QStringLiteral("application/x-datasync-account-data"), QStringLiteral("application/octet-stream")},
	QUrl::fromLocalFile(home));
}

void DataSyncViewModel::performReset()
{
	MessageConfig config {MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion};
	config.setTitle(tr("Reset Account?"))
			.setText(tr("Do you want to reset your account? "
						"You will loose the connection to all other devices and get a new identity. "
						"You can either keep your data or reset it as well. "
						"This cannot be undone!"))
			.setButtons(MessageConfig::YesToAll | MessageConfig::Yes | MessageConfig::Cancel)
			.setButtonText(MessageConfig::YesToAll, tr("Reset data"))
			.setButtonText(MessageConfig::Yes, tr("Keep data"));
	auto res = CoreApp::showDialog(config);
	connect(res, &MessageResult::dialogDone, this, [this](MessageConfig::StandardButton btn) {
		switch (btn) {
		case MessageConfig::YesToAll:
			d->accountManager->resetAccount(false);
			break;
		case MessageConfig::Yes:
			d->accountManager->resetAccount(true);
			break;
		default:
			break;
		}
	});
}

void DataSyncViewModel::changeRemote()
{
	showForResult<ChangeRemoteViewModel>(DataSyncViewModelPrivate::ChangeRemoteRequestCode);
}

void DataSyncViewModel::startNetworkExchange()
{
	show<NetworkExchangeViewModel>(NetworkExchangeViewModel::showParams(d->accountManager));
}

void DataSyncViewModel::removeDevice(int sortedIndex)
{
	auto mIndex = d->sortedModel->index(sortedIndex, 0);
	auto index = d->sortedModel->mapToSource(mIndex);
	if(index.isValid())
		d->accountModel->removeDevice(index);
}

void DataSyncViewModel::setColorMap(DataSyncViewModel::ColorMap colorMap)
{
	if (d->colorMap == colorMap)
		return;

	d->colorMap = std::move(colorMap);
	emit colorMapChanged(d->colorMap, {});
}

void DataSyncViewModel::resetColorMap()
{
	d->colorMap.clear();
	d->colorMap.insert(SyncManager::Initializing, Qt::darkCyan);
	d->colorMap.insert(SyncManager::Downloading, Qt::darkCyan);
	d->colorMap.insert(SyncManager::Uploading, Qt::darkCyan);
	d->colorMap.insert(SyncManager::Synchronized, Qt::darkGreen);
	d->colorMap.insert(SyncManager::Error, Qt::darkRed);
	d->colorMap.insert(SyncManager::Disconnected, Qt::darkYellow);
	emit colorMapChanged(d->colorMap, {});
}

void DataSyncViewModel::showImportDialog(LoginRequest request)
{
	if(request.handled()) {
		logDebug() << "Skipping already handeled import request";
		return;
	}

	question(tr("Login requested!"),
			 tr("<p>A device wants to log into your account:</p>"
				"<p>Name: %1<br/>"
				"Fingerprint: %2</p>"
				"<p>Do you want accept the request?</p>")
			 .arg(request.device().name(), formatFingerPrint(request.device().fingerprint())),
			 this, [request](bool ok) {
		auto req = request;
		if(!req.handled()) {
			if(ok)
				req.accept();
			else
				req.reject();
		} else
			logDebug() << "Skipping already handeled import request";
	});
}

void DataSyncViewModel::showImportAccepted()
{
	information(tr("Import accepted"),
				tr("The partner has accepted the import. You are now logged in."));
}

void DataSyncViewModel::showAccessGranted(const QUuid &id)
{
	d->pendingGrants.insert(id);
	//no need to call list devices, is done by the model
}

void DataSyncViewModel::triggerGranted(const QList<DeviceInfo> &devices)
{
	for(const auto &device : devices) {
		if(d->pendingGrants.remove(device.deviceId())) {
			information(tr("Account access granted"),
						tr("<p>Account access has been granted to device:</p>"
						   "<p>Name: %1<br/>"
						   "Fingerprint: %2</p>")
						.arg(device.name(), formatFingerPrint(device.fingerprint())));
		}
	}
}

void DataSyncViewModel::onInit(const QVariantHash &params)
{
	try {
		if(params.contains(paramReplicaNode)) {
			auto node = params.value(paramReplicaNode).value<QRemoteObjectNode*>();
			d->syncManager = new SyncManager(node, this);
			d->accountManager = new AccountManager(node, this);
		} else {
			auto setup = params.value(paramSetup, QtDataSync::DefaultSetup).toString();
			d->syncManager = new SyncManager(setup, this);
			d->accountManager = new AccountManager(setup, this);
		}

		//sync manager
		connect(d->syncManager, &SyncManager::syncStateChanged,
				this, PSIG(&DataSyncViewModel::statusStringChanged));
		connect(d->syncManager, &SyncManager::lastErrorChanged,
				this, PSIG(&DataSyncViewModel::statusStringChanged));

		//account manager
		connect(d->accountManager, &AccountManager::loginRequested,
				this, &DataSyncViewModel::showImportDialog);
		connect(d->accountManager, &AccountManager::importAccepted,
				this, &DataSyncViewModel::showImportAccepted);
		connect(d->accountManager, &AccountManager::accountAccessGranted,
				this, &DataSyncViewModel::showAccessGranted);
		connect(d->accountManager, &AccountManager::accountDevices,
				this, &DataSyncViewModel::triggerGranted);

		//model
		d->accountModel->setup(d->accountManager, d->syncManager);
		d->sortedModel->sort(0);

		emit syncManagerChanged(d->syncManager, {});
		emit accountManagerChanged(d->accountManager, {});
		emit ready({});
	} catch(SetupDoesNotExistException &e) {
		logCritical() << "Failed to init DataSyncViewModel with error:"
					  << e.what();
	}
}

void DataSyncViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	switch (requestCode) {
	case DataSyncViewModelPrivate::ExportRequestCode:
		if(result.isValid()) {
			auto res = ExportSetupViewModel::result(result);
			d->performExport(std::get<0>(res), std::get<1>(res), std::get<2>(res));
		}
		break;
	case DataSyncViewModelPrivate::ChangeRemoteRequestCode:
		if(result.isValid()) {
			auto res = ChangeRemoteViewModel::result(result);
			logDebug() << "Changing remote to server" << std::get<0>(res).url();
			d->accountManager->changeRemote(std::get<0>(res), std::get<1>(res));
		}
		break;
	default:
		break;
	}
}

// ------------- Private Implementation -------------

QtMvvm::DataSyncViewModelPrivate::DataSyncViewModelPrivate(DataSyncViewModel *q_ptr) :
	q(q_ptr),
	accountModel(new AccountModel(q_ptr)),
	sortedModel(new QSortFilterProxyModel(q_ptr))
{}

void DataSyncViewModelPrivate::performExport(bool trusted, bool includeServer, const QString &password)
{
	auto home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	getSaveFile(q, [this, trusted, includeServer, password](const QUrl &url) {
		if(url.isValid()) {
			logDebug() << "Exporting to URL" << url;

			QSharedPointer<QIODevice> device;
#ifdef Q_OS_ANDROID
			if(url.scheme() == QStringLiteral("content"))
				device.reset(new ContentDevice(url));
			else
#endif
			if(url.isLocalFile())
				device.reset(new QFile(url.toLocalFile()));
			else {
				critical(DataSyncViewModel::tr("Export failed"),
						 DataSyncViewModel::tr("Unsupported URL Scheme: %1").arg(url.scheme()));
				return;
			}

			if(!device->open(QIODevice::WriteOnly | QIODevice::Text)) {
				critical(DataSyncViewModel::tr("Export failed"),
						 DataSyncViewModel::tr("Failed to open URL \"%1\" with error: %2")
						 .arg(url.toString(), device->errorString()));
				return;
			}

			QPointer<DataSyncViewModel> qPtr(q);
			auto resFn = [qPtr, device](const QByteArray &data) {
				if(!qPtr)
					return;
				device->write(data);
				device->close();
				information(DataSyncViewModel::tr("Export completed"),
							DataSyncViewModel::tr("Data was successfully exported."));
			};
			auto errFn = [qPtr, device](const QString &error){
				if(!qPtr)
					return;
				critical(DataSyncViewModel::tr("Export failed"), error);
			};

			if(trusted)
				accountManager->exportAccountTrusted(includeServer, password, resFn, errFn);
			else
				accountManager->exportAccount(includeServer, resFn, errFn);
		}
	}, DataSyncViewModel::tr("Export account data"),
	{QStringLiteral("application/x-datasync-account-data"), QStringLiteral("application/octet-stream")},
	QUrl::fromLocalFile(home));
}

void DataSyncViewModelPrivate::performImport(bool trusted, const QString &password, const QByteArray &data, bool keepData)
{
	QPointer<DataSyncViewModel> qPtr(q);
	auto resFn = [qPtr](bool ok, const QString &error) {
		if(!qPtr)
			return;
		if(ok) {
			information(DataSyncViewModel::tr("Import completed"),
						DataSyncViewModel::tr("Data was successfully imported."));
		} else
			critical(DataSyncViewModel::tr("Import failed"), error);
	};

	if(trusted)
		accountManager->importAccountTrusted(data, password, resFn, keepData);
	else
		accountManager->importAccount(data, resFn, keepData);
}
