#ifndef TABVIEW_H
#define TABVIEW_H

#include <QtWidgets/QTabWidget>
#include <QtMvvmWidgets/IPresentingView>
#include <tabviewmodel.h>

namespace Ui {
class TabView;
}

class TabView : public QTabWidget, public QtMvvm::IPresentingView
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresentingView)

public:
	Q_INVOKABLE explicit TabView(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~TabView();

	bool tryPresent(QWidget *view) override;

private Q_SLOTS:
	void trigger(int index);

private:
	TabViewModel *_viewModel;
	Ui::TabView *ui;
};

#endif // TABVIEW_H
