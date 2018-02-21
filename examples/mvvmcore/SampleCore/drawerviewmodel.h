#ifndef DRAWERVIEWMODEL_H
#define DRAWERVIEWMODEL_H

#include <QtGui/QStandardItemModel>
#include <QtMvvmCore/ViewModel>

class DrawerViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QStandardItemModel* navModel READ navModel CONSTANT)

public:
	Q_INVOKABLE explicit DrawerViewModel(QObject *parent = nullptr);
	~DrawerViewModel();

	QStandardItemModel* navModel() const;

public Q_SLOTS:
	void open(int index);

private:
	QStandardItemModel* _navModel;
};

#endif // DRAWERVIEWMODEL_H
