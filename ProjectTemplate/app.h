#ifndef %{AppGuard}
#define %{AppGuard}

#include <QtMvvmCore/CoreApp>

class %{AppCn} : public QtMvvm::CoreApp
{
	Q_OBJECT

public:
	explicit %{AppCn}(QObject *parent = nullptr);

protected:
	void performRegistrations() override;
	int startApp(const QStringList &arguments) override;
};

#undef coreApp
#define coreApp static_cast<%{AppCn}*>(CoreApp::instance())

#endif // %{AppGuard}
