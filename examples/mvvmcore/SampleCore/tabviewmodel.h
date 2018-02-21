#ifndef TABVIEWMODEL_H
#define TABVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

class TabViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit TabViewModel(QObject *parent = nullptr);
	~TabViewModel();
};

#endif // TABVIEWMODEL_H
