#include "inputwidgetfactory.h"
#include "inputwidgetfactory_p.h"

#include <QtMvvmCore/IPresenter>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QSlider>

#include "fontcombobox_p.h"
#include "selectcombobox_p.h"
#include "tooltipslider_p.h"
#include "coloredit_p.h"

#include <qurlvalidator.h>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

InputWidgetFactory::InputWidgetFactory(QObject *parent) :
	QObject(parent),
	d(new InputWidgetFactoryPrivate())
{}

InputWidgetFactory::~InputWidgetFactory() = default;

QWidget *InputWidgetFactory::createInput(const QByteArray &type, QWidget *parent, const QVariantMap &viewProperties)
{
	QWidget *widget = nullptr;
	if(d->aliases.contains(type))
		return createInput(d->aliases.value(type), parent, viewProperties);
	if(d->simpleWidgets.contains(type))
		widget = d->simpleWidgets.value(type)(parent);
	else if(type == QMetaType::typeName(QMetaType::Bool) || type == "switch")
		widget = new QCheckBox(parent);
	else if(type == QMetaType::typeName(QMetaType::QString) || type == "string") {
		auto edit = new QLineEdit(parent);
		if(viewProperties.contains(QStringLiteral("regexp"))) {
			QRegularExpression regex(viewProperties.value(QStringLiteral("regexp")).toString());
			regex.setPatternOptions(static_cast<QRegularExpression::PatternOptions>(
										viewProperties.value(QStringLiteral("patternOptions"),
															 static_cast<int>(regex.patternOptions()))
										.toInt()));
			edit->setValidator(new QRegularExpressionValidator(regex, edit));
		}
		widget = edit;
	} else if(type == QMetaType::typeName(QMetaType::Int))
		widget = new QSpinBox(parent);
	else if(type == QMetaType::typeName(QMetaType::Double) || type == "number")
		widget = new QDoubleSpinBox(parent);
	else if(type == "range")
		widget = new ToolTipSlider(parent);
	else if(type == QMetaType::typeName(QMetaType::QDate)) {
		widget = new QDateEdit(parent);
		static_cast<QDateEdit*>(widget)->setCalendarPopup(true);
	} else if(type == QMetaType::typeName(QMetaType::QTime))
		widget = new QTimeEdit(parent);
	else if(type == QMetaType::typeName(QMetaType::QDateTime) || type == "date") {
		widget = new QDateTimeEdit(parent);
		static_cast<QDateEdit*>(widget)->setCalendarPopup(true);
	} else if(type == QMetaType::typeName(QMetaType::QColor) || type == "color")
		widget = new ColorEdit(parent);
	else if(type == QMetaType::typeName(QMetaType::QFont) || type == "font")
		widget = new FontComboBox(parent);
	else if(type == QMetaType::typeName(QMetaType::QKeySequence))
		widget = new QKeySequenceEdit(parent);
	else if(type == QMetaType::typeName(QMetaType::QUrl) || type == "url") {
		auto edit = new QLineEdit(parent);
		auto validator = new QUrlValidator(edit);
		if(viewProperties.contains(QStringLiteral("allowedSchemes")))
			validator->setAllowedSchemes(viewProperties.value(QStringLiteral("allowedSchemes")).toStringList());
		edit->setValidator(validator);
		widget = edit;
	} else if(type == "selection" || type == "list")
		widget = new SelectComboBox(parent);
	else
		throw PresenterException("Unable to find an input view for type: " + type);

	for(auto it = viewProperties.constBegin(); it != viewProperties.constEnd(); it++)
		widget->setProperty(qUtf8Printable(it.key()), it.value());
	logDebug() << "Found view for input of type" << type << "as" << widget->metaObject()->className();
	return widget;
}

void InputWidgetFactory::addSimpleWidget(const QByteArray &type, const std::function<QWidget *(QWidget *)> &creator)
{
	Q_ASSERT_X(creator, Q_FUNC_INFO, "The passed creation function must be valid");
	d->simpleWidgets.insert(type, creator);
}

void InputWidgetFactory::addAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->aliases.insert(alias, targetType);
}
