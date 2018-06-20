#ifndef TABVIEWMODEL_H
#define TABVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

class TabViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	QTMVVM_SINGLETON

public:
	Q_INVOKABLE explicit TabViewModel(QObject *parent = nullptr);
	~TabViewModel();

public Q_SLOTS:
	void addTab();
};

class TabItemViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
	Q_INVOKABLE explicit TabItemViewModel(QObject *parent = nullptr);
	~TabItemViewModel();

	QString title() const;

Q_SIGNALS:
	void titleChanged(QString title);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QString _title;
};

#endif // TABVIEWMODEL_H
