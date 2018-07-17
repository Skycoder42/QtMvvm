#ifndef SETTINGSTRANSLATOR_H
#define SETTINGSTRANSLATOR_H

#include <QFile>
#include <QTextStream>

#include "settingsconfigimpl.h"

class SettingsTranslator : public SettingsConfigImpl
{
public:
	SettingsTranslator(const QString &srcPath);

	void process(const QString &inPath);

private:
	QFile _srcFile;
	QTextStream _src;

	void writeTranslations(const SettingsConfigType &settings);
};

#endif // SETTINGSTRANSLATOR_H
