#ifndef QTMVVM_CHANGEREMOTEVIEWMODEL_P_H
#define QTMVVM_CHANGEREMOTEVIEWMODEL_P_H

#include <QtMvvmCore/ViewModel>

#include <QtDataSync/RemoteConfig>

#include <QtGui/QStandardItemModel>

#include "qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class Q_MVVMDATASYNCCORE_EXPORT ChangeRemoteViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY(QString accessKey READ accessKey WRITE setAccessKey NOTIFY accessKeyChanged)
	Q_PROPERTY(int keepAlive READ keepAlive WRITE setKeepAlive NOTIFY keepAliveChanged)
	Q_PROPERTY(bool keepData READ keepData WRITE setKeepData NOTIFY keepDataChanged)
	Q_PROPERTY(QStandardItemModel* headerModel READ headerModel CONSTANT)

	Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
	enum Roles {
		KeyRole = Qt::DisplayRole,
		ValueRole = Qt::UserRole + 1
	};
	Q_ENUM(Roles)

	static std::tuple<QtDataSync::RemoteConfig, bool> result(const QVariant &data);

	Q_INVOKABLE explicit ChangeRemoteViewModel(QObject *parent = nullptr);

	QUrl url() const;
	QString accessKey() const;
	int keepAlive() const;
	bool keepData() const;
	QStandardItemModel* headerModel() const;
	bool isValid() const;

public Q_SLOTS:
	bool completeSetup();

	void setUrl(QUrl url);
	void setAccessKey(QString accessKey);
	void setKeepAlive(int keepAlive);
	void setKeepData(bool keepData);

	void addHeaderConfig(const QString &key, const QString &value);
	void removeHeaderConfig(int index);

Q_SIGNALS:
	void urlChanged(const QUrl &url);
	void accessKeyChanged(const QString &accessKey);
	void keepAliveChanged(int keepAlive);
	void keepDataChanged(bool keepData);
	void validChanged();

private:
	QUrl _url;
	QString _accessKey;
	int _keepAlive = 1;
	bool _keepData = true;

	QStandardItemModel *_headerModel;
};

}

#endif // QTMVVM_CHANGEREMOTEVIEWMODEL_P_H
