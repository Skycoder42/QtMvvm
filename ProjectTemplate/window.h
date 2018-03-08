#ifndef %{WindowGuard}
#define %{WindowGuard}

#include <QtWidgets/QMainWindow>

#include <%{VmHdrName}>

namespace Ui {
class %{WindowCn};
}

class %{WindowCn} : public QMainWindow
{
	Q_OBJECT

public:
	Q_INVOKABLE %{WindowCn}(QtMvvm::ViewModel *viewModel, QWidget *parent = nullptr);
	~%{WindowCn}();

private:
	%{VmCn} *_viewModel;
	Ui::%{WindowCn} *_ui;
};

#endif // %{WindowGuard}
