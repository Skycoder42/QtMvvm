#ifndef SAMPLEVIEW_H
#define SAMPLEVIEW_H

#include <QtWidgets/QMainWindow>
#include <sampleviewmodel.h>

namespace Ui {
class SampleView;
}

class SampleView : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SampleView(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~SampleView();

private:
	SampleViewModel *_viewModel;
	Ui::SampleView *ui;
};

#endif // SAMPLEVIEW_H
