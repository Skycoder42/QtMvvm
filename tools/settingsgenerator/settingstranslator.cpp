#include "settingstranslator.h"

SettingsTranslator::SettingsTranslator(const QString &srcPath) :
	_srcFile{srcPath},
	_src{&_srcFile}
{}

void SettingsTranslator::process(const QString &inPath)
{
	auto settings = readDocument(inPath);
	if(!nonstd::holds_alternative<SettingsConfigType>(settings))
		throw XmlException{inPath, 0, 0, QStringLiteral("Expected the root element to be a SettingsConfig element")};
	writeTranslations(nonstd::get<SettingsConfigType>(settings));
}

void SettingsTranslator::writeTranslations(const SettingsConfigBase::SettingsConfigType &settings)
{

}
