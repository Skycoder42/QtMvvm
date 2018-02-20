#include "inputviewfactory.h"
#include "inputviewfactory_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMetaType>

#include <QtQml/qqml.h>

#include <QtMvvmCore/private/qtmvvm_logging_p.h>

#include <qurlvalidator.h>
using namespace QtMvvm;

namespace {

void initPrivateQml()
{
	qmlRegisterType<QUrlValidator>("de.skycoder42.QtMvvm.Quick.Private", 1, 0, "UrlValidator");
}

void initResources()
{
#ifdef QT_STATIC
	initPrivateQml();
	Q_INIT_RESOURCE(qtmvvmquick_module);
#endif
}

}
Q_COREAPP_STARTUP_FUNCTION(initPrivateQml)

InputViewFactory::InputViewFactory() :
	QObject(nullptr),
	d(new InputViewFactoryPrivate())
{
	initResources();
}

InputViewFactory::~InputViewFactory() {}

//int InputViewFactory::metaTypeId(const QByteArray &type, const QVariantMap &properties)
//{
//	if(type == "string")
//		return QMetaType::QString;
//	else if(type == "list")
//		return metaTypeId(properties.value(QStringLiteral("_list_data"), QByteArray("string")).toByteArray(), properties);
//	else
//		return QMetaType::type(type);
//}

QUrl InputViewFactory::getInputUrl(const QByteArray &type, const QVariantMap &viewProperties)
{
	Q_UNUSED(viewProperties)
	if(d->simpleViews.contains(type))
		return d->simpleViews.value(type);
	else if(type == QMetaType::typeName(QMetaType::Bool))
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/CheckBox.qml");
	else if(type == QMetaType::typeName(QMetaType::QString) || type == "string")
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/TextField.qml");
	else if(type == QMetaType::typeName(QMetaType::Int))
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/SpinBox.qml");
	else if(type == QMetaType::typeName(QMetaType::Double))
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/DoubleSpinBox.qml");
//	else if(type == QMetaType::typeName(QMetaType::QDate))
//		return QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QTime))
//		return QUrl();
//	else if(type == QMetaType::typeName(QMetaType::QDateTime) || type == "date")
//		return QUrl();
	else if(type == QMetaType::typeName(QMetaType::QFont))
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/FontEdit.qml");
	else if(type == QMetaType::typeName(QMetaType::QUrl) || type == "url")
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/UrlField.qml");
	else if(type == "selection" || type == "list")
		return QStringLiteral("qrc:/de/skycoder42/qtmvvm/quick/inputs/ListEdit.qml");
	else {
		logCritical() << "Failed to find any input view for input type:" << type;
		return QUrl();
	}
}

bool InputViewFactory::addSimpleView(const QByteArray &type, const QUrl &qmlFileUrl)
{
	d->simpleViews.insert(type, qmlFileUrl);
	return true;
}

// ------------- Private Implementation -------------

InputViewFactoryPrivate::InputViewFactoryPrivate() :
	simpleViews()
{}

