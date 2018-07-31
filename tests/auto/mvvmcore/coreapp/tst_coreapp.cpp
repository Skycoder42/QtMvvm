#include <QtTest>
#include <QtMvvmCore/ServiceRegistry>
#include "testapp.h"
#include "testviewmodel.h"
using namespace QtMvvm;

QTMVVM_REGISTER_CORE_APP(TestApp)

class CoreAppTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void testAppBoot();
	void testTypeMapping_data();
	void testTypeMapping();

	void testPresentVm();
	void testPresentVmArgs();
	void testPresentVmChild();
	void testPresentVmForResult();
	void testPresentVmContainer();
	void testPresentVmSingleton();

	void testPresentDialog();
	void testPresentMessage_data();
	void testPresentMessage();
	void testProgressMessage();
};

void CoreAppTest::initTestCase()
{
	qRegisterMetaType<TestViewModel*>();
	qRegisterMetaType<TestSingleViewModel*>();
	QtMvvm::ServiceRegistry::instance()->registerInterface<QtMvvm::IPresenter, TestPresenter>();
}

void CoreAppTest::testAppBoot()
{
	QVERIFY(coreApp);
	QVERIFY(coreApp->wasRegistered);
	QVERIFY(coreApp->startArgs.isEmpty());

	QSignalSpy startSpy{coreApp, &TestApp::appStarted};
	QCoreApplication::processEvents();
	QVERIFY(!coreApp->startArgs.isEmpty());
	QCOMPARE(coreApp->startArgs, QCoreApplication::arguments());
	QCOMPARE(startSpy.size(), 1);
}

void CoreAppTest::testTypeMapping_data()
{
	QTest::addColumn<QByteArray>("type");
	QTest::addColumn<int>("targetType");
	QTest::addColumn<QVariant>("value");
	QTest::addColumn<bool>("needRegister");
	QTest::addColumn<QVariant>("result");

	QTest::newRow("builtin.switch") << QByteArrayLiteral("switch")
									<< static_cast<int>(QMetaType::Bool)
									<< QVariant{QStringLiteral("true")}
									<< false
									<< QVariant{true};

	QTest::newRow("builtin.string") << QByteArrayLiteral("string")
									<< static_cast<int>(QMetaType::QString)
									<< QVariant{QStringLiteral("test")}
									<< false
									<< QVariant{QStringLiteral("test")};

	QTest::newRow("builtin.number") << QByteArrayLiteral("number")
									<< static_cast<int>(QMetaType::Double)
									<< QVariant{QStringLiteral("4.2")}
									<< false
									<< QVariant{4.2};

	QTest::newRow("builtin.range") << QByteArrayLiteral("range")
								   << static_cast<int>(QMetaType::Int)
								   << QVariant{QStringLiteral("55")}
								   << false
								   << QVariant{55};

	QTest::newRow("builtin.date") << QByteArrayLiteral("date")
								  << static_cast<int>(QMetaType::QDateTime)
								  << QVariant{QStringLiteral("2018-10-10T10:10")}
								  << false
								  << QVariant{QDateTime{{2018, 10, 10}, {10, 10}}};

	QTest::newRow("builtin.color") << QByteArrayLiteral("color")
								   << static_cast<int>(QMetaType::QColor)
								   << QVariant{QStringLiteral("#123456")}
								   << false
								   << QVariant{QColor{0x12, 0x34, 0x56}};

	QTest::newRow("builtin.url") << QByteArrayLiteral("url")
								 << static_cast<int>(QMetaType::QUrl)
								 << QVariant{QStringLiteral("file:///root.txt")}
								 << false
								 << QVariant{QUrl::fromLocalFile(QStringLiteral("/root.txt"))};

	QTest::newRow("builtin.var") << QByteArrayLiteral("var")
								 << static_cast<int>(QMetaType::QVariant)
								 << QVariant{42}
								 << false
								 << QVariant{42};

	QTest::newRow("builtin.variant") << QByteArrayLiteral("variant")
									 << static_cast<int>(QMetaType::QVariant)
									 << QVariant{43}
									 << false
									 << QVariant{43};

	QTest::newRow("builtin.selection") << QByteArrayLiteral("selection")
									   << static_cast<int>(QMetaType::QVariant)
									   << QVariant{44}
									   << false
									   << QVariant{44};

	QTest::newRow("builtin.list") << QByteArrayLiteral("list")
								  << static_cast<int>(QMetaType::QVariant)
								  << QVariant{45}
								  << false
								  << QVariant{45};

	QTest::newRow("builtin.radiolist") << QByteArrayLiteral("radiolist")
									   << static_cast<int>(QMetaType::QVariant)
									   << QVariant{46}
									   << false
									   << QVariant{46};

	QTest::newRow("known") << QByteArrayLiteral("int")
						   << static_cast<int>(QMetaType::Int)
						   << QVariant{QStringLiteral("66")}
						   << false
						   << QVariant{66};

	QTest::newRow("custom") << QByteArrayLiteral("salt")
							<< static_cast<int>(QMetaType::Double)
							<< QVariant{QStringLiteral("0.5")}
							<< true
							<< QVariant{0.5};

	QTest::newRow("invalid.unknown") << QByteArrayLiteral("sugar")
									 << static_cast<int>(QMetaType::Int)
									 << QVariant{QStringLiteral("66")}
									 << false
									 << QVariant{};

	QTest::newRow("invalid.inconvertible") << QByteArrayLiteral("color")
										   << static_cast<int>(QMetaType::QColor)
										   << QVariant{true}
										   << false
										   << QVariant{};
}

void CoreAppTest::testTypeMapping()
{
	QFETCH(QByteArray, type);
	QFETCH(int, targetType);
	QFETCH(QVariant, value);
	QFETCH(bool, needRegister);
	QFETCH(QVariant, result);

	if(needRegister)
		CoreApp::registerInputTypeMapping(type, targetType);

	auto res = CoreApp::safeCastInputType(type, value);
	if(result.isValid()) {
		if(targetType == QMetaType::QVariant)
			QVERIFY(res.userType() != targetType);
		else
			QCOMPARE(res.userType(), targetType);
		QVERIFY(res.isValid());
		QCOMPARE(res, result);
	} else {
		QCOMPARE(res.userType(), value.userType());
		QVERIFY(res.userType() != targetType);
	}
}

void CoreAppTest::testPresentVm()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[0]).isEmpty());
	QVERIFY(!std::get<2>(presenter->presented[0]));
}

void CoreAppTest::testPresentVmArgs()
{
	QVariantHash params {
		{QStringLiteral("key"), QStringLiteral("value")},
		{QStringLiteral("baum"), 42}
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>(params);

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[0]), params);
	QVERIFY(!std::get<2>(presenter->presented[0]));
}

void CoreAppTest::testPresentVmChild()
{
	QVariantHash params {
		{QStringLiteral("key"), QStringLiteral("value")},
		{QStringLiteral("baum"), 42}
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = static_cast<TestViewModel*>(std::get<0>(presenter->presented[0]));
	QVERIFY(vm);

	vm->presenChild(params);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[1]), params);
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
}

void CoreAppTest::testPresentVmForResult()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = static_cast<TestViewModel*>(std::get<0>(presenter->presented[0]));
	QVERIFY(vm);

	// show with result
	vm->presentResult(42);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[1]).isEmpty());
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);

	QVariant result{5.5};
	emit child->resultReady(result);
	QCOMPARE(vm->results.size(), 1);
	QCOMPARE(std::get<0>(vm->results[0]), 42u);
	QCOMPARE(std::get<1>(vm->results[0]), result);
	delete child;
	QCOMPARE(vm->results.size(), 1);

	//show without result
	vm->presentResult(24);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 3);
	QCOMPARE(presenter->presented.size(), 3);

	child = std::get<0>(presenter->presented[2]);
	QVERIFY(child);

	delete child;
	QCOMPARE(vm->results.size(), 2);
	QCOMPARE(std::get<0>(vm->results[1]), 24u);
	QVERIFY(!std::get<1>(vm->results[1]).isValid());
}

void CoreAppTest::testPresentVmContainer()
{
	QVariantHash initMap {
		{QStringLiteral("qtmvvm_container_for"), QByteArray{TestContainedViewModel::staticMetaObject.className()}},
		{QStringLiteral("qtmvvm_child_params"), QVariantHash{}}
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestContainedViewModel>();

	while(presentSpy.size() < 2)
		QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[0]), initMap);
	QVERIFY(!std::get<2>(presenter->presented[0]));

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestContainedViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[1]).isEmpty());
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
}

void CoreAppTest::testPresentVmSingleton()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestSingleViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestSingleViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[0]).isEmpty());
	QVERIFY(!std::get<2>(presenter->presented[0]));

	QSignalSpy instSpy{vm, &ViewModel::instanceInvoked};

	CoreApp::show<TestSingleViewModel>();
	QVERIFY(!presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);
	QCOMPARE(instSpy.size(), 1);

	// test single container
	CoreApp::show<TestContainedSingleViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestContainedSingleViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[1]).isEmpty());
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
	QCOMPARE(instSpy.size(), 2);
}

void CoreAppTest::testPresentDialog()
{
	Q_UNIMPLEMENTED();
}

void CoreAppTest::testPresentMessage_data()
{
	Q_UNIMPLEMENTED();
}

void CoreAppTest::testPresentMessage()
{
	Q_UNIMPLEMENTED();
}

void CoreAppTest::testProgressMessage()
{
	Q_UNIMPLEMENTED();
}

QTEST_MAIN(CoreAppTest)

#include "tst_coreapp.moc"
