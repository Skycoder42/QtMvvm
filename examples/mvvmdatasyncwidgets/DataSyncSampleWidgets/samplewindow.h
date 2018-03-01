#ifndef SAMPLEWINDOW_H
#define SAMPLEWINDOW_H

#include <QtWidgets/QMainWindow>
#include <sampleviewmodel.h>

namespace Ui {
class SampleWindow;
}

class SampleWindow : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SampleWindow(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~SampleWindow();

private Q_SLOTS:
	void on_addButton_clicked();
	void on_removeButton_clicked();

private:
	SampleViewModel *_viewModel;
	Ui::SampleWindow *ui;
};

#endif // SAMPLEWINDOW_H
