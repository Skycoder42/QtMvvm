#include "inputviewfactory.h"
#include "inputviewfactory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

#include <QtQml/qqml.h>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>

using namespace QtMvvm;

InputViewFactory::InputViewFactory() :
	QObject(nullptr),
	d(new InputViewFactoryPrivate())
{}

InputViewFactory::~InputViewFactory() {}

QUrl InputViewFactory::getInputUrl(const QByteArray &type, const QVariantMap &viewProperties)
{
	QUrl url;

	Q_UNUSED(viewProperties)
	if(d->inputAliases.contains(type))
		url = getInputUrl(d->inputAliases.value(type), viewProperties);
	if(d->simpleInputs.contains(type))
		url = d->simpleInputs.value(type);
	else if(type == QMetaType::typeName(QMetaType::Bool))
		url = QStringLiteral("qrc:/qtmvvm/inputs/CheckBox.qml");
	else if(type == "switch")
		url = QStringLiteral("qrc:/qtmvvm/inputs/Switch.qml");
	else if(type == QMetaType::typeName(QMetaType::QString) || type == "string")
		url = QStringLiteral("qrc:/qtmvvm/inputs/TextField.qml");
	else if(type == QMetaType::typeName(QMetaType::Int))
		url = QStringLiteral("qrc:/qtmvvm/inputs/SpinBox.qml");
	else if(type == QMetaType::typeName(QMetaType::Double))
		url = QStringLiteral("qrc:/qtmvvm/inputs/DoubleSpinBox.qml");
//	else if(type == QMetaType::typeName(QMetaType::QDate))
//		url = QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QTime))
//		url = QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QDateTime) || type == "date")
//		url = QUrl();
	else if(type == QMetaType::typeName(QMetaType::QFont))
		url = QStringLiteral("qrc:/qtmvvm/inputs/FontEdit.qml");
	else if(type == QMetaType::typeName(QMetaType::QUrl) || type == "url")
		url = QStringLiteral("qrc:/qtmvvm/inputs/UrlField.qml");
	else if(type == "selection" || type == "list")
		url = QStringLiteral("qrc:/qtmvvm/inputs/ListEdit.qml");
	else if(type == "radiolist")
		url = QStringLiteral("qrc:/qtmvvm/inputs/RadioListEdit.qml");
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
	if(d->simpleDelegates.contains(type))
		url = d->simpleDelegates.value(type);
	else if(type == QMetaType::typeName(QMetaType::Bool))
		url = QStringLiteral("qrc:/qtmvvm/delegates/BoolDelegate.qml");
	else if(type == "switch")
		url = QStringLiteral("qrc:/qtmvvm/delegates/SwitchDelegate.qml");
	else if((type == "selection" || type == "list") &&
			!viewProperties.value(QStringLiteral("editable"), false).toBool())
		url = QStringLiteral("qrc:/qtmvvm/delegates/ListDelegate.qml");
	else
		url = QStringLiteral("qrc:/qtmvvm/delegates/MsgDelegate.qml");

	logDebug() << "Found view URL for delegate of type" << type << "as" << url;
	return url;
}

void InputViewFactory::addSimpleInput(const QByteArray &type, const QUrl &qmlFileUrl)
{
	d->simpleInputs.insert(type, qmlFileUrl);
}

void InputViewFactory::addSimpleDelegate(const QByteArray &type, const QUrl &qmlFileUrl)
{
	d->simpleDelegates.insert(type, qmlFileUrl);
}

void InputViewFactory::addInputAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->inputAliases.insert(alias, targetType);
}

void InputViewFactory::addDelegateAlias(const QByteArray &alias, const QByteArray &targetType)
{
	d->delegateAliases.insert(alias, targetType);
}
