#ifndef %{WindowGuard}
#define %{WindowGuard}

#include <QMainWindow>
#include <%{ControlHdrName}>

namespace Ui {
class %{WindowCn};
}

class %{WindowCn} : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE %{WindowCn}(Control *mControl, QWidget *parent = nullptr);
	~%{WindowCn}();

private:
	%{ControlCn} *control;
	Ui::%{WindowCn} *ui;
};

#endif // %{WindowGuard}
