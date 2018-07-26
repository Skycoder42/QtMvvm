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
	QCOMPARE(settings->simpleListEntry.key(), QStringLiteral("tests/simpleListEntry"));
	QCOMPARE(settings->listNode.key(), QStringLiteral("tests/listNode"));

	//verify defaults
	QCOMPARE(settings->emptyEntry.get(), false);
	QCOMPARE(settings->advancedEntry.get(), QStringLiteral("Hello World"));
	QCOMPARE(settings->codeEntry.get(), QUrl::fromLocalFile(QStringLiteral("/path/to/something")));
	QCOMPARE(settings->parentNode.fullChildNode.replaceEntry.get(), 42);
	QCOMPARE(settings->parentNode.parentEntry.get(), true);
	QCOMPARE(settings->parentNode.parentEntry.nodeWithCodeEntry.get(), 43);
	QCOMPARE(settings->parentNode.parentEntry.leafEntry.get(), QStringLiteral("translate me"));
	QCOMPARE(settings->variantEntry.get(), QVariant{});
	QCOMPARE(settings->simpleListEntry.get(), QList<int>{42});

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
	QVERIFY(!settings->listNode.isSet());
	QCOMPARE(settings->listNode.size(), 0);
	settings->listNode.push();
	QVERIFY(settings->listNode.isSet());
	QCOMPARE(settings->listNode.size(), 1);

	QCOMPARE(settings->listNode.at(0).simpleChild.key(), QStringLiteral("tests/listNode/0/simpleChild"));
	QCOMPARE(settings->listNode[0].someNode.deepChild.key(), QStringLiteral("tests/listNode/0/someNode/deepChild"));
	QCOMPARE(settings->listNode[0].someNode.deepParent.key(), QStringLiteral("tests/listNode/0/someNode/deepParent"));
	QCOMPARE(settings->listNode[0].someNode.deepParent.simpleChild.key(), QStringLiteral("tests/listNode/0/someNode/deepParent/simpleChild"));
	QCOMPARE(settings->listNode[0].childList.key(), QStringLiteral("tests/listNode/0/childList"));

	QCOMPARE(settings->listNode.at(0).simpleChild, false);
	QCOMPARE(settings->listNode[0].someNode.deepChild, 22);
	QCOMPARE(settings->listNode[0].someNode.deepParent, QStringLiteral("___"));
	QCOMPARE(settings->listNode[0].someNode.deepParent.simpleChild, true);
	QVERIFY(!settings->listNode[0].childList.isSet());
	QCOMPARE(settings->listNode[0].childList.size(), 0);

	int resVal = -1;
	settings->listNode[0].someNode.deepChild.addChangeCallback([&](int val){
		resVal = val;
	});
	settings->listNode[0].someNode.deepChild = 47;
	QCOMPARE(resVal, 47);
	resVal = -1;

	{
		auto &newEntry = settings->listNode.push();
		QCOMPARE(settings->listNode.size(), 2);
		QCOMPARE(newEntry.simpleChild, false);
		QCOMPARE(settings->listNode[1].simpleChild.key(), QStringLiteral("tests/listNode/1/simpleChild"));
		QCOMPARE(settings->listNode[1].simpleChild, false);
		newEntry.simpleChild = true;
		QCOMPARE(newEntry.simpleChild, true);
		QCOMPARE(settings->listNode[1].simpleChild, true);
		QCOMPARE(settings->listNode[0].simpleChild, false);
	}

	{
		auto newEntry = settings->listNode.push_deferred();
		QCOMPARE(settings->listNode.size(), 2);
		QCOMPARE(newEntry.element().someNode.deepChild, 22);
		QCOMPARE(settings->listNode[2].simpleChild.key(), QStringLiteral("tests/listNode/2/simpleChild"));
		QCOMPARE(settings->listNode[2].someNode.deepChild, 22);
		newEntry->someNode.deepChild = 44;
		QCOMPARE((*newEntry).someNode.deepChild, 44);
		QCOMPARE(settings->listNode[2].someNode.deepChild, 44);
		QCOMPARE(settings->listNode.size(), 2);
	}
	QCOMPARE(settings->listNode.size(), 3);
	QCOMPARE(resVal, -1); //should be unchanged

	for(const auto &elem : qAsConst(settings->listNode))
		qDebug() << elem.simpleChild.get();
	for(auto &elem : settings->listNode)
		elem.simpleChild = true;

	auto sizeChanged = false;
	settings->listNode.addChangeCallback([&](int size) {
		sizeChanged = (size == 1);
	});

	settings->listNode.pop(2);
	QVERIFY(sizeChanged);
	QCOMPARE(settings->listNode.size(), 1);
	QCOMPARE(settings->listNode[1].simpleChild, false);
	QCOMPARE(settings->listNode[2].someNode.deepChild, 22);

	settings->listNode.reset();
	QVERIFY(!settings->listNode.isSet());
	QCOMPARE(settings->listNode.size(), 0);
	QCOMPARE(resVal, 22); //should be resetted
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
