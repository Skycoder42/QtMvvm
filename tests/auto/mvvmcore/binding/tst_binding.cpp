#include <QtTest>
#include <QtMvvmCore/Binding>
using namespace QtMvvm;

class BindingTest : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int vmProp MEMBER vmProp NOTIFY vmPropChanged)
	Q_PROPERTY(int vProp MEMBER vProp NOTIFY vPropChanged)

Q_SIGNALS:
	void vmPropChanged(int vmProp);
	void vPropChanged(int vProp);

	void triggerVm();
	void triggerV();

private Q_SLOTS:
	void testSingleInit();
	void testOneWayToView();
	void testOneWayToViewModel();
	void testTwoWay();
	void testCustomChangeSignals();

private:
	int vmProp = 0;
	int vProp = 0;
};

void BindingTest::testSingleInit()
{
	vmProp = 42;
	vProp = 0;
	auto binding = bind(this, "vmProp", this, "vProp", Binding::SingleInit);
	QVERIFY(binding);
	QCOMPARE(vmProp, 42);
	QCOMPARE(vProp, vmProp);

	vmProp = 33;
	emit vmPropChanged(vmProp);
	QCOMPARE(vProp, 42);

	emit vPropChanged(vProp);
	QCOMPARE(vmProp, 33);

	binding.unbind();
	QVERIFY(!binding.isValid());
}

void BindingTest::testOneWayToView()
{
	vmProp = 13;
	vProp = 0;
	auto binding = bind(this, "vmProp", this, "vProp", Binding::OneWayToView);
	QVERIFY(binding);
	QCOMPARE(vmProp, 13);
	QCOMPARE(vProp, vmProp);

	vmProp = 33;
	emit vmPropChanged(vmProp);
	QCOMPARE(vProp, vmProp);

	vProp = 13;
	emit vPropChanged(vProp);
	QCOMPARE(vmProp, 33);

	binding.unbind();
	QVERIFY(!binding.isValid());
}

void BindingTest::testOneWayToViewModel()
{
	vmProp = 0;
	vProp = 11;
	auto binding = bind(this, "vmProp", this, "vProp", Binding::OneWayToViewModel);
	QVERIFY(binding);
	QCOMPARE(vmProp, 0);
	QCOMPARE(vProp, 11);

	vmProp = 55;
	emit vmPropChanged(vmProp);
	QCOMPARE(vProp, 11);

	vProp = 5;
	emit vPropChanged(vProp);
	QCOMPARE(vmProp, vProp);

	binding.unbind();
	QVERIFY(!binding.isValid());
}

void BindingTest::testTwoWay()
{
	vmProp = 77;
	vProp = 0;
	auto binding = bind(this, "vmProp", this, "vProp", Binding::TwoWay);
	QVERIFY(binding);
	QCOMPARE(vmProp, 77);
	QCOMPARE(vProp, vmProp);

	vmProp = 12;
	emit vmPropChanged(vmProp);
	QCOMPARE(vProp, 12);
	QCOMPARE(vProp, vmProp);

	vProp = 9;
	emit vPropChanged(vProp);
	QCOMPARE(vmProp, 9);
	QCOMPARE(vmProp, vProp);

	binding.unbind();
	QVERIFY(!binding.isValid());
}

void BindingTest::testCustomChangeSignals()
{
	vmProp = 16;
	vProp = 0;
	auto binding = bind(this, "vmProp", this, "vProp", Binding::TwoWay, "triggerVm()", "triggerV()");
	QVERIFY(binding);
	QCOMPARE(vmProp, 16);
	QCOMPARE(vProp, vmProp);

	vmProp = 8;
	emit vmPropChanged(vmProp);
	QCOMPARE(vProp, 16);
	emit triggerVm();
	QCOMPARE(vProp, 8);
	QCOMPARE(vProp, vmProp);

	vProp = 99;
	emit vPropChanged(vProp);
	QCOMPARE(vmProp, 8);
	emit triggerV();
	QCOMPARE(vmProp, 99);
	QCOMPARE(vmProp, vProp);

	binding.unbind();
	QVERIFY(!binding.isValid());
}

QTEST_MAIN(BindingTest)

#include "tst_binding.moc"
