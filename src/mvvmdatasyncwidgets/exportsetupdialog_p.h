#ifndef QTMVVM_EXPORTSETUPDIALOG_P_H
#define QTMVVM_EXPORTSETUPDIALOG_P_H

#include <QtCore/QScopedPointer>

#include <QtWidgets/QDialog>

#include <QtMvvmDataSyncCore/private/exportsetupviewmodel_p.h>

#include "qtmvvmdatasyncwidgets_global.h"

namespace Ui {
class ExportSetupDialog;
}

namespace QtMvvm {

class Q_MVVMDATASYNCWIDGETS_EXPORT ExportSetupDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ExportSetupDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~ExportSetupDialog() override;

public Q_SLOTS:
	void accept() override;

private:
	ExportSetupViewModel *_viewModel;
	QScopedPointer<Ui::ExportSetupDialog> ui;
};

}

#endif // EXPORTSETUPDIALOG_P_H
