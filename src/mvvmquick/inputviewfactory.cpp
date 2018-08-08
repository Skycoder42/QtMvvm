#include "inputviewfactory.h"
#include "inputviewfactory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

#include <QtQml/qqml.h>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include "formatters_p.h"

using namespace QtMvvm;

InputViewFactory::InputViewFactory(QObject *parent) :
	QObject(parent),
	d(new InputViewFactoryPrivate())
{}

InputViewFactory::~InputViewFactory() = default;

QUrl InputViewFactory::getInputUrl(const QByteArray &type, const QVariantMap &viewProperties)
{
	Q_UNUSED(viewProperties)

	QUrl url;
	if(d->inputAliases.contains(type))
		url = getInputUrl(d->inputAliases.value(type), viewProperties);
	else if(d->simpleInputs.contains(type))
		url = d->simpleInputs.value(type);
	else {
		logCritical() << "Failed to find any input view for input type:" << type;
		return QUrl();
	}

	logDebug() << "Found view URL for input of type" << type << "as" << url;
	return url;
}

QUrl InputViewFactory::getDelegate(const QByteArray &type, const QVariantMap &viewProperties)
{
	QUrl url;

	Q_UNUSED(viewProperties)
	if(d->delegateAliases.contains(type))
		url = getDelegate(d->delegateAliases.value(type), viewProperties);
	else if(d->simpleDelegates.contains(type))
		url = d->simpleDelegates.value(type);
	else if((type == "selection" || type == "list") &&
			!viewProperties.value(QStringLiteral("editable"), false).toBool())
		url = QStringLiteral("qrc:/qtmvvm/delegates/ListDelegate.qml");
	else
		url = QStringLiteral("qrc:/qtmvvm/delegates/MsgDelegate.qml");

	logDebug() << "Found view URL for delegate of type" << type << "as" << url;
	return url;
}

QString QtMvvm::InputViewFactory::format(const QByteArray &type, const QString &formatString, const QVariant &value, const QVariantMap &viewProperties)
{
	if(d->formatterAliases.contains(type))
		return format(d->formatterAliases.value(type), formatString, value, viewProperties);
	else if(d->formatters.contains(type))
		return d->formatters.value(type)->format(formatString, value, viewProperties);
	else
		return formatString.arg(value.toString());
}

void InputViewFactory::addSimpleInput(const QByteArray &type, const QUrl &qmlFileUrl)
{
	d->simpleInputs.insert(type, qmlFileUrl);
}

void InputViewFactory::addSimpleDelegate(const QByteArray &type, const QUrl &qmlFileUrl)
{
	d->simpleDelegates.insert(type, qmlFileUrl);
}

void InputViewFactory::addFormatter(const QByteArray &type, Formatter *formatter)
{
	Q_ASSERT_X(formatter, Q_FUNC_INFO, "formatter must not be null");
	d->formatters.insert(type, QSharedPointer<Formatter>{formatter});
}

void InputViewFactory::addInputAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->inputAliases.insert(alias, targetType);
}

void InputViewFactory::addDelegateAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->delegateAliases.insert(alias, targetType);
}

void QtMvvm::InputViewFactory::addFormatterAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->formatterAliases.insert(alias, targetType);
}



Formatter::Formatter() = default;

Formatter::~Formatter() = default;



InputViewFactoryPrivate::InputViewFactoryPrivate() :
	simpleInputs{
		{QMetaType::typeName(QMetaType::Bool), QStringLiteral("qrc:/qtmvvm/inputs/CheckBox.qml")},
		{"switch", QStringLiteral("qrc:/qtmvvm/inputs/Switch.qml")},
		{QMetaType::typeName(QMetaType::QString), QStringLiteral("qrc:/qtmvvm/inputs/TextField.qml")},
		{"string", QStringLiteral("qrc:/qtmvvm/inputs/TextField.qml")},
		{QMetaType::typeName(QMetaType::Int), QStringLiteral("qrc:/qtmvvm/inputs/SpinBox.qml")},
		{QMetaType::typeName(QMetaType::Double), QStringLiteral("qrc:/qtmvvm/inputs/DoubleSpinBox.qml")},
		{"number", QStringLiteral("qrc:/qtmvvm/inputs/DoubleSpinBox.qml")},
		{"range", QStringLiteral("qrc:/qtmvvm/inputs/Slider.qml")},
		{QMetaType::typeName(QMetaType::QTime), QStringLiteral("qrc:/qtmvvm/inputs/TimeEdit.qml")},
		{QMetaType::typeName(QMetaType::QDate), QStringLiteral("qrc:/qtmvvm/inputs/DateEdit.qml")},
		{QMetaType::typeName(QMetaType::QDateTime), QStringLiteral("qrc:/qtmvvm/inputs/DateTimeEdit.qml")},
		{"date", QStringLiteral("qrc:/qtmvvm/inputs/DateTimeEdit.qml")},
		{QMetaType::typeName(QMetaType::QColor), QStringLiteral("qrc:/qtmvvm/inputs/ColorEdit.qml")},
		{"color", QStringLiteral("qrc:/qtmvvm/inputs/ColorEdit.qml")},
		{QMetaType::typeName(QMetaType::QFont), QStringLiteral("qrc:/qtmvvm/inputs/FontEdit.qml")},
		{"font", QStringLiteral("qrc:/qtmvvm/inputs/FontEdit.qml")},
		{QMetaType::typeName(QMetaType::QUrl), QStringLiteral("qrc:/qtmvvm/inputs/UrlField.qml")},
		{"url", QStringLiteral("qrc:/qtmvvm/inputs/UrlField.qml")},
		{"selection", QStringLiteral("qrc:/qtmvvm/inputs/ListEdit.qml")},
		{"list", QStringLiteral("qrc:/qtmvvm/inputs/ListEdit.qml")},
		{"radiolist", QStringLiteral("qrc:/qtmvvm/inputs/RadioListEdit.qml")}
	},
	simpleDelegates{
		{QMetaType::typeName(QMetaType::Bool), QStringLiteral("qrc:/qtmvvm/delegates/BoolDelegate.qml")},
		{"switch", QStringLiteral("qrc:/qtmvvm/delegates/SwitchDelegate.qml")},
		{"range", QStringLiteral("qrc:/qtmvvm/delegates/RangeDelegate.qml")},
		{QMetaType::typeName(QMetaType::QColor), QStringLiteral("qrc:/qtmvvm/delegates/ColorDelegate.qml")}
	},
	formatters{
		{QMetaType::typeName(QMetaType::Int), QSharedPointer<IntFormatter>::create()},
		{QMetaType::typeName(QMetaType::QTime), QSharedPointer<DateTimeFormatter<QTime>>::create()},
		{QMetaType::typeName(QMetaType::QDate), QSharedPointer<DateTimeFormatter<QDate>>::create()}
	}
{
	auto dblFormatter = QSharedPointer<SimpleFormatter<double>>::create();
	formatters.insert(QMetaType::typeName(QMetaType::Double), dblFormatter);
	formatters.insert("number", dblFormatter);

	auto dateFormatter = QSharedPointer<DateTimeFormatter<QDateTime>>::create();
	formatters.insert(QMetaType::typeName(QMetaType::QDateTime), dateFormatter);
	formatters.insert("date", dateFormatter);

	auto listFormatter = QSharedPointer<ListFormatter>::create();
	formatters.insert("selection", listFormatter);
	formatters.insert("list", listFormatter);
	formatters.insert("radiolist", listFormatter);
}
