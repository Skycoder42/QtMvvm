#include "qmlsettingsgenerator.h"
#include <QFileInfo>

QmlSettingsGenerator::QmlSettingsGenerator(const QString &hdrPath, const QString &srcPath) :
	_hdrFile{hdrPath},
	_srcFile{srcPath},
	_hdr{&_hdrFile},
	_src{&_srcFile}
{}

void QmlSettingsGenerator::process(const QString &inPath)
{
	// read settings and adjust defaults
	auto settings = readDocument(inPath);
	if(!settings.name)
		settings.name = QFileInfo{inPath}.baseName();
	settings.name.value().prepend(QStringLiteral("QMLTYPE_"));
	fixTrContext(settings, QFileInfo{inPath}.fileName());

	if(!_hdrFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_hdrFile};
	writeHeader(settings);
	_hdrFile.close();

	if(!_srcFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw FileException{_srcFile};
	writeSource(settings);
	_srcFile.close();
}

void QmlSettingsGenerator::writeHeader(const SettingsGeneratorBase::SettingsType &settings)
{
	QString incGuard = QFileInfo{_hdrFile.fileName()}
					   .completeBaseName()
					   .replace(QLatin1Char('.'), QLatin1Char('_'))
					   .toUpper() + QStringLiteral("_H");
	_hdr << "#ifndef " << incGuard << '\n'
		 << "#define " << incGuard << "\n\n";

	// write the includes
	auto includes = QList<IncludeType> {
		{false, QStringLiteral("QtCore/QObject")},
		{false, QStringLiteral("QtQml/QQmlListProperty")},
		{false, QStringLiteral("QtMvvmCore/ISettingsAccessor")}
	} + settings.includes;
	for(const auto &inc : includes) {
		if(inc.local)
			_hdr << "#include \"" << inc.includePath << "\"\n";
		else
			_hdr << "#include <" << inc.includePath << ">\n";
	}
	_hdr << "\n";

	// create the class
	_hdr << "class " << QString{settings.prefix ? settings.prefix.value() + QLatin1Char(' ') + settings.name.value() : settings.name.value()} << " : public QObject\n"
		 << "{\n"
		 << "\tQ_OBJECT\n\n"
		 << "\tQ_PROPERTY(QtMvvm::ISettingsAccessor *accessor READ accessor CONSTANT FINAL)\n\n"
		 << "public:\n"
		 << "\texplicit " << settings.name.value() << "(QObject *parent = nullptr);\n"
		 << "\tQtMvvm::ISettingsAccessor *accessor() const;\n\n";



	_hdr << "\nprivate:\n"
		 << "\tQtMvvm::ISettingsAccessor *_accessor;\n"
		 << "};\n\n"
		 << "#endif //" << incGuard << '\n';
}

void QmlSettingsGenerator::writeSource(const SettingsGeneratorBase::SettingsType &settings)
{

}
