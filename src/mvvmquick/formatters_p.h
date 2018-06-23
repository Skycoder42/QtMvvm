#ifndef QTMVVM_FORMATTERS_P_H
#define QTMVVM_FORMATTERS_P_H

#include <QtCore/QCoreApplication>

#include "inputviewfactory.h"

namespace QtMvvm {

class IntFormatter : public Formatter
{
public:
	QString format(const QString &formatString, const QVariant &value, const QVariantMap &viewProperties) const override {
		if(viewProperties.value(QStringLiteral("qtmvvm_ztr"), formatString.contains(QStringLiteral("%n"))).toBool())
			return QCoreApplication::translate("qtmvvm_settings_xml", qUtf8Printable(formatString), nullptr, value.toInt());
		else
			return formatString.arg(value.toInt());
	}
};

class ListFormatter : public Formatter
{
public:
	QString format(const QString &formatString, const QVariant &value, const QVariantMap &viewProperties) const override {
		if(viewProperties.contains(QStringLiteral("listElements"))) {
			for(const auto &element : viewProperties.value(QStringLiteral("listElements")).toList()) {
				if(element.type() != QVariant::Map)
					continue;
				auto eMap = element.toMap();
				if(eMap.value(QStringLiteral("value")) == value)
					return formatString.arg(eMap.value(QStringLiteral("name")).toString());
			}
		}

		return formatString.arg(value.toString());
	}
};

}

#endif // QTMVVM_FORMATTERS_P_H
