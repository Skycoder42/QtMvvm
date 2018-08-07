#ifndef QTMVVM_ANDROIDSETTINGSACCESSOR_P_H
#define QTMVVM_ANDROIDSETTINGSACCESSOR_P_H

#include <QtCore/QRegularExpression>
#include <QtAndroidExtras/QAndroidJniObject>

#include "androidsettingsaccessor.h"

namespace QtMvvm {

class AndroidSettingsAccessorPrivate
{
	Q_DISABLE_COPY(AndroidSettingsAccessorPrivate)

public:
	AndroidSettingsAccessor *q;
	QAndroidJniObject settings;

	QRegularExpression varRegex;

	AndroidSettingsAccessorPrivate(AndroidSettingsAccessor *q_ptr);
	AndroidSettingsAccessorPrivate(AndroidSettingsAccessor *q_ptr, const QString &file, AndroidSettingsAccessor::Mode mode);

	void setup();
	QVariant convertFromJava(const QAndroidJniObject &object);

	void dataChangedCallback(const QString &key);
};

}

#endif // QTMVVM_ANDROIDSETTINGSACCESSOR_P_H
