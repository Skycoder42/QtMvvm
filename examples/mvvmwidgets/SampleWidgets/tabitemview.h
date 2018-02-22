#ifndef TABITEMVIEW_H
#define TABITEMVIEW_H

#include <QWidget>
#include <tabviewmodel.h>

namespace Ui {
class TabItemView;
}

class TabItemView : public QWidget
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit TabItemView(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~TabItemView();

	TabItemViewModel *viewModel() const;

private:
	TabItemViewModel *_viewModel;
	Ui::TabItemView *ui;
};

#endif // TABITEMVIEW_H
