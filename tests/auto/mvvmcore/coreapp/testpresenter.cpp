#include "testpresenter.h"

TestPresenter::TestPresenter(QObject *parent) :
	QObject{parent}
{}

void TestPresenter::present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, QPointer<QtMvvm::ViewModel> parent)
{
	presented.append(std::make_tuple(viewModel, params, parent));
	emit presentDone();
}

void TestPresenter::showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result)
{
	dialogs.append(std::make_tuple(config, result));
	emit dialogDone();
}
