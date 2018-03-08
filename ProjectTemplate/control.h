#ifndef %{ControlGuard}
#define %{ControlGuard}

#include <control.h>

class %{ControlCn} : public Control
{
	Q_OBJECT
	
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
	explicit %{ControlCn}(QObject *parent = nullptr);
	
	void onShow() override;
	void onClose() override;
	
	QString text() const;

public slots:
@if '%{UseSettings}'
	void showSettings();

@endif
	void setText(QString text);
	
signals:
	void textChanged(QString text);
	
private:
	QString _text;
};

#endif // %{ControlGuard}
