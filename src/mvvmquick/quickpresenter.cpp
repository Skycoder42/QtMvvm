#include "quickpresenter.h"
using namespace QtMvvm;

QuickPresenter::QuickPresenter(QObject *parent) :
	QObject(parent),
	IPresenter()
{}

void QuickPresenter::present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, QPointer<QtMvvm::ViewModel> parent)
{

}

void QuickPresenter::showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result)
{

}
