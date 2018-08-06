#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QtMvvmCore/AndroidSettingsAccessor>

class Settings : public QObject
{
	Q_OBJECT

public:
	explicit Settings(QObject *parent = nullptr);

	Q_INVOKABLE QString load(const QString &key);

public Q_SLOTS:
	void save(const QString &key, const QString &value);
	void remove(const QString &key);

Q_SIGNALS:
	void changeEvent(const QString &text);

private Q_SLOTS:
	void entryChanged(const QString &key, const QVariant &value);
	void entryRemoved(const QString &key);

private:
	QtMvvm::AndroidSettingsAccessor *_accessor;
};

#endif // SETTINGS_H
