#include "progressdialog_p.h"
#include <QtGui/QCloseEvent>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QDebug>

#include <dialogmaster.h>
using namespace QtMvvm;

ProgressDialog::ProgressDialog(const MessageConfig &config, QPointer<MessageResult> result, QPointer<ProgressControl> control, QWidget *parent) :
	QDialog{parent},
	_result{std::move(result)},
	_control{std::move(control)},
	_label{new QLabel{this}},
	_progress{new QProgressBar{this}}
{
	auto props = config.viewProperties();
	auto isBusy = (config.subType() == MessageConfig::SubTypeBusy);

	auto layout = new QVBoxLayout{this};
	setLayout(layout);

	//set title and text
	auto text = config.text();
	if(text.isNull())
		text = config.title();
	else
		setWindowTitle(config.title());
	if(text.isNull())
		_label->setVisible(false);
	else
		_label->setText(text);
	connect(_control, &ProgressControl::changeLabel,
			this, &ProgressDialog::updateLabel);
	layout->addWidget(_label);

	//add the input
	if(props.contains(QStringLiteral("format")))
		_progress->setFormat(props.value(QStringLiteral("format")).toString());
	setIndetem(isBusy || _control->isIndeterminate());
	layout->addWidget(_progress);

	//add the cancel button, in case it was requested
	if(config.buttons() != MessageConfig::NoButton) {
		_btnBox = new QDialogButtonBox{this};
		_btnBox->setStandardButtons(static_cast<QDialogButtonBox::StandardButtons>(static_cast<int>(config.buttons()))); //is ok, as the buttons are the same
		auto btns = config.buttonTexts();
		for(auto it = btns.constBegin(); it != btns.constEnd(); it++){
			auto sBtn = static_cast<QDialogButtonBox::StandardButton>(it.key());
			Q_ASSERT(_btnBox->standardButtons().testFlag(sBtn)); //Must be the case now because of the change in MessageConfig
			_btnBox->button(sBtn)->setText(it.value());
		}
		layout->addWidget(_btnBox);
		// connect the box
		connect(_btnBox, &QDialogButtonBox::clicked,
				this, [this](QAbstractButton *btn) {
			tryCancel(_btnBox->standardButton(btn));
		});
	}

	//set extra props
	for(auto it = props.constBegin(); it != props.constEnd(); it++)
		setProperty(qUtf8Printable(it.key()), it.value());

	//connect stuff
	connect(_control, &ProgressControl::closeRequested,
			this, &ProgressDialog::accept);
	if(!isBusy) {
		connect(_control, &ProgressControl::minimumChanged,
				this, &ProgressDialog::setMin);
		connect(_control, &ProgressControl::maximumChanged,
				this, &ProgressDialog::setMax);
		connect(_control, &ProgressControl::progressChanged,
				this, &ProgressDialog::setProg);
		connect(_control, &ProgressControl::indeterminateChanged,
				this, &ProgressDialog::setIndetem);
	}

	adjustSize();
	DialogMaster::masterDialog(this, true, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
}

void ProgressDialog::done(int code)
{
	QDialog::done(code);
	if(_control)
		_control->notifyClosed();
	if(_result)
		_result->complete(_cancelAction);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	tryCancel(QDialogButtonBox::Cancel);
}

void ProgressDialog::tryCancel(QDialogButtonBox::StandardButton btn)
{
	if(_cancelAction == MessageConfig::NoButton && _btnBox) {
		_cancelAction = static_cast<MessageConfig::StandardButton>(btn);
		_btnBox->setEnabled(false);
		if(_control)
			_control->requestCancel(_cancelAction);
		else
			done(btn);
	}
}

void ProgressDialog::updateLabel(const QString &text)
{
	_label->setText(text);
	_label->setVisible(true);
}

void ProgressDialog::setIndetem(bool indetem)
{
	if(indetem) {
		_progress->setRange(0, 0);
		_progress->setTextVisible(false);
	} else {
		_progress->setRange(_control->minimum(), _control->maximum());
		_progress->setValue(_control->progress());
		_progress->setTextVisible(true);
	}
}

void ProgressDialog::setMin(int min)
{
	if(_control && !_control->isIndeterminate())
		_progress->setMinimum(min);
}

void ProgressDialog::setMax(int max)
{
	if(_control && !_control->isIndeterminate())
		_progress->setMaximum(max);
}

void ProgressDialog::setProg(int prog)
{
	if(_control && !_control->isIndeterminate())
		_progress->setValue(prog);
}
