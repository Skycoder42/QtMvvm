#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include <resultviewmodel.h>

namespace Ui {
class ResultDialog;
}

class ResultDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ResultDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~ResultDialog();

private:
	ResultViewModel *_viewModel;
	Ui::ResultDialog *ui;
};

#endif // RESULTDIALOG_H
