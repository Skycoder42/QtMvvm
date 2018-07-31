#ifndef TESTVIEWMODEL_H
#define TESTVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

class TestViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit TestViewModel(QObject *parent = nullptr);

	void presenChild(const QVariantHash &params = {});
	void presentResult(quint32 code);

	QList<std::tuple<quint32, QVariant>> results;

public Q_SLOTS:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;
};
Q_DECLARE_METATYPE(TestViewModel*)

class TestSingleViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT
	QTMVVM_SINGLETON

public:
	Q_INVOKABLE explicit TestSingleViewModel(QObject *parent = nullptr);
};
Q_DECLARE_METATYPE(TestSingleViewModel*)

class TestContainedViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT
	QTMVVM_CONTAINER_VM(TestViewModel)

public:
	Q_INVOKABLE explicit TestContainedViewModel(QObject *parent = nullptr);
};

class TestContainedSingleViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT
	QTMVVM_CONTAINER_VM(TestSingleViewModel)

public:
	Q_INVOKABLE explicit TestContainedSingleViewModel(QObject *parent = nullptr);
};

#endif // TESTVIEWMODEL_H
