#ifndef CONTAINERVIEWMODEL_H
#define CONTAINERVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

class ChildViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChildViewModel(QObject *parent = nullptr);
	~ChildViewModel();
};

class ContainerViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString vmType READ vmType CONSTANT)

public:
	Q_INVOKABLE explicit ContainerViewModel(QObject *parent = nullptr);
	~ContainerViewModel();

	QString vmType() const;

public Q_SLOTS:
	void loadChild();
};

Q_DECLARE_METATYPE(ChildViewModel*)
Q_DECLARE_METATYPE(ContainerViewModel*)

#endif // CONTAINERVIEWMODEL_H
