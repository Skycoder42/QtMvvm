#include "changeremoteviewmodel_p.h"
using namespace QtMvvm;
using namespace QtDataSync;

std::tuple<RemoteConfig, bool> ChangeRemoteViewModel::result(const QVariant &data)
{
	auto map = data.toHash();
	return std::make_tuple(map.value(QStringLiteral("config")).value<RemoteConfig>(),
						   map.value(QStringLiteral("keepData")).toBool());
}

ChangeRemoteViewModel::ChangeRemoteViewModel(QObject *parent) :
	ViewModel(parent),
	_headerModel(new QStandardItemModel(0, 2, this))
{
	_headerModel->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
	_headerModel->setItemRoleNames({
									   {KeyRole, "key"},
									   {ValueRole, "value"}
								   });
	_headerModel->setSortRole(KeyRole);
	_headerModel->sort(0);

	connect(this, &ChangeRemoteViewModel::urlChanged,
			this, &ChangeRemoteViewModel::validChanged);
}

QUrl ChangeRemoteViewModel::url() const
{
	return _url;
}

QString ChangeRemoteViewModel::accessKey() const
{
	return _accessKey;
}

int ChangeRemoteViewModel::keepAlive() const
{
	return _keepAlive;
}

bool ChangeRemoteViewModel::keepData() const
{
	return _keepData;
}

QStandardItemModel *ChangeRemoteViewModel::headerModel() const
{
	return _headerModel;
}

bool ChangeRemoteViewModel::isValid() const
{
	return _url.isValid();
}

bool ChangeRemoteViewModel::completeSetup()
{
	if(!isValid())
		return false;

	RemoteConfig::HeaderHash headers;
	for(auto i = 0; i < _headerModel->rowCount(); i++) {
		auto item = _headerModel->item(i);
		headers.insert(item->text().toUtf8(),
					   item->data(ValueRole).toString().toUtf8());
	}

	QVariantHash hash;
	hash[QStringLiteral("config")] = QVariant::fromValue<RemoteConfig>({
																		   _url,
																		   _accessKey,
																		   headers,
																		   _keepAlive
																	   });
	hash[QStringLiteral("keepData")] = _keepData;
	emit resultReady(hash);
	return true;
}

void ChangeRemoteViewModel::setUrl(QUrl url)
{
	if (_url == url)
		return;

	_url = std::move(url);
	emit urlChanged(_url);
}

void ChangeRemoteViewModel::setAccessKey(QString accessKey)
{
	if (_accessKey == accessKey)
		return;

	_accessKey = std::move(accessKey);
	emit accessKeyChanged(_accessKey);
}

void ChangeRemoteViewModel::setKeepAlive(int keepAlive)
{
	if (_keepAlive == keepAlive)
		return;

	_keepAlive = keepAlive;
	emit keepAliveChanged(_keepAlive);
}

void ChangeRemoteViewModel::setKeepData(bool keepData)
{
	if (_keepData == keepData)
		return;

	_keepData = keepData;
	emit keepDataChanged(_keepData);
}

void ChangeRemoteViewModel::addHeaderConfig(const QString &key, const QString &value)
{
	auto item0 = new QStandardItem(key);
	item0->setData(value, ValueRole);
	auto item1 = new QStandardItem(value);
	_headerModel->appendRow({item0, item1});
}

void ChangeRemoteViewModel::removeHeaderConfig(int index)
{
	_headerModel->removeRow(index);
}
