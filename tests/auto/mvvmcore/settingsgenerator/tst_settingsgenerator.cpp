#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QtMvvmCore/QSettingsAccessor>
#include "generatortest.h"
#include "importtest.h"

class SettingsGeneratorTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testSettingsGenerator();
	void testImportedSettings();

private:
	void trStuff();
};


void SettingsGeneratorTest::testSettingsGenerator()
{
	auto settings = TestSettings::instance();
	QVERIFY(settings);

	// verify backend
	auto tBackend = qobject_cast<TestBackend*>(settings->accessor());
	QVERIFY(tBackend);
	QCOMPARE(tBackend->parent(), settings);
	QCOMPARE(tBackend->_name, QStringLiteral("Test Backend"));
	QCOMPARE(tBackend->_code, 42);

	//verify keys
	QCOMPARE(settings->emptyEntry.key(), QStringLiteral("tests/emptyEntry"));
	QCOMPARE(settings->advancedEntry.key(), QStringLiteral("tests/advancedEntry"));
	QCOMPARE(settings->codeEntry.key(), QStringLiteral("tests/codeEntry"));
	QCOMPARE(settings->parentNode.fullChildNode.replaceEntry.key(), QStringLiteral("tests/parentNode/fullChildNode/replaceEntry"));
	QCOMPARE(settings->parentNode.parentEntry.key(), QStringLiteral("tests/parentNode/parentEntry"));
	QCOMPARE(settings->parentNode.parentEntry.nodeWithCodeEntry.key(), QStringLiteral("tests/parentNode/parentEntry/nodeWithCodeEntry"));
	QCOMPARE(settings->parentNode.parentEntry.leafEntry.key(), QStringLiteral("tests/parentNode/parentEntry/leafEntry"));

	//verify defaults
	QCOMPARE(settings->emptyEntry.get(), false);
	QCOMPARE(settings->advancedEntry.get(), QStringLiteral("Hello World"));
	QCOMPARE(settings->codeEntry.get(), QUrl::fromLocalFile(QStringLiteral("/path/to/something")));
	QCOMPARE(settings->parentNode.fullChildNode.replaceEntry.get(), 42);
	QCOMPARE(settings->parentNode.parentEntry.get(), true);
	QCOMPARE(settings->parentNode.parentEntry.nodeWithCodeEntry.get(), 43);
	QCOMPARE(settings->parentNode.parentEntry.leafEntry.get(), QStringLiteral("translate me"));

	//verify read/write (just on a single entry, they work all the same)
	Q_ASSERT(tBackend->_data.isEmpty());
	QString cValue;
	settings->advancedEntry.addChangeCallback([&](QString value) {
		cValue = std::move(value);
	});
	QString tValue = QStringLiteral("test42");
	auto tKey = QStringLiteral("tests/advancedEntry");
	QCOMPARE(settings->advancedEntry.key(), tKey);
	settings->advancedEntry = tValue;
	QCOMPARE(tBackend->_data.value(tKey).toString(), tValue);
	QCOMPARE(cValue, tValue);
	QVERIFY(settings->advancedEntry.isSet());
	QCOMPARE(static_cast<QString>(settings->advancedEntry), tValue);
}

void SettingsGeneratorTest::testImportedSettings()
{
	auto settings = TestImportSettings::instance();
	QVERIFY(settings);
	QVERIFY(qobject_cast<QtMvvm::QSettingsAccessor*>(settings->accessor()));

	//verify keys
	QCOMPARE(settings->impRoot.importedEntry.key(), QStringLiteral("impRoot/importedEntry"));
	QCOMPARE(settings->node1.key(), QStringLiteral("node1"));
	QCOMPARE(settings->node1.entry1.key(), QStringLiteral("node1/entry1"));
	QCOMPARE(settings->node1.entry2.key(), QStringLiteral("node1/entry2"));
	QCOMPARE(settings->node2.entry1.key(), QStringLiteral("node2/entry1"));

	//verify defaults
	QCOMPARE(settings->impRoot.importedEntry.get(), false);
	QCOMPARE(settings->node1.get(), QUrl{});
	QCOMPARE(settings->node1.entry1.get(), true);
	QCOMPARE(settings->node1.entry2.get(), 42);
	QCOMPARE(settings->node2.entry1.get(), QString{});
}

void SettingsGeneratorTest::trStuff()
{
	tr("somet translated text...");
}

QTEST_MAIN(SettingsGeneratorTest)

#include "tst_settingsgenerator.moc"
