#ifndef QMLSETTINGSGENERATOR_H
#define QMLSETTINGSGENERATOR_H

#include "settingsgeneratorimpl.h"

class QmlSettingsGenerator : public SettingsGeneratorImpl
{
public:
	QmlSettingsGenerator(const QString &hdrPath,
						 const QString &srcPath);

	void process(const QString &inPath);

	void writeHeader(const SettingsType &settings);
	void writeSource(const SettingsType &settings);

private:
	QFile _hdrFile;
	QFile _srcFile;

	QTextStream _hdr;
	QTextStream _src;
};

#endif // QMLSETTINGSGENERATOR_H
