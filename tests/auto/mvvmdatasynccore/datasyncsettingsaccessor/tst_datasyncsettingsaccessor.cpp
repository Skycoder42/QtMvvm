#include <QtTest>
#include <QtMvvmDataSyncCore/DataSyncSettingsAccessor>
#include <QtDataSync/Setup>
#include "../../../shared/tst_isettingsaccessor.h"
using namespace QtDataSync;
using namespace QtMvvm;

class DataSyncSettingsAccessorTest : public ISettingsAccessorTest
{
	Q_OBJECT

protected:
	ISettingsAccessor *createFirst() override;
	ISettingsAccessor *createSecond() override;
	bool testSyncChangeSignals() override;

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

private:
	QTemporaryDir tDir;
};

ISettingsAccessor *DataSyncSettingsAccessorTest::createFirst()
{
	return new DataSyncSettingsAccessor{this};
}

ISettingsAccessor *DataSyncSettingsAccessorTest::createSecond()
{
	return new DataSyncSettingsAccessor{this};
}

bool DataSyncSettingsAccessorTest::testSyncChangeSignals()
{
	return true;
}

void DataSyncSettingsAccessorTest::initTestCase()
{
	Setup{}.setLocalDir(tDir.path())
			.setKeyStoreProvider(QStringLiteral("plain"))
			.create();
}

void DataSyncSettingsAccessorTest::cleanupTestCase()
{
	delete first;
	delete second;
	Setup::removeSetup(DefaultSetup, true);
	tDir.remove();
}

QTEST_MAIN(DataSyncSettingsAccessorTest)

#include "tst_datasyncsettingsaccessor.moc"
