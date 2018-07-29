#include <QtTest>
#include <QtCore>
#include <QtMvvmCore/QSettingsAccessor>
#include "../../../shared/tst_isettingsaccessor.h"
using namespace QtMvvm;

class QSettingsAccessorTest : public ISettingsAccessorTest
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
	QTemporaryFile *tempFile;
};

ISettingsAccessor *QSettingsAccessorTest::createFirst()
{
	auto settings = new QSettings{tempFile->fileName(), QSettings::IniFormat, this};
	auto accessor = new QSettingsAccessor{settings, this};
	settings->setParent(accessor);
	return accessor;
}

ISettingsAccessor *QSettingsAccessorTest::createSecond()
{
	return createFirst();
}

bool QSettingsAccessorTest::testSyncChangeSignals()
{
	return false;
}

void QSettingsAccessorTest::initTestCase()
{
	tempFile = new QTemporaryFile{this};
	QVERIFY(tempFile->open());
	tempFile->close();
}

void QSettingsAccessorTest::cleanupTestCase()
{
	delete tempFile;
}

QTEST_MAIN(QSettingsAccessorTest)

#include "tst_qsettingsaccessor.moc"
