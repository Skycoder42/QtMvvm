#ifndef %{AppGuard}
#define %{AppGuard}

#include <coreapp.h>

#include "%{ControlHdrName}"

class %{AppCn} : public CoreApp
{
	Q_OBJECT

public:
	explicit %{AppCn}(QObject *parent = nullptr);

protected:
	void setupParser(QCommandLineParser &parser, bool &allowInvalid) const override;
	bool startApp(const QCommandLineParser &parser) override;

protected slots:
	void aboutToQuit() override;

private:
	%{ControlCn} *mainControl;
};

#undef coreApp
#define coreApp static_cast<%{AppCn}*>(CoreApp::instance())

#endif // %{AppGuard}
