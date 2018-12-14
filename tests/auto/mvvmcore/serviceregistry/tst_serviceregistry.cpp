#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QtMvvmCore/ServiceRegistry>

#include "testinterface.h"
#include "testservice.h"
#include "testobject.h"
#include "plugintestinterface.h"
using namespace QtMvvm;

#if !defined(Q_CC_GNU) || !defined(Q_OS_WIN) || defined(QT_NO_DEBUG)
#define NO_MINGW_DEBUG_BUILD
#endif

class ServiceRegistryTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testRegistrations();
	void testPlainConstructions();
	void testDepedencyConstructions();
	void testInjection();

#ifdef NO_MINGW_DEBUG_BUILD
	void testPluginRegistration();
#endif

	void testDestructionScopes();

public:
	static QWeakPointer<TestObject0> weak0;
	static QWeakPointer<TestObject0> weak1;
	static QWeakPointer<TestObject0> weak2;
	static QWeakPointer<TestObject0> weak3;
};

QWeakPointer<TestObject0> ServiceRegistryTest::weak0;
QWeakPointer<TestObject0> ServiceRegistryTest::weak1;
QWeakPointer<TestObject0> ServiceRegistryTest::weak2;
QWeakPointer<TestObject0 >ServiceRegistryTest::weak3;

void ServiceRegistryTest::testRegistrations()
{
	try {
		QVERIFY(!ServiceRegistry::instance()->isRegistered<TestInterface>());
		QVERIFY(!ServiceRegistry::instance()->isRegistered<TestService>());
		QVERIFY(!ServiceRegistry::instance()->isRegistered<TestObject0>());
		QVERIFY_EXCEPTION_THROWN(ServiceRegistry::instance()->service<TestInterface>(), ServiceDependencyException);
		QVERIFY_EXCEPTION_THROWN(ServiceRegistry::instance()->service<TestObject0>(), ServiceDependencyException);

		//register service
		ServiceRegistry::instance()->registerInterface<TestInterface, WeakTestService>(ServiceRegistry::DestroyOnAppDestroy, true);
		QVERIFY(ServiceRegistry::instance()->isRegistered<TestInterface>());
		QVERIFY(ServiceRegistry::instance()->isRegistered(TestInterfaceIId));
		QVERIFY(!ServiceRegistry::instance()->isRegistered<WeakTestService>());
		QVERIFY(!ServiceRegistry::instance()->isRegistered<TestService>());

		ServiceRegistry::instance()->registerInterface<TestInterface, TestService>();
		QVERIFY(ServiceRegistry::instance()->isRegistered<TestInterface>());
		QVERIFY(!ServiceRegistry::instance()->isRegistered<TestService>());
#define TMP_REGISTER ServiceRegistry::instance()->registerInterface<TestInterface, WeakTestService>()
		QVERIFY_EXCEPTION_THROWN(TMP_REGISTER, ServiceExistsException);
#undef TMP_REGISTER

		QVERIFY(ServiceRegistry::instance()->service<TestInterface>());
		QVERIFY(ServiceRegistry::instance()->serviceObj(TestInterfaceIId));
		QVERIFY(dynamic_cast<TestService*>(ServiceRegistry::instance()->service<TestInterface>()));

		//register object
		ServiceRegistry::instance()->registerObject<TestObject0>();
		QVERIFY(ServiceRegistry::instance()->isRegistered<TestObject0>());
		QVERIFY(ServiceRegistry::instance()->service<TestObject0>());
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void ServiceRegistryTest::testPlainConstructions()
{
	try {
		// test default (meta) construction
		ServiceRegistry::instance()->registerObject<TestObject1>();
		auto obj1 = ServiceRegistry::instance()->service<TestObject1>();
		QVERIFY(obj1);
		QCOMPARE(obj1->data, 42);

		// test function construction
		ServiceRegistry::instance()->registerObject<TestObject2>([](){
			auto obj = new TestObject2{};
			obj->data = 13;
			return obj;
		});
		auto obj2 = ServiceRegistry::instance()->service<TestObject2>();
		QVERIFY(obj2);
		QCOMPARE(obj2->data, 13);

		// test instance construction
		auto obj3Instance = new TestObject3{};
		ServiceRegistry::instance()->registerObject(obj3Instance);
		auto obj3 = ServiceRegistry::instance()->service<TestObject3>();
		QCOMPARE(obj3, obj3Instance);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void ServiceRegistryTest::testDepedencyConstructions()
{
	try {
		//try valid (inverse order must work)
		ServiceRegistry::instance()->registerObject<TestObject6>();
		ServiceRegistry::instance()->registerObject<TestObject4>();
		auto obj = ServiceRegistry::instance()->service<TestObject6>();
		QVERIFY(obj);
		QVERIFY(obj->dep);

		//try without dep
		ServiceRegistry::instance()->registerObject<TestObject7>();
		QVERIFY_EXCEPTION_THROWN(ServiceRegistry::instance()->service<TestObject7>(), ServiceDependencyException);
		ServiceRegistry::instance()->registerObject<TestObject5>();
		QVERIFY(ServiceRegistry::instance()->service<TestObject7>());

		// try with fn
		ServiceRegistry::instance()->registerObject<TestObject8>([](TestObject6 *obj1, TestObject0 *obj2){
			auto retObj = new TestObject8{};
			retObj->hasBoth = obj1 && obj2;
			return retObj;
		});
		auto fnObj = ServiceRegistry::instance()->service<TestObject8>();
		QVERIFY(fnObj);
		QVERIFY(fnObj->hasBoth);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

void ServiceRegistryTest::testInjection()
{
	try {
		// try normal injection
		auto obj1 = new TestObject9{this};
		QVERIFY(!obj1->dep);
		ServiceRegistry::instance()->injectServices(obj1);
		QVERIFY(obj1->dep);
		obj1->deleteLater();

		//try construct injection
		auto obj2 = ServiceRegistry::instance()->constructInjected<TestObject9>(this);
		QVERIFY(obj2);
		QVERIFY(obj2->dep);
		obj2->deleteLater();

		//try invalid injection
		QVERIFY_EXCEPTION_THROWN(ServiceRegistry::instance()->constructInjected<TestObject10>(this), ServiceDependencyException);
		auto obj3 = new TestObject10{this};
		QVERIFY_EXCEPTION_THROWN(ServiceRegistry::instance()->injectServices(obj3), ServiceDependencyException);
		obj3->deleteLater();
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

#ifdef NO_MINGW_DEBUG_BUILD
void ServiceRegistryTest::testPluginRegistration()
{
	try {
#ifdef Q_OS_LINUX
		QVERIFY(QFile::exists(QStringLiteral(PLUGIN_TESTDIR "/serviceregistry/libserviceregistry_testplugin.so")));
#elif defined(Q_OS_DARWIN)
		QVERIFY(QFile::exists(QStringLiteral(PLUGIN_TESTDIR "/serviceregistry/libserviceregistry_testplugin.dylib")));
#elif defined(Q_OS_WIN)
#ifdef QT_NO_DEBUG
		QVERIFY(QFile::exists(QStringLiteral(PLUGIN_TESTDIR "/serviceregistry/serviceregistry_testplugin.dll")));
#else
		QVERIFY(QFile::exists(QStringLiteral(PLUGIN_TESTDIR "/serviceregistry/serviceregistry_testplugind.dll")));
#endif
#endif

		qputenv("QT_PLUGIN_PATH", PLUGIN_TESTDIR);
		ServiceRegistry::instance()->registerPlugin<PluginTestInterface>(QStringLiteral("serviceregistry"), QStringLiteral("test2"));
		QVERIFY(ServiceRegistry::instance()->isRegistered<PluginTestInterface>());
		QVERIFY(ServiceRegistry::instance()->service<PluginTestInterface>());
		QCOMPARE(ServiceRegistry::instance()->service<PluginTestInterface>()->magicNumber(), 72);
	} catch(QException &e) {
#ifdef __MINGW32__
		QEXPECT_FAIL("", "Plugin loading in debug builds is currently broken on mingw", Abort);
		QVERIFY2(false, e.what());
#else
		QFAIL(e.what());
#endif
	}
}
#endif

void ServiceRegistryTest::testDestructionScopes()
{
	try {
		{
			auto holder0 = QSharedPointer<TestObject0>::create();
			auto holder1 = QSharedPointer<TestObject0>::create();
			auto holder2 = QSharedPointer<TestObject0>::create();
			auto holder3 = QSharedPointer<TestObject0>::create();

			weak0 = holder0;
			weak1 = holder1;
			weak2 = holder2;
			weak3 = holder3;

			//register and create all destruction instances
			ServiceRegistry::instance()->registerObject<TestObject11>(ServiceRegistry::DestroyOnAppQuit);
			QVERIFY(ServiceRegistry::instance()->service<TestObject11>());
			ServiceRegistry::instance()->service<TestObject11>()->holder = holder0;
			ServiceRegistry::instance()->registerObject<TestObject12>(ServiceRegistry::DestroyOnAppDestroy);
			QVERIFY(ServiceRegistry::instance()->service<TestObject12>());
			ServiceRegistry::instance()->service<TestObject12>()->holder = holder1;
			ServiceRegistry::instance()->registerObject<TestObject13>(ServiceRegistry::DestroyOnRegistryDestroy);
			QVERIFY(ServiceRegistry::instance()->service<TestObject13>());
			ServiceRegistry::instance()->service<TestObject13>()->holder = holder2;
			ServiceRegistry::instance()->registerObject<TestObject14>(ServiceRegistry::DestroyNever);
			QVERIFY(ServiceRegistry::instance()->service<TestObject14>());
			ServiceRegistry::instance()->service<TestObject14>()->holder = holder3;
		}

		QVERIFY(weak0);
		QVERIFY(weak1);
		QVERIFY(weak2);
		QVERIFY(weak3);

		bool called = false;
		connect(qApp, &QCoreApplication::aboutToQuit, [&](){ //called after the method destructor
			QVERIFY(!weak0);
			QVERIFY(weak1);
			QVERIFY(weak2);
			QVERIFY(weak3);
			called = true;
		});

		// "run" the main eventloop to test the signal
		QTimer::singleShot(0, qApp, &QCoreApplication::quit);
		QCoreApplication::exec();
		QVERIFY(called);
		QVERIFY(!weak0);
		QVERIFY(weak1);
		QVERIFY(weak2);
		QVERIFY(weak3);
	} catch(QException &e) {
		QFAIL(e.what());
	}
}

// manual main for testing destruction
int main(int argc, char *argv[])
{
	int res = EXIT_FAILURE;
	{
		QCoreApplication app(argc, argv);
		app.setAttribute(Qt::AA_Use96Dpi, true);
		ServiceRegistryTest tc;
		QTEST_SET_MAIN_SOURCE_PATH
		res = QTest::qExec(&tc, argc, argv);
		Q_ASSERT(!ServiceRegistryTest::weak0);
		Q_ASSERT(ServiceRegistryTest::weak1);
		Q_ASSERT(ServiceRegistryTest::weak2);
		Q_ASSERT(ServiceRegistryTest::weak3);
	}

	Q_ASSERT(!ServiceRegistryTest::weak0);
	Q_ASSERT(!ServiceRegistryTest::weak1);
	Q_ASSERT(ServiceRegistryTest::weak2);
	Q_ASSERT(ServiceRegistryTest::weak3);

	return res;
}

#include "tst_serviceregistry.moc"
