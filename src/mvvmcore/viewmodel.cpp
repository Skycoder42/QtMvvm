#include "viewmodel.h"
#include "viewmodel_p.h"
#include "coreapp_p.h"
#include "qtmvvm_logging_p.h"
using namespace QtMvvm;

ViewModel::ViewModel(QObject *parent) :
	QObject(parent),
	d(new ViewModelPrivate())
{}

ViewModel::~ViewModel() {}

void ViewModel::onInit(const QVariantHash &) {}

void ViewModel::onResult(quint32 requestCode, const QVariant &result)
{
	Q_UNUSED(result)
	logDebug() << "Unused result on" << metaObject()->className()
			   << "with request code" << requestCode;
}

void ViewModel::show(const char *viewModelName, const QVariantHash &params) const
{
	auto metaId = QMetaType::type(viewModelName);
	auto metaObject = QMetaType::metaObjectForType(metaId);
	if(!metaObject) {
		throw PresenterException(QByteArrayLiteral("Given name (") +
								 viewModelName +
								 QByteArrayLiteral(") does not name a type with meta data"));
	}
	show(metaObject, params);
}

void ViewModel::show(const QMetaObject *viewMetaObject, const QVariantHash &params) const
{
	if(!viewMetaObject->inherits(&ViewModel::staticMetaObject)) {
		throw PresenterException(QByteArrayLiteral("Given type (") +
								 viewMetaObject->className() +
								 QByteArrayLiteral(") is not a class that extends QtMvvm::ViewModel"));
	}
	showImp(viewMetaObject, params, const_cast<ViewModel*>(this));
}

void ViewModel::showForResult(quint32 requestCode, const char *viewModelName, const QVariantHash &params) const
{
	auto metaId = QMetaType::type(viewModelName);
	auto metaObject = QMetaType::metaObjectForType(metaId);
	if(!metaObject) {
		throw PresenterException(QByteArrayLiteral("Given name (") +
								 viewModelName +
								 QByteArrayLiteral(") does not name a type with meta data"));
	}
	showForResult(requestCode, metaObject, params);
}

void ViewModel::showForResult(quint32 requestCode, const QMetaObject *viewMetaObject, const QVariantHash &params) const
{
	if(!viewMetaObject->inherits(&ViewModel::staticMetaObject)) {
		throw PresenterException(QByteArrayLiteral("Given type (") +
								 viewMetaObject->className() +
								 QByteArrayLiteral(") is not a class that extends QtMvvm::ViewModel"));
	}
	showResultImp(requestCode, viewMetaObject, params);
}

void ViewModel::showImp(const QMetaObject *mo, const QVariantHash &params, QPointer<ViewModel> parent)
{
	CoreAppPrivate::dInstance()->showViewModel(mo, params, parent, 0);
}

void ViewModel::showResultImp(quint32 requestCode, const QMetaObject *mo, const QVariantHash &params) const
{
	Q_ASSERT_X(requestCode != 0, Q_FUNC_INFO, "requestCode must not be 0");
	CoreAppPrivate::dInstance()->showViewModel(mo, params, const_cast<ViewModel*>(this), requestCode);
}

// ------------- Private Implementation -------------

ViewModelPrivate::ViewModelPrivate()
{}
