#ifndef QTMVVMCORE_HELPERTYPES_H
#define QTMVVMCORE_HELPERTYPES_H

#include <type_traits>
#include <tuple>
#include <functional>

#include <QtCore/qobject.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qvariant.h>

namespace QtMvvm {
namespace __helpertypes {

static const QByteArray InjectablePrefix = QByteArrayLiteral("de.skycoder42.qtmvvm.injectable.");

template <typename T>
struct is_qobj : public std::is_base_of<QObject, T> {};

template <typename T>
struct is_qobj_ptr : public is_qobj<std::remove_pointer_t<T>> {};

template <typename TInterface, typename TService>
struct is_valid_interface : public std::integral_constant<bool, std::is_base_of<TInterface, TService>::value && is_qobj<TService>::value> {};

template <typename TInjectPtr>
inline QByteArray qobject_iid(std::enable_if_t<is_qobj_ptr<TInjectPtr>::value, void*> = nullptr) {
	return InjectablePrefix + std::remove_pointer_t<TInjectPtr>::staticMetaObject.className();
}

template <typename TInjectPtr>
inline QByteArray qobject_iid(std::enable_if_t<!is_qobj_ptr<TInjectPtr>::value, void*> = nullptr) {
	Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid injection type");
	return "";
}

template <typename TInjectPtr>
inline QByteArray inject_iid() {
	auto iid = qobject_interface_iid<TInjectPtr>();
	if(iid)
		return iid;
	else
		return qobject_iid<TInjectPtr>();
}

template <typename TFunc, typename T1, typename... TArgs>
inline std::function<QObject*(QObjectList)> pack_function_imp(TFunc fn, QByteArrayList &injectables);
template <typename TFunc>
inline std::function<QObject*(QObjectList)> pack_function_imp(TFunc fn, QByteArrayList &injectables);

template <typename TFunc, typename T1, typename... TArgs>
inline std::function<QObject*(QObjectList)> pack_function_imp(TFunc fn, QByteArrayList &injectables) {
	injectables.append(inject_iid<T1>());
	auto subFn = [fn](const QObjectList &params, int index, TArgs... tArgs) {
		--index;
		return fn(params, index, qobject_cast<T1>(params[index]), tArgs...);
	};
	return pack_function_imp<decltype(subFn), TArgs...>(std::move(subFn), injectables);
}

template <typename TFunc>
inline std::function<QObject*(QObjectList)> pack_function_imp(TFunc fn, QByteArrayList &injectables) {
	Q_UNUSED(injectables)
	return [fn](const QObjectList &params) {
		return fn(params, params.size());
	};
}

template <typename T>
struct fn_info : public fn_info<decltype(&T::operator())> {};

template <typename TClass, typename TRet, typename... TArgs>
struct fn_info<TRet(TClass::*)(TArgs...) const>
{
	template <typename TFunctor>
	static inline std::function<QObject*(QObjectList)> pack(TFunctor fn, QByteArrayList &injectables) {
		auto subFn = [fn](const QObjectList &params, int index, TArgs... args) -> QObject* {
			Q_UNUSED(params)
			Q_ASSERT_X(index == 0, Q_FUNC_INFO, "number of params does not equal recursion depth");
			return fn(args...);
		};
		return pack_function_imp<decltype(subFn), TArgs...>(std::move(subFn), injectables);
	}
};

template <typename TFunc>
inline std::function<QObject*(QObjectList)> pack_function(TFunc fn, QByteArrayList &injectables) {
	return fn_info<TFunc>::pack(std::move(fn), injectables);
}


}
}

#endif // QTMVVMCORE_HELPERTYPES_H
