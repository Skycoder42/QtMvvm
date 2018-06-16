#ifndef QTMVVM_SETTINGSDIALOG_H
#define QTMVVM_SETTINGSDIALOG_H

#include <QtCore/qscopedpointer.h>

#include <QtMvvmCore/settingsviewmodel.h>

#include <QtWidgets/qdialog.h>

#include "QtMvvmWidgets/qtmvvmwidgets_global.h"

namespace QtMvvm {

class SettingsDialogPrivate;
//! The widgets view implementation for the SettingsViewModel
class Q_MVVMWIDGETS_EXPORT SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	//! View constructor
	Q_INVOKABLE explicit SettingsDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~SettingsDialog() override;

protected:
	//! Returns the stylesheet used highlight labels found when searching
	virtual QString labelFilterStyleSheet() const;
	//! Return a url to overwrite the default settings icon
	virtual QUrl iconOverwrite() const;

private:
	friend class QtMvvm::SettingsDialogPrivate;
	SettingsDialogPrivate *d;
};

}

#endif // QTMVVM_SETTINGSDIALOG_H
