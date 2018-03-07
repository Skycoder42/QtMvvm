#ifndef QTMVVM_QQMLMVVMBINDING_H
#define QTMVVM_QQMLMVVMBINDING_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>

#include <QtMvvmCore/Binding>

namespace QtMvvm {

class QQmlMvvmBinding : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

	Q_PROPERTY(QObject* viewModel MEMBER _viewModel NOTIFY viewModelChanged)
	Q_PROPERTY(QString viewModelProperty MEMBER _viewModelProperty NOTIFY viewModelPropertyChanged)
	Q_PROPERTY(QObject* view MEMBER _view NOTIFY viewChanged)
	Q_PROPERTY(QString viewProperty MEMBER _viewProperty NOTIFY viewPropertyChanged)
	Q_PROPERTY(QString viewModelChangeSignal MEMBER _viewModelChangeSignal NOTIFY viewModelChangeSignalChanged)
	Q_PROPERTY(QString viewChangeSignal MEMBER _viewChangeSignal NOTIFY viewChangeSignalChanged)
	Q_PROPERTY(BindingDirection type READ type WRITE setType NOTIFY typeChanged) //MEMBER is broken for flags

public:
	enum BindingDirectionFlag { //copy flags from binding
		SingleInit = Binding::SingleInit,
		OneWayToView = Binding::OneWayToView,
		OneWayToViewModel = Binding::OneWayToViewModel,
		TwoWay = Binding::TwoWay
	};
	Q_DECLARE_FLAGS(BindingDirection, BindingDirectionFlag)
	Q_FLAG(BindingDirection)

	explicit QQmlMvvmBinding(QObject *parent = nullptr);

	void classBegin() override;
	void componentComplete() override;

	BindingDirection type() const;
	Q_INVOKABLE bool isValid() const;

public Q_SLOTS:
	void setType(BindingDirection type);
	void unbind();

Q_SIGNALS:
	void viewModelChanged(QObject* viewModel);
	void viewModelPropertyChanged(const QString &viewModelProperty);
	void viewChanged(QObject* view);
	void viewPropertyChanged(const QString &viewProperty);
	void typeChanged(BindingDirection type);
	void viewModelChangeSignalChanged(const QString &viewModelChangeSignal);
	void viewChangeSignalChanged(const QString &viewChangeSignal);

private Q_SLOTS:
	void resetBinding();

private:
	Binding _binding;
	bool _completed;

	QObject* _viewModel;
	QString _viewModelProperty;
	QObject* _view;
	QString _viewProperty;
	BindingDirection _type;
	QString _viewModelChangeSignal;
	QString _viewChangeSignal;
};

}

#endif // QTMVVM_QQMLMVVMBINDING_H
