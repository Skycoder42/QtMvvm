#include "isettingsaccessor.h"
#include "qsettingsaccessor.h"
using namespace QtMvvm;

int ISettingsAccessor::_DefaultAccessorType = qMetaTypeId<QSettingsAccessor*>();

void ISettingsAccessor::setDefaultAccessor(int typeId)
{
	auto metaObject = QMetaType::metaObjectForType(typeId);
	Q_ASSERT_X(metaObject && metaObject->inherits(&ISettingsAccessor::staticMetaObject),
			   Q_FUNC_INFO,
			   "Invalid type ID. Must be a pointer to a class that implements QtMvvm::ISettingsAccessor");
	_DefaultAccessorType = typeId;
}

ISettingsAccessor *ISettingsAccessor::createDefaultAccessor(QObject *parent)
{
	auto metaObject = QMetaType::metaObjectForType(_DefaultAccessorType);
	if(metaObject)
		return qobject_cast<ISettingsAccessor*>(metaObject->newInstance(Q_ARG(QObject*, parent)));
	else
		return nullptr;
}

ISettingsAccessor::ISettingsAccessor(QObject *parent) :
	QObject{parent}
{}
