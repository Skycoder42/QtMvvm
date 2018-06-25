#ifndef QTMVVM_FORMATTERS_P_H
#define QTMVVM_FORMATTERS_P_H

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>

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

template <typename T>
class DateTimeFormatter : public Formatter
{
public:
	QString format(const QString &formatString, const QVariant &value, const QVariantMap &viewProperties) const override {
		auto formatType = viewProperties.value(QStringLiteral("qtmvvm_dateformat"));
		QString dateStr;
		if(formatType.type() == QVariant::Int)
			dateStr = value.template value<T>().toString(static_cast<Qt::DateFormat>(formatType.toInt()));
		else if(formatType.type() == QVariant::String)
			dateStr = value.template value<T>().toString(formatType.toString());
		else
			dateStr = value.template value<T>().toString(Qt::DefaultLocaleShortDate);
		return formatString.arg(dateStr);
	}
};

}

#endif // QTMVVM_FORMATTERS_P_H
