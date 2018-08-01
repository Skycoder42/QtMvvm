#ifndef TST_ISETTINGSACCESSOR_H
#define TST_ISETTINGSACCESSOR_H

#include <QtCore/QObject>
#include <QtMvvmCore/ISettingsAccessor>
#include <QtTest>

class ISettingsAccessorTest : public QObject
{
	Q_OBJECT

protected:
	QtMvvm::ISettingsAccessor *first = nullptr;
	QtMvvm::ISettingsAccessor *second = nullptr;

	virtual QtMvvm::ISettingsAccessor *createFirst() = 0;
	virtual QtMvvm::ISettingsAccessor *createSecond() = 0;
	virtual bool testSyncChangeSignals() = 0;

private Q_SLOTS:
	void testAccessorValid() {
		first = createFirst();
		QVERIFY(first);
	}

	void testSimpleOperations() {
		QSignalSpy changedSpy{first, &QtMvvm::ISettingsAccessor::entryChanged};
		QSignalSpy removedSpy{first, &QtMvvm::ISettingsAccessor::entryRemoved};

		auto key = QStringLiteral("test/key");
		QVERIFY(!first->contains(key));
		QCOMPARE(first->load(key, 24).toInt(), 24);
		QVERIFY(changedSpy.isEmpty());
		QVERIFY(removedSpy.isEmpty());

		first->save(key, 42);
		QVERIFY(first->contains(key));
		QCOMPARE(first->load(key, 24).toInt(), 42);
		if(changedSpy.isEmpty())
			QVERIFY(changedSpy.wait());
		QCOMPARE(changedSpy.size(), 1);
		auto changeEvent = changedSpy.takeFirst();
		QCOMPARE(changeEvent[0].toString(), key);
		QCOMPARE(changeEvent[1].toInt(), 42);
		QVERIFY(removedSpy.isEmpty());

		first->remove(key);
		QVERIFY(!first->contains(key));
		QCOMPARE(first->load(key, 24).toInt(), 24);
		if(removedSpy.isEmpty())
			QVERIFY(removedSpy.wait());
		QCOMPARE(removedSpy.size(), 1);
		QCOMPARE(removedSpy.takeFirst()[0].toString(), key);
		QVERIFY(changedSpy.isEmpty());
	}

	void testBatchRemove() {
		auto key0 = QStringLiteral("group");
		auto key1 = QStringLiteral("group/key");
		auto key2 = QStringLiteral("group/key/subkey");
		auto key3 = QStringLiteral("group/key/subgroup/subkey");

		first->save(key0, true);
		first->save(key1, true);
		first->save(key2, true);
		first->save(key3, true);

		QSignalSpy removedSpy{first, &QtMvvm::ISettingsAccessor::entryRemoved};
		first->remove(key1);
		while(removedSpy.size() < 3)
			QVERIFY(removedSpy.wait());

		QCOMPARE(removedSpy.size(), 3);
		QStringList eventKeys {
			removedSpy[0][0].toString(),
			removedSpy[1][0].toString(),
			removedSpy[2][0].toString()
		};
		QVERIFY(eventKeys.contains(key1));
		QVERIFY(eventKeys.contains(key2));
		QVERIFY(eventKeys.contains(key3));

		QVERIFY(!removedSpy.wait(1000));
	}

	void testSync() {
		second = createSecond();
		QVERIFY(second);
		QSignalSpy changedSpy{second, &QtMvvm::ISettingsAccessor::entryChanged};
		QSignalSpy removedSpy{first, &QtMvvm::ISettingsAccessor::entryRemoved};

		auto key = QStringLiteral("sync/key");
		first->save(key, 13);
		first->sync();
		second->sync();
		if(!testSyncChangeSignals())
			QEXPECT_FAIL("", "The testes accessor does not support notifying changes over multiple instances", Continue);
		if(changedSpy.isEmpty())
			QVERIFY(changedSpy.wait());
		if(!changedSpy.isEmpty()) {
			QCOMPARE(changedSpy.size(), 1);
			auto changeEvent = changedSpy.takeFirst();
			QCOMPARE(changeEvent[0].toString(), key);
			QCOMPARE(changeEvent[1].toInt(), 13);
		}
		QCOMPARE(second->load(key, 24).toInt(), 13);

		second->remove(key);
		second->sync();
		first->sync();
		if(!testSyncChangeSignals())
			QEXPECT_FAIL("", "The testes accessor does not support notifying changes over multiple instances", Continue);
		if(removedSpy.isEmpty())
			QVERIFY(removedSpy.wait());
		if(!removedSpy.isEmpty()) {
			QCOMPARE(removedSpy.size(), 1);
			QCOMPARE(removedSpy.takeFirst()[0].toString(), key);
		}
		QCOMPARE(first->load(key, 24).toInt(), 24);
	}
};

#endif // TST_ISETTINGSACCESSOR_H
