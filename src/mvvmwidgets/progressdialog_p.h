#ifndef QTMVVM_PROGRESSDIALOG_P_H
#define QTMVVM_PROGRESSDIALOG_P_H

#include <QtCore/QPointer>

#include <QtMvvmCore/Messages>

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QDialogButtonBox>

#include "qtmvvmwidgets_global.h"

namespace QtMvvm {

class ProgressDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProgressDialog(const MessageConfig &config,
							QPointer<MessageResult> result,
							QPointer<ProgressControl> control,
							QWidget *parent = nullptr);

public Q_SLOTS:
	void done(int code) override;

protected:
	void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
	void tryCancel(QDialogButtonBox::StandardButton btn);

	void updateLabel(const QString &text);
	void setIndetem(bool indetem);
	void setMin(int min);
	void setMax(int max);
	void setProg(int prog);

private:
	QPointer<MessageResult> _result;
	QPointer<ProgressControl> _control;

	QLabel *_label;
	QProgressBar *_progress;
	QDialogButtonBox *_btnBox = nullptr;
	MessageConfig::StandardButton _cancelAction = MessageConfig::NoButton;
};

}

#endif // QTMVVM_PROGRESSDIALOG_P_H
