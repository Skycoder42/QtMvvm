#ifndef SETTINGSCONFIGLOADER_P_H
#define SETTINGSCONFIGLOADER_P_H

#include <QtCore/QObject>
#include <QtCore/QCache>

#include "settingssetup.h"

#include <settingsconfigimpl.h>

namespace QtMvvm {

class SettingsConfigLoader : public QObject, public ISettingsSetupLoader, public SettingsConfigImpl
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsSetupLoader)

public:
	Q_INVOKABLE SettingsConfigLoader(QObject *parent = nullptr);

	void changeDefaultIcon(const QUrl &defaultIcon) override;
	SettingsElements::Setup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const override;

private:
	QUrl _defaultIcon;
	mutable QCache<QString, SettingsElements::Setup> _cache;

	SettingsElements::Setup *convertSettings(const SettingsConfigType &settings) const;

	template <typename... TContent>
	SettingsElements::Category convertCategory(const CategoryType &category, const QList<variant<TContent...>> &content) const;
	template <typename... TContent>
	SettingsElements::Section convertSection(const SectionType &section, const QList<variant<TContent...>> &content) const;
	template <typename... TContent>
	SettingsElements::Group convertGroup(const GroupType &group, const QList<variant<TContent...>> &content) const;
};

class SettingsConfigException : public SettingsLoaderException
{
public:
	SettingsConfigException(SettingsConfigBase::Exception &exception);
	SettingsConfigException(QByteArray what);

	const char *what() const noexcept override;

	void raise() const override;
	QException *clone() const override;

private:
	const QByteArray _what;
};

}

#endif // SETTINGSCONFIGLOADER_P_H
