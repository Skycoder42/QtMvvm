#ifndef QTMVVM_IDENTITYEDITVIEWMODEL_P_H
#define QTMVVM_IDENTITYEDITVIEWMODEL_P_H

#include <QtMvvmCore/ViewModel>

#include <QtDataSync/AccountManager>

#include "qtmvvmdatasynccore_global.h"

namespace QtMvvm {

class Q_MVVMDATASYNCCORE_EXPORT IdentityEditViewModel : public ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString fingerPrint READ fingerPrint NOTIFY fingerPrintChanged)

public:
	Q_INVOKABLE explicit IdentityEditViewModel(QObject *parent = nullptr);

	static QVariantHash showParams(QtDataSync::AccountManager *manager);

	QString name() const;
	QString fingerPrint() const;

public Q_SLOTS:
	void save();

	void setName(const QString &name);

Q_SIGNALS:
	void nameChanged(const QString &name);
	void fingerPrintChanged();

protected:
	void onInit(const QVariantHash &showParams) override;

private:
	QtDataSync::AccountManager *_manager;
	QString _nameBuffer;
};

}

#endif // QTMVVM_IDENTITYEDITVIEWMODEL_P_H
