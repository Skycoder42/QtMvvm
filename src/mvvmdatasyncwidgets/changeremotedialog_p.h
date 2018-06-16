#ifndef QTMVVM_CHANGEREMOTEDIALOG_P_H
#define QTMVVM_CHANGEREMOTEDIALOG_P_H

#include <QtCore/QScopedPointer>

#include <QtWidgets/QDialog>

#include <QtMvvmDataSyncCore/private/changeremoteviewmodel_p.h>

#include "qtmvvmdatasyncwidgets_global.h"

namespace Ui {
class ChangeRemoteDialog;
}

namespace QtMvvm {

class Q_MVVMDATASYNCWIDGETS_EXPORT ChangeRemoteDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ChangeRemoteDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~ChangeRemoteDialog() override;

public Q_SLOTS:
	void accept() override;

private Q_SLOTS:
	void addHeader();
	void removeHeader();

private:
	ChangeRemoteViewModel *_viewModel;
	QScopedPointer<Ui::ChangeRemoteDialog> ui;
};

}

#endif // QTMVVM_CHANGEREMOTEDIALOG_P_H
