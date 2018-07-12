#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

#include "settingsgenerator.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE_PREFIX));

	QCommandLineParser parser;
	parser.setApplicationDescription(QStringLiteral("A tool to...")); //TODO description
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 {QStringLiteral("t"), QStringLiteral("translate")},
						 QStringLiteral("Translate the given settings file. The output will be"
						 "a dummy cpp file, writte to --impl.")
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

	try {
		SettingsGenerator generator {
			parser.value(QStringLiteral("header")),
			parser.value(QStringLiteral("impl"))
		};
		generator.process(parser.value(QStringLiteral("in")));
		return EXIT_SUCCESS;
	} catch (const QException &e) {
		qCritical() << e.what();
		return EXIT_FAILURE;
	}
}
