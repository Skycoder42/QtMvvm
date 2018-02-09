#ifndef QTMVVMCORE_HELPERTYPES_H
#define QTMVVMCORE_HELPERTYPES_H

#include <type_traits>

#include <QtCore/qobject.h>

namespace QtMvvm {
namespace __helpertypes {

template <typename T>
struct is_qobj : public std::is_base_of<QObject, std::remove_pointer_t<T>> {};

template <typename TInject>
inline QByteArray qobject_iid(std::enable_if_t<is_qobj<TInject>::value, void*> = nullptr) {
	return QByteArrayLiteral("de.skycoder42.qtmvvm.injectable.") + std::remove_pointer_t<TInject>::staticMetaObject.className();
}

template <typename TInject>
inline QByteArray qobject_iid(std::enable_if_t<!is_qobj<TInject>::value, void*> = nullptr) {
	Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid injection type");
	return "";
}

template <typename TInject>
inline QByteArray inject_iid() {
	auto iid = qobject_interface_iid<TInject>();
	if(iid)
		return iid;
	else
		return qobject_iid<TInject>();
}

}
}

#endif // QTMVVMCORE_HELPERTYPES_H
