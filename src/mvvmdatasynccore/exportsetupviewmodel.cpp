#include "exportsetupviewmodel_p.h"
using namespace QtMvvm;

std::tuple<bool, bool, QString> ExportSetupViewModel::result(const QVariant &data)
{
	auto map = data.toHash();
	return std::make_tuple(map.value(QStringLiteral("trusted")).toBool(),
						   map.value(QStringLiteral("includeServer")).toBool(),
						   map.value(QStringLiteral("password")).toString());
}

ExportSetupViewModel::ExportSetupViewModel(QObject *parent) :
	ViewModel(parent),
	_trusted(false),
	_includeServer(false),
	_password()
{
	connect(this, &ExportSetupViewModel::trustedChanged,
			this, &ExportSetupViewModel::validChanged);
	connect(this, &ExportSetupViewModel::passwordChanged,
			this, &ExportSetupViewModel::validChanged);
}

bool ExportSetupViewModel::trusted() const
{
	return _trusted;
}

bool ExportSetupViewModel::includeServer() const
{
	return _includeServer;
}

QString ExportSetupViewModel::password() const
{
	return _password;
}

bool ExportSetupViewModel::isValid() const
{
	return !_trusted || !_password.isEmpty();
}

bool ExportSetupViewModel::completeSetup()
{
	if(!isValid())
		return false;

	QVariantHash hash;
	hash[QStringLiteral("trusted")] = _trusted;
	hash[QStringLiteral("includeServer")] = _includeServer;
	hash[QStringLiteral("password")] = _trusted ? _password : QString();
	emit resultReady(hash);
	return true;
}

void ExportSetupViewModel::setTrusted(bool trusted)
{
	if (_trusted == trusted)
		return;

	_trusted = trusted;
	emit trustedChanged(_trusted);
}

void ExportSetupViewModel::setIncludeServer(bool includeServer)
{
	if (_includeServer == includeServer)
		return;

	_includeServer = includeServer;
	emit includeServerChanged(_includeServer);
}

void ExportSetupViewModel::setPassword(QString password)
{
	if (_password == password)
		return;

	_password = password;
	emit passwordChanged(_password);
}
