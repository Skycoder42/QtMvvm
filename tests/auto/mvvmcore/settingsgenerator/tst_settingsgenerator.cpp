#include <QString>
#include <QtTest>
#include <QCoreApplication>

class SettingsGeneratorTest : public QObject
{
	Q_OBJECT

public:
	SettingsGeneratorTest();

private Q_SLOTS:
	void testSettingsGeneration_data();
	void testSettingsGeneration();
};

SettingsGeneratorTest::SettingsGeneratorTest()
{
}

void SettingsGeneratorTest::testSettingsGeneration_data()
{
	QTest::addColumn<QString>("data");
	QTest::newRow("0") << QString();
}

void SettingsGeneratorTest::testSettingsGeneration()
{
	QFETCH(QString, data);
	QVERIFY2(true, "Failure");
}

QTEST_MAIN(SettingsGeneratorTest)

#include "tst_settingsgenerator.moc"
