#ifndef QTMVVM_QQMLMVVMBINDING_H
#define QTMVVM_QQMLMVVMBINDING_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>

#include <QtMvvmCore/Binding>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtMvvm::Core {

/*! @brief A QML class to create a local mvvm multiway binding
 *
 * @extends QtQml.QtObject
 * @since 1.0
 *
 * It is basically a wrapper around the QtMvvm::bind method. The parameters are set via the
 * properties, and once the binding component is completed, it will create the binding. It is
 * possible to modify the properties while running. This will recreate the binding, discarding
 * the previous one
 *
 * @sa QtMvvm::bind
 */
class MvvmBinding : public QtObject
#else
namespace QtMvvm {

class QQmlMvvmBinding : public QObject, public QQmlParserStatus
#endif
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

	/*! @brief The object in the role of a viewmodel
	 *
	 * @default{`null`}
	 * @accessors{
	 *	@memberAc{viewModel}
	 *	@notifyAc{viewModelChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QObject* viewModel MEMBER _viewModel NOTIFY viewModelChanged)
	/*! @brief The property of the viewmodel to use in the binding
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewModelProperty}
	 *	@notifyAc{viewModelPropertyChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewModelProperty MEMBER _viewModelProperty NOTIFY viewModelPropertyChanged)
	/*! @brief The object in the role of a view
	 *
	 * @default{`parent()`}
	 * @accessors{
	 *	@memberAc{view}
	 *	@notifyAc{viewChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QObject* view MEMBER _view NOTIFY viewChanged)
	/*! @brief The property of the view to use in the binding
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewProperty}
	 *	@notifyAc{viewPropertyChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewProperty MEMBER _viewProperty NOTIFY viewPropertyChanged)
	/*! @brief An alternative signal to be used instead of the viewModelProperty notify signal
	 * to detect property changes
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewModelChangeSignal}
	 *	@notifyAc{viewModelChangeSignalChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewModelChangeSignal MEMBER _viewModelChangeSignal NOTIFY viewModelChangeSignalChanged)
	/*! @brief An alternative signal to be used instead of the viewProperty notify signal to
	 * detect property changes
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewChangeSignal}
	 *	@notifyAc{viewChangeSignalChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewChangeSignal MEMBER _viewChangeSignal NOTIFY viewChangeSignalChanged)
	/*! @brief The type/direction of binding to create
	 *
	 * @default{`MvvmBinding::TwoWay`}
	 * @accessors{
	 *	@memberAc{type}
	 *	@notifyAc{typeChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(BindingDirection type READ type WRITE setType NOTIFY typeChanged) //MEMBER is broken for flags

public:
	//! @copydoc QtMvvm::Binding::BindingDirectionFlag
	enum BindingDirectionFlag { //copy flags from binding
		SingleInit = Binding::SingleInit, //!< @copydoc QtMvvm::Binding::SingleInit
		OneWayToView = Binding::OneWayToView, //!< @copydoc QtMvvm::Binding::OneWayToView
		OneWayToViewModel = Binding::OneWayToViewModel, //!< @copydoc QtMvvm::Binding::OneWayToViewModel
		TwoWay = Binding::TwoWay //!< @copydoc QtMvvm::Binding::TwoWay
	};
	Q_DECLARE_FLAGS(BindingDirection, BindingDirectionFlag)
	Q_FLAG(BindingDirection)

	//! @private
	explicit QQmlMvvmBinding(QObject *parent = nullptr);

	//! @private
	void classBegin() override;
	//! @private
	void componentComplete() override;

	//! @private
	BindingDirection type() const;
	//! @copydoc QtMvvm::Binding::isValid
	Q_INVOKABLE bool isValid() const;

#ifdef DOXYGEN_RUN
public:
#else
public Q_SLOTS:
#endif
	//! @private
	void setType(BindingDirection type);
	//! @copydoc QtMvvm::Binding::unbind
	void unbind();

Q_SIGNALS:
	//! @notifyAcFn{MvvmBinding::viewModel}
	void viewModelChanged(QObject* viewModel);
	//! @notifyAcFn{MvvmBinding::viewModelProperty}
	void viewModelPropertyChanged(const QString &viewModelProperty);
	//! @notifyAcFn{MvvmBinding::view}
	void viewChanged(QObject* view);
	//! @notifyAcFn{MvvmBinding::viewProperty}
	void viewPropertyChanged(const QString &viewProperty);
	//! @notifyAcFn{MvvmBinding::viewModelChangeSignal}
	void typeChanged(BindingDirection type);
	//! @notifyAcFn{MvvmBinding::viewChangeSignal}
	void viewModelChangeSignalChanged(const QString &viewModelChangeSignal);
	//! @notifyAcFn{MvvmBinding::type}
	void viewChangeSignalChanged(const QString &viewChangeSignal);

private Q_SLOTS:
	void resetBinding();

	void viewWasSet();
	void viewModelWasSet();

private:
	Binding _binding;
	bool _completed = false;
	bool _viewSet = false;
	bool _viewModelSet = false;

	QObject* _viewModel = nullptr;
	QString _viewModelProperty;
	QObject* _view = nullptr;
	QString _viewProperty;
	BindingDirection _type = TwoWay;
	QString _viewModelChangeSignal;
	QString _viewChangeSignal;
};

}

#endif // QTMVVM_QQMLMVVMBINDING_H
