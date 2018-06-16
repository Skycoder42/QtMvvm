#ifndef QTMVVM_IDENTITYEDITDIALOG_P_H
#define QTMVVM_IDENTITYEDITDIALOG_P_H

#include <QtCore/QScopedPointer>

#include <QtWidgets/QDialog>

#include <QtMvvmDataSyncCore/private/identityeditviewmodel_p.h>

#include "qtmvvmdatasyncwidgets_global.h"

namespace Ui {
class IdentityEditDialog;
}

namespace QtMvvm {

class Q_MVVMDATASYNCWIDGETS_EXPORT IdentityEditDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit IdentityEditDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~IdentityEditDialog() override;

public Q_SLOTS:
	void accept() override;

private:
	IdentityEditViewModel *_viewModel;
	QScopedPointer<Ui::IdentityEditDialog> ui;
};

}

#endif // QTMVVM_IDENTITYEDITDIALOG_P_H
