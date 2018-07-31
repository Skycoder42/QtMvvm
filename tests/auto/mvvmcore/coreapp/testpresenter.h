#ifndef TESTPRESENTER_H
#define TESTPRESENTER_H

#include <QObject>
#include <QtMvvmCore/IPresenter>

class TestPresenter : public QObject, public QtMvvm::IPresenter
{
	Q_OBJECT
	Q_INTERFACES(QtMvvm::IPresenter)

public:
	Q_INVOKABLE explicit TestPresenter(QObject *parent = nullptr);

	void present(QtMvvm::ViewModel *viewModel, const QVariantHash &params, QPointer<QtMvvm::ViewModel> parent) override;
	void showDialog(const QtMvvm::MessageConfig &config, QtMvvm::MessageResult *result) override;

	QList<std::tuple<QtMvvm::ViewModel*, QVariantHash, QPointer<QtMvvm::ViewModel>>> presented;
	QList<std::tuple<QtMvvm::MessageConfig, QtMvvm::MessageResult*>> dialogs;

Q_SIGNALS:
	void presentDone();
	void dialogDone();
};

#endif // TESTPRESENTER_H
