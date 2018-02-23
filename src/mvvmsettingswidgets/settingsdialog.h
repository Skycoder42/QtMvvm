#ifndef QTMVVM_SETTINGSDIALOG_H
#define QTMVVM_SETTINGSDIALOG_H

#include <QtCore/qscopedpointer.h>

#include <QtMvvmSettingsCore/settingsviewmodel.h>

#include <QtWidgets/qdialog.h>

#include "QtMvvmSettingsWidgets/qtmvvmsettingswidgets_global.h"

namespace QtMvvm {

class SettingsDialogPrivate;
class Q_MVVMSETTINGSWIDGETS_EXPORT SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SettingsDialog(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~SettingsDialog();

protected:
	virtual QString labelFilterStyleSheet() const;
	virtual QUrl iconOverwrite() const;

private:
	friend class QtMvvm::SettingsDialogPrivate;
	SettingsDialogPrivate *d;
};

}

#endif // QTMVVM_SETTINGSDIALOG_H
