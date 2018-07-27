#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

#include "cppsettingsgenerator.h"
#include "qmlsettingsgenerator.h"
#include "settingstranslator.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE_PREFIX));

	QCommandLineParser parser;
	parser.setApplicationDescription(QStringLiteral("A tool to create C++ and QML bindings for generic settings access"));
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 {QStringLiteral("tr"), QStringLiteral("translate")},
						 QStringLiteral("Translate the given settings file. The output will be"
						 "a dummy cpp file, writte to --impl.")
					 });
	parser.addOption({
						 QStringLiteral("qml"),
						 QStringLiteral("Generate the QML-bindings for the given settings xml "
						 "instead of the default cpp bindings.")
					 });
	parser.addOption({
						 QStringLiteral("in"),
						 QStringLiteral("The input XML <file> containing the settings definition"),
						 QStringLiteral("file")
					 });
	parser.addOption({
						 QStringLiteral("header"),
						 QStringLiteral("The <name> of the header file to generate"),
						 QStringLiteral("name")
					 });
	parser.addOption({
						 QStringLiteral("impl"),
						 QStringLiteral("The <name> of the implementation file to generate"),
						 QStringLiteral("name")
					 });

	parser.process(a);

	if(parser.isSet(QStringLiteral("translate"))) {
		try {
			SettingsTranslator translator {parser.value(QStringLiteral("impl"))};
			translator.process(parser.value(QStringLiteral("in")));
			return EXIT_SUCCESS;
		} catch (SettingsTranslator::Exception &e) {
			qCritical() << e.what();
			return EXIT_FAILURE;
		}
	} else if(parser.isSet(QStringLiteral("qml"))) {
		try {
			QmlSettingsGenerator generator {
				parser.value(QStringLiteral("header")),
				parser.value(QStringLiteral("impl"))
			};
			generator.process(parser.value(QStringLiteral("in")));
			return EXIT_SUCCESS;
		} catch (SettingsGeneratorImpl::Exception &e) {
			qCritical() << e.what();
			return EXIT_FAILURE;
		}
	} else {
		try {
			CppSettingsGenerator generator {
				parser.value(QStringLiteral("header")),
				parser.value(QStringLiteral("impl"))
			};
			generator.process(parser.value(QStringLiteral("in")));
			return EXIT_SUCCESS;
		} catch (SettingsGeneratorImpl::Exception &e) {
			qCritical() << e.what();
			return EXIT_FAILURE;
		}
	}
}
