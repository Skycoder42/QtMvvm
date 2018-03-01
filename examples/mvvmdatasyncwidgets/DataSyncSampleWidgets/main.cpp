#include <QtWidgets/QApplication>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <samplecoreapp.h>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>

#include "samplewindow.h"

QTMVVM_REGISTER_CORE_APP(SampleCoreApp)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QtMvvm::registerDataSyncWidgets();
	QtMvvm::WidgetsPresenter::registerView<SampleWindow>();

	return a.exec();
}
