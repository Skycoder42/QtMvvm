#include "networkexchangeviewmodel.h"
#include "networkexchangeviewmodel_p.h"
#include "datasyncviewmodel.h"
#include "exportsetupviewmodel_p.h"

#include <QtMvvmCore/Messages>
#include <QtMvvmCore/CoreApp>
#include <QtDataSync/AccountManager>
#include <QtDataSync/SetupDoesNotExistException>

#undef logDebug
#undef logInfo
#undef logWarning
#undef logCritical
#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#define PSIGARG(x, argt) [this](argt arg) { \
	(this->*(x))(arg, QPrivateSignal{}); \
}

using namespace QtMvvm;
using namespace QtDataSync;

const QString NetworkExchangeViewModel::paramSetup(QStringLiteral("setup"));
const QString NetworkExchangeViewModel::paramAccountManager(QStringLiteral("accountManager"));

QVariantHash NetworkExchangeViewModel::showParams(const QString &setup)
{
	return {
		{paramSetup, setup}
	};
}

QVariantHash NetworkExchangeViewModel::showParams(AccountManager *accountManager)
{
	return {
		{paramAccountManager, QVariant::fromValue(accountManager)}
	};
}

NetworkExchangeViewModel::NetworkExchangeViewModel(QObject *parent) :
	ViewModel(parent),
	d(new NetworkExchangeViewModelPrivate(this))
{
	d->sortedModel->setSourceModel(d->deviceModel);
}

NetworkExchangeViewModel::~NetworkExchangeViewModel() = default;

UserExchangeManager *NetworkExchangeViewModel::userExchangeManager() const
{
	return d->exchangeManager;
}

quint16 NetworkExchangeViewModel::port() const
{
	return d->port;
}

QString NetworkExchangeViewModel::deviceName() const
{
	return d->exchangeManager ?
				d->exchangeManager->accountManager()->deviceName() :
				QString();
}

bool NetworkExchangeViewModel::isActive() const
{
	return d->exchangeManager && d->active; //... d->exchangeManager->isActive();
}

bool NetworkExchangeViewModel::active() const
{
	return isActive();
}

ExchangeDevicesModel *NetworkExchangeViewModel::deviceModel() const
{
	return d->deviceModel;
}

QSortFilterProxyModel *NetworkExchangeViewModel::sortedModel() const
{
	return d->sortedModel;
}

void NetworkExchangeViewModel::exportTo(int sortedIndex)
{
	auto mIndex = d->sortedModel->index(sortedIndex, 0);
	auto index = d->sortedModel->mapToSource(mIndex);
	if(index.isValid()) {
		auto exCode = NetworkExchangeViewModelPrivate::ExportRequestCode;
		while(d->activeExports.contains(exCode))
			exCode++;
		auto info = d->deviceModel->infoAt(index);
		d->activeExports.insert(exCode, info);
		showForResult<ExportSetupViewModel>(exCode,
											ExportSetupViewModel::showParams(tr("Export accont data to device \"%1\" with address \"%1\":")
																			 .arg(info.name(), ExchangeDevicesModel::fullAddress(info))));
	}
}

void NetworkExchangeViewModel::setPort(quint16 port)
{
	if (d->port == port)
		return;

	d->port = port;
	emit portChanged(d->port, {});
}

void NetworkExchangeViewModel::setDeviceName(QString deviceName)
{
	if (!d->exchangeManager ||
		this->deviceName() == deviceName)
		return;

	d->exchangeManager->accountManager()->setDeviceName(deviceName);
}

void NetworkExchangeViewModel::setActive(bool active)
{
	if(active != isActive()) {
		if(active)
			d->active = d->exchangeManager->startExchange(d->port);
		else {
			d->exchangeManager->stopExchange();
			d->active = false;
		}
		emit activeChanged(d->active, {});
	}
}

void NetworkExchangeViewModel::onInit(const QVariantHash &params)
{
	try {
		if(params.contains(paramAccountManager)) {
			auto accountManager = params.value(paramAccountManager).value<AccountManager*>();
			d->exchangeManager = new UserExchangeManager(accountManager, this);
		} else {
			auto setup = params.value(paramSetup, QtDataSync::DefaultSetup).toString();
			d->exchangeManager = new UserExchangeManager(setup, this);
		}

		connect(d->exchangeManager->accountManager(), &AccountManager::deviceNameChanged,
				this, PSIGARG(&NetworkExchangeViewModel::deviceNameChanged, const QString &));
		connect(d->exchangeManager, &UserExchangeManager::userDataReceived,
				this, &NetworkExchangeViewModel::newUserData);
		connect(d->exchangeManager, &UserExchangeManager::exchangeError,
				this, &NetworkExchangeViewModel::exchangeError);
//		connect(d->exchangeManager, &UserExchangeManager::activeChanged,
//				this, PSIGARG(&NetworkExchangeViewModel::activeChanged, bool));

		emit userExchangeManagerChanged(d->exchangeManager, {});
		emit deviceNameChanged(deviceName(), {});

		d->deviceModel->setup(d->exchangeManager);
		d->sortedModel->sort(0);
		emit ready({});
	} catch(SetupDoesNotExistException &e) {
		logCritical() << "Failed to init DataSyncViewModel with error:"
					  << e.what();
	}
}

void NetworkExchangeViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	if(d->activeExports.contains(requestCode)) {
		auto info = d->activeExports.take(requestCode);
		if(result.isValid()) {
			logDebug() << "Exporting account to device" << info;
			auto res = ExportSetupViewModel::result(result);
			if(std::get<0>(res))
				d->exchangeManager->exportTrustedTo(info, std::get<1>(res), std::get<2>(res));
			else
				d->exchangeManager->exportTo(info, std::get<1>(res));
		}
	}
}

void NetworkExchangeViewModel::exchangeError(const QString &errorString)
{
	critical(tr("Network exchange error"), errorString);
}

void NetworkExchangeViewModel::newUserData(const UserInfo &userInfo, bool trusted)
{
	QPointer<NetworkExchangeViewModel> qPtr(this);
	auto importDoneHandler = [qPtr](bool ok, const QString &error) {
		if(!qPtr)
			return;
		if(ok) {
			information(DataSyncViewModel::tr("Import completed"),
						DataSyncViewModel::tr("Data was successfully imported."));
		} else
			critical(DataSyncViewModel::tr("Import failed"), error);
	};

	if(trusted) {
		MessageConfig config{MessageConfig::TypeInputDialog, QMetaType::typeName(QMetaType::QString)};
		config.setTitle(tr("Import account data"))
				.setText(tr("Enter the password to decrypt the account data received from \"%1\" with address \"%2\". "
							"Then choose whether you want to keep you local data or not:")
						 .arg(userInfo.name(), ExchangeDevicesModel::fullAddress(userInfo)))
				.setButtons(MessageConfig::YesToAll | MessageConfig::Yes | MessageConfig::Cancel)
				.setButtonText(MessageConfig::YesToAll, tr("Reset data"))
				.setButtonText(MessageConfig::Yes, tr("Keep data"))
				.setViewProperties({
									   {QStringLiteral("echoMode"), 2} //QLineEdit::Password
								   });
		auto res = CoreApp::showDialog(config);
		connect(res, &MessageResult::dialogDone, this, [this, res, userInfo, importDoneHandler](MessageConfig::StandardButton btn) {
			switch (btn) {
			case MessageConfig::YesToAll:
				logDebug() << "Importing account from device" << userInfo;
				d->exchangeManager->importTrustedFrom(userInfo, res->result().toString(), importDoneHandler, false);
				break;
			case MessageConfig::Yes:
				logDebug() << "Importing account from device" << userInfo;
				d->exchangeManager->importTrustedFrom(userInfo, res->result().toString(), importDoneHandler, true);
				break;
			default:
				break;
			}
		});
	} else {
		MessageConfig config{MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion};
		config.setTitle(tr("Import account data"))
				.setText(tr("Do you want to import data received from \"%1\" with address \"%2\"? "
							"Keep the local data after changing the account?")
						 .arg(userInfo.name(), ExchangeDevicesModel::fullAddress(userInfo)))
				.setButtons(MessageConfig::YesToAll | MessageConfig::Yes | MessageConfig::Cancel)
				.setButtonText(MessageConfig::YesToAll, tr("Reset data"))
				.setButtonText(MessageConfig::Yes, tr("Keep data"));
		auto res = CoreApp::showDialog(config);
		connect(res, &MessageResult::dialogDone, this, [this, userInfo, importDoneHandler](MessageConfig::StandardButton btn) {
			switch (btn) {
			case MessageConfig::YesToAll:
				logDebug() << "Importing account from device" << userInfo;
				d->exchangeManager->importFrom(userInfo, importDoneHandler, false);
				break;
			case MessageConfig::Yes:
				logDebug() << "Importing account from device" << userInfo;
				d->exchangeManager->importFrom(userInfo, importDoneHandler, true);
				break;
			default:
				break;
			}
		});
	}
}

// ------------- Private Implementation -------------

NetworkExchangeViewModelPrivate::NetworkExchangeViewModelPrivate(NetworkExchangeViewModel *q_ptr) :
	deviceModel(new ExchangeDevicesModel(q_ptr)),
	sortedModel(new QSortFilterProxyModel(q_ptr))
{}
