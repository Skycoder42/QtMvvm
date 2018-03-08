#ifndef %{VmGuard}
#define %{VmGuard}

#include <QtMvvmCore/ViewModel>

class %{VmCn} : public QtMvvm::ViewModel
{
	Q_OBJECT
	
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
	Q_INVOKABLE explicit %{VmCn}(QObject *parent = nullptr);
	
	QString text() const;

public Q_SLOTS:
@if '%{UseSettings}'
	void showSettings();

@endif
	void setText(const QString &text);
	
Q_SIGNALS:
	void textChanged(const QString &text);
	
private:
	QString _text;
};

#endif // %{VmGuard}
