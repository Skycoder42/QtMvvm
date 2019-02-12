#ifndef SETTINGSCONFIGLOADER_P_H
#define SETTINGSCONFIGLOADER_P_H

#include <QtCore/QObject>
#include <QtCore/QCache>

#include "qtmvvmcore_global.h"
#include "settingssetup.h"
#include "exception.h"

#include <settingsconfigimpl_p.h>

namespace QtMvvm {

class Q_MVVMCORE_EXPORT SettingsConfigLoader : public QObject, public ISettingsSetupLoader, public SettingsConfigImpl
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::ISettingsSetupLoader)

public:
	Q_INVOKABLE SettingsConfigLoader(QObject *parent = nullptr);

	void changeDefaultIcon(const QUrl &defaultIcon) override;
	SettingsElements::Setup loadSetup(const QString &filePath, const QString &frontend, const QFileSelector *selector) const override;

protected:
	QStringList allSelectors() const override;
	QString select(const QString &path) const override;

private:
	QUrl _defaultIcon;
	mutable QCache<std::tuple<QString, QString, QStringList>, SettingsElements::Setup> _cache;

	SettingsElements::Setup convertSettings(const SettingsConfigType &settings) const;

	template <typename... TContent>
	SettingsElements::Category convertCategory(const CategoryType &category, const QList<variant<TContent...>> &content) const;
	template <typename... TContent>
	SettingsElements::Section convertSection(const SectionType &section, const QList<variant<TContent...>> &content) const;
	template <typename... TContent>
	SettingsElements::Group convertGroup(const GroupType &group, const QList<variant<TContent...>> &content) const;
	SettingsElements::Entry convertEntry(const EntryType &entry) const;

	QVariant readElement(const ElementType &element) const;
	QPair<QString, QVariant> readProperty(const PropertyType &property) const;

	template <typename TType = QString>
	TType trIf(const optional<QString> &text, const TType &defaultValue = {}) const;
	template <typename TType = QString>
	TType trIf(const optional<QString> &text, bool allowTr, const TType &defaultValue = {}) const;
};

class Q_MVVMCORE_EXPORT SettingsConfigException : public SettingsLoaderException
{
public:
	SettingsConfigException(SettingsConfigBase::Exception &exception);
	SettingsConfigException(QByteArray what);

	const char *what() const noexcept override;

	void raise() const QTMVVM_EXCEPTION_OR;
	QTMVVM_EXCEPTION_BASE *clone() const QTMVVM_EXCEPTION_OR;

private:
	const QByteArray _what;
};

}

namespace std {

uint qHash(const std::tuple<QString, QString, QStringList> &key, uint seed);

}

#endif // SETTINGSCONFIGLOADER_P_H
