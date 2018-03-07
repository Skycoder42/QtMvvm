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
	Q_UNUSED(viewProperties)
	if(d->inputAliases.contains(type))
		return getInputUrl(d->inputAliases.value(type), viewProperties);
	if(d->simpleInputs.contains(type))
		return d->simpleInputs.value(type);
	else if(type == QMetaType::typeName(QMetaType::Bool))
		return QStringLiteral("qrc:/qtmvvm/inputs/CheckBox.qml");
	else if(type == "switch")
		return QStringLiteral("qrc:/qtmvvm/inputs/Switch.qml");
	else if(type == QMetaType::typeName(QMetaType::QString) || type == "string")
		return QStringLiteral("qrc:/qtmvvm/inputs/TextField.qml");
	else if(type == QMetaType::typeName(QMetaType::Int))
		return QStringLiteral("qrc:/qtmvvm/inputs/SpinBox.qml");
	else if(type == QMetaType::typeName(QMetaType::Double))
		return QStringLiteral("qrc:/qtmvvm/inputs/DoubleSpinBox.qml");
//	else if(type == QMetaType::typeName(QMetaType::QDate))
//		return QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QTime))
//		return QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QDateTime) || type == "date")
//		return QUrl();
	else if(type == QMetaType::typeName(QMetaType::QFont))
		return QStringLiteral("qrc:/qtmvvm/inputs/FontEdit.qml");
	else if(type == QMetaType::typeName(QMetaType::QUrl) || type == "url")
		return QStringLiteral("qrc:/qtmvvm/inputs/UrlField.qml");
	else if(type == "selection" || type == "list")
		return QStringLiteral("qrc:/qtmvvm/inputs/ListEdit.qml");
	else if(type == "radiolist")
		return QStringLiteral("qrc:/qtmvvm/inputs/RadioListEdit.qml");
	else {
		logCritical() << "Failed to find any input view for input type:" << type;
		return QStringLiteral("qrc:/qtmvvm/inputs/ErrorLabel.qml");
	}
}

QUrl InputViewFactory::getDelegate(const QByteArray &type, const QVariantMap &viewProperties)
{
	Q_UNUSED(viewProperties)
	if(d->delegateAliases.contains(type))
		return getDelegate(d->delegateAliases.value(type), viewProperties);
	if(d->simpleDelegates.contains(type))
		return d->simpleDelegates.value(type);
	else if(type == QMetaType::typeName(QMetaType::Bool))
		return QStringLiteral("qrc:/qtmvvm/delegates/BoolDelegate.qml");
	else if(type == "switch")
		return QStringLiteral("qrc:/qtmvvm/delegates/SwitchDelegate.qml");
	else if((type == "selection" || type == "list") &&
			!viewProperties.value(QStringLiteral("editable"), false).toBool())
		return QStringLiteral("qrc:/qtmvvm/delegates/ListDelegate.qml");
	else
		return QStringLiteral("qrc:/qtmvvm/delegates/MsgDelegate.qml");
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

// ------------- Private Implementation -------------

InputViewFactoryPrivate::InputViewFactoryPrivate() :
	simpleInputs(),
	simpleDelegates(),
	inputAliases(),
	delegateAliases()
{}

