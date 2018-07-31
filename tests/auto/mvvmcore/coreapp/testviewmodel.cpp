#include "testviewmodel.h"

TestViewModel::TestViewModel(QObject *parent) :
	ViewModel(parent)
{}

void TestViewModel::presenChild(const QVariantHash &params)
{
	show<TestViewModel>(params);
}

void TestViewModel::presentResult(quint32 code)
{
	showForResult<TestViewModel>(code);
}

void TestViewModel::onInit(const QVariantHash &params)
{
}

void TestViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	results.append(std::make_tuple(requestCode, result));
}



TestSingleViewModel::TestSingleViewModel(QObject *parent) :
	ViewModel(parent)
{}



TestContainedViewModel::TestContainedViewModel(QObject *parent) :
	ViewModel(parent)
{}



TestContainedSingleViewModel::TestContainedSingleViewModel(QObject *parent) :
	ViewModel(parent)
{}
