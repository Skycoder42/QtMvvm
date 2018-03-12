#include <QObject>

/*! @brief A QML class to create a local mvvm multiway binding
 *
 * It is basically a wrapper around the QtMvvm::bind method. The parameters are set via the
 * properties, and once the binding component is completed, it will create the binding. It is
 * possible to modify the properties while running. This will recreate the binding, discarding
 * the previous one
 *
 * @sa QtMvvm::bind
 */
class MvvmBinding : public QObject
{
	/*! @brief The object in the role of a viewmodel
	 *
	 * @default{`null`}
	 * @accessors{
	 *	@memberAc{viewModel}
	 *	@notifyAc{viewModelChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QObject* viewModel MEMBER viewModel NOTIFY viewModelChanged)
	/*! @brief The property of the viewmodel to use in the binding
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewModelProperty}
	 *	@notifyAc{viewModelPropertyChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewModelProperty MEMBER viewModelProperty NOTIFY viewModelPropertyChanged)
	/*! @brief The object in the role of a view
	 *
	 * @default{`parent()`}
	 * @accessors{
	 *	@memberAc{view}
	 *	@notifyAc{viewChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QObject* view MEMBER view NOTIFY viewChanged)
	/*! @brief The property of the view to use in the binding
	 *
	 * @default{<i>Empty</i>}
	 * @accessors{
	 *	@memberAc{viewProperty}
	 *	@notifyAc{viewPropertyChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(QString viewProperty MEMBER viewProperty NOTIFY viewPropertyChanged)
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
	Q_PROPERTY(QString viewModelChangeSignal MEMBER viewModelChangeSignal NOTIFY viewModelChangeSignalChanged)
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
	Q_PROPERTY(QString viewChangeSignal MEMBER viewChangeSignal NOTIFY viewChangeSignalChanged)
	/*! @brief The type/direction of binding to create
	 *
	 * @default{`MvvmBinding::TwoWay`}
	 * @accessors{
	 *	@readAc{type}
	 *	@writeAc{setType()}
	 *	@notifyAc{typeChanged()}
	 * }
	 * @sa QtMvvm::bind
	 */
	Q_PROPERTY(BindingDirection type READ type WRITE setType NOTIFY typeChanged)

public:
	//! @copydoc QtMvvm::Binding::BindingDirectionFlag
	enum BindingDirection {
		SingleInit = 1, //!< @copydoc QtMvvm::Binding::SingleInit
		OneWayToView = 3, //!< @copydoc QtMvvm::Binding::OneWayToView
		OneWayToViewModel = 4, //!< @copydoc QtMvvm::Binding::OneWayToViewModel
		TwoWay = 7 //!< @copydoc QtMvvm::Binding::TwoWay
	};

	//! @copydoc QtMvvm::Binding::unbind
	void unbind();
	//! @copydoc QtMvvm::Binding::isValid
	bool isValid();
	//! @writeAcFn{MvvmBinding::type}
	void setType(BindingDirection type);

signals:
	//! @notifyAcFn{MvvmBinding::viewModel}
	void viewModelChanged();
	//! @notifyAcFn{MvvmBinding::viewModelProperty}
	void viewModelPropertyChanged();
	//! @notifyAcFn{MvvmBinding::view}
	void viewChanged();
	//! @notifyAcFn{MvvmBinding::viewProperty}
	void viewPropertyChanged();
	//! @notifyAcFn{MvvmBinding::viewModelChangeSignal}
	void viewModelChangeSignalChanged();
	//! @notifyAcFn{MvvmBinding::viewChangeSignal}
	void viewChangeSignalChanged();
	//! @notifyAcFn{MvvmBinding::type}
	void typeChanged();
};
