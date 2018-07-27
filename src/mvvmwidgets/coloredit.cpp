#include "coloredit_p.h"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOptionFrame>
#include <QtMvvmCore/Messages>
#include <utility>
using namespace QtMvvm;

ColorEdit::ColorEdit(QWidget *parent) :
	QWidget{parent},
	_btn{new ColorButton{this}},
	_edit{new QLineEdit{this}},
	_validator{new QRegularExpressionValidator{this}}
{
	auto layout = new QHBoxLayout{this};
	layout->addWidget(_btn);
	layout->addWidget(_edit);
	layout->addStretch(1);
	layout->setStretch(0, 0);
	layout->setStretch(1, 0);
	layout->setContentsMargins({0,0,0,0});
	setLayout(layout);

	_edit->setValidator(_validator);
	connect(_edit, &QLineEdit::textEdited,
			this, &ColorEdit::updateColor);
	connect(_edit, &QLineEdit::editingFinished,
			this, [this](){
		_edit->setText(_color.name(_alpha ? QColor::HexArgb : QColor::HexRgb));
	});

	connect(_btn, &ColorButton::clicked,
			this, &ColorEdit::showDialog);

	setAlpha(false);
	setColor(palette().color(QPalette::Highlight));
}

QColor ColorEdit::color() const
{
	return _color;
}

bool ColorEdit::useAlpha() const
{
	return _alpha;
}

void ColorEdit::setColor(QColor color)
{
	if (_color == color)
		return;

	_color = std::move(color);
	_btn->setColor(_color);
	if(!_skipEditUpdate)
		_edit->setText(_color.name(_alpha ? QColor::HexArgb : QColor::HexRgb));
	emit colorChanged(_color);
}

void ColorEdit::setAlpha(bool alpha)
{
	if (_alpha == alpha)
		return;

	_alpha = alpha;
	_validator->setRegularExpression(QRegularExpression{
		QStringLiteral(R"__(^#(?:[0-9a-f]{%1}){1,2}$)__").arg(_alpha ? 4 : 3),
		QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption
	});
	_edit->setMaxLength(_alpha ? 9 : 7);
	_edit->setText(_color.name(_alpha ? QColor::HexArgb : QColor::HexRgb));
	emit alphaChanged(_alpha);
}

void ColorEdit::showDialog()
{
	QtMvvm::getColor(this, [this](const QColor &color) {
		if(color.isValid())
			setColor(color);
	}, tr("Select a color"), _color, _alpha);
}

void ColorEdit::updateColor(const QString &text)
{
	if(_edit->hasAcceptableInput()) {
		QColor color{text};
		if(color.isValid()) {
			_skipEditUpdate = true;
			setColor(color);
			_skipEditUpdate = false;
		}
	}
}



ColorButton::ColorButton(QWidget *parent) :
	QAbstractButton{parent}
{
	setSizePolicy({QSizePolicy::Expanding, QSizePolicy::Preferred});
}

QColor ColorButton::color() const
{
	return _color;
}

QSize ColorButton::sizeHint() const
{
	return {50, 25};
}

void ColorButton::setColor(QColor color)
{
	_color = std::move(color);
	repaint();
}

void ColorButton::paintEvent(QPaintEvent *)
{
	QStylePainter painter{this};

	painter.fillRect(painter.window(), _color);

	QStyleOptionFrame option;
	option.initFrom(this);
	option.frameShape = QFrame::Panel;
	option.lineWidth = 2;
	option.midLineWidth = 2;
	if(isDown())
		option.state |= QStyle::State_Sunken;
	else
		option.state |= QStyle::State_Raised;

	painter.drawControl(QStyle::CE_ShapedFrame, option);
}
