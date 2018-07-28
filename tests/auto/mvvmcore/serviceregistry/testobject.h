#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtMvvmCore/Injection>

#define CREATE_TEST_OBJECT(index, ...) \
	class TestObject  ## index : public QObject \
	{ \
		Q_OBJECT \
	\
	public: \
		__VA_ARGS__ \
	\
		Q_INVOKABLE explicit inline TestObject ## index(QObject *parent = nullptr) : \
			QObject{parent} \
		{} \
	};

#define DEP(index) \
	TestObject ## index *dep = nullptr; \
	QTMVVM_INJECT_PROP(TestObject ## index *, dep, dep)

#define DESTROYER \
	QSharedPointer<TestObject0> holder;

#define SOMETIME_DESTROYER(index) \
	DESTROYER \
	inline ~TestObject ## index() { qDebug(Q_FUNC_INFO); }

#define NEVER_DESTROYER(index) \
	DESTROYER \
	inline ~TestObject ## index() { Q_ASSERT_X(false, Q_FUNC_INFO, "destructor of never-destroy-object called"); }

CREATE_TEST_OBJECT(0)

CREATE_TEST_OBJECT(1, int data = 42;)
CREATE_TEST_OBJECT(2, int data = 42;)
CREATE_TEST_OBJECT(3, int data = 42;)

CREATE_TEST_OBJECT(4)
CREATE_TEST_OBJECT(5)
CREATE_TEST_OBJECT(6, DEP(4))
CREATE_TEST_OBJECT(7, DEP(5))
CREATE_TEST_OBJECT(8, bool hasBoth = false;)

CREATE_TEST_OBJECT(9, DEP(0))
CREATE_TEST_OBJECT(10, DEP(9))

CREATE_TEST_OBJECT(11, SOMETIME_DESTROYER(11))
CREATE_TEST_OBJECT(12, SOMETIME_DESTROYER(12))
CREATE_TEST_OBJECT(13, SOMETIME_DESTROYER(13))
CREATE_TEST_OBJECT(14, NEVER_DESTROYER(14))

#endif // TESTOBJECT_H
