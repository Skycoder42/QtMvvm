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
	QCOMPARE(settings->variantEntry.key(), QStringLiteral("tests/variantEntry"));
	QCOMPARE(settings->listEntry.key(), QStringLiteral("tests/listEntry"));
	QCOMPARE(settings->listEntry.dummyChild.key(), QStringLiteral("tests/listEntry/dummyChild"));

	//verify defaults
	QCOMPARE(settings->emptyEntry.get(), false);
	QCOMPARE(settings->advancedEntry.get(), QStringLiteral("Hello World"));
	QCOMPARE(settings->codeEntry.get(), QUrl::fromLocalFile(QStringLiteral("/path/to/something")));
	QCOMPARE(settings->parentNode.fullChildNode.replaceEntry.get(), 42);
	QCOMPARE(settings->parentNode.parentEntry.get(), true);
	QCOMPARE(settings->parentNode.parentEntry.nodeWithCodeEntry.get(), 43);
	QCOMPARE(settings->parentNode.parentEntry.leafEntry.get(), QStringLiteral("translate me"));
	QCOMPARE(settings->variantEntry.get(), QVariant{});
	QByteArrayList iList{"test1", "test2", "test3"};
	QCOMPARE(settings->listEntry.get(), iList);
	QCOMPARE(settings->listEntry.dummyChild.get(), false);

	//verify read/write (just on a single entry, they work all the same)
	auto tKey = QStringLiteral("tests/advancedEntry");
	QVERIFY(!tBackend->_data.contains(tKey));
	QString cValue;
	settings->advancedEntry.addChangeCallback([&](QString value) {
		cValue = std::move(value);
	});
	QString tValue = QStringLiteral("test42");
	QCOMPARE(settings->advancedEntry.key(), tKey);
	settings->advancedEntry = tValue;
	QCOMPARE(tBackend->_data.value(tKey).toString(), tValue);
	QCOMPARE(cValue, tValue);
	QVERIFY(settings->advancedEntry.isSet());
	QCOMPARE(settings->advancedEntry, tValue);

	//verify list stuff
	QVERIFY(settings->listEntry.isSet());
	QCOMPARE(settings->listEntry.size(), 3);
	QCOMPARE(settings->listEntry.getAt(0), "test1");
	QCOMPARE(settings->listEntry[1].get(), "test2");
	QCOMPARE(static_cast<QByteArray>(settings->listEntry[2]), "test3");
	QCOMPARE(settings->listEntry.getAt(3), "Hello World");

	settings->listEntry.push("baum");
	QCOMPARE(settings->listEntry.size(), 4);
	QCOMPARE(settings->listEntry.getAt(3), "baum");
	settings->listEntry.setAt(1, "tree");
	settings->listEntry[2] = "eetr";
	QCOMPARE(settings->listEntry.size(), 4);
	QCOMPARE(settings->listEntry.getAt(1), "tree");
	QCOMPARE(settings->listEntry.getAt(2), "eetr");
	settings->listEntry += "baum42";
	QCOMPARE(settings->listEntry.size(), 5);
	QCOMPARE(settings->listEntry.getAt(4), "baum42");
	QCOMPARE(settings->listEntry.pop(), "baum42");
	QCOMPARE(settings->listEntry.size(), 4);
	QCOMPARE(settings->listEntry.getAt(4), "Hello World");
	settings->listEntry.chop(10);
	QCOMPARE(settings->listEntry.size(), 0);
	QVERIFY(settings->listEntry.isSet());

	settings->listEntry.reset(false);
	QVERIFY(!settings->listEntry.isSet());
	QCOMPARE(settings->listEntry.size(), 0);
	settings->listEntry.reset(true);
	QVERIFY(settings->listEntry.isSet());
	QCOMPARE(settings->listEntry.size(), 3);

	for(const auto &elem : qAsConst(settings->listEntry))
		qDebug() << elem.get() << static_cast<QByteArray>(elem);
	for(auto &elem : settings->listEntry) {
		elem = "test1";
		elem.set("test2");
	}
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
