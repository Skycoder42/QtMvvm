#ifndef RESULTVIEWMODEL_H
#define RESULTVIEWMODEL_H

#include <QtMvvmCore/ViewModel>

class ResultViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(QString result READ result WRITE setResult NOTIFY resultChanged)

public:
	Q_INVOKABLE explicit ResultViewModel(QObject *parent = nullptr);
	~ResultViewModel();

	QString result() const;

public Q_SLOTS:
	void done();

	void setResult(QString result);

Q_SIGNALS:
	void resultChanged(QString result);

protected:
	void onInit(const QVariantHash &params) override;

private:
	QString _result;
};

#endif // RESULTVIEWMODEL_H
