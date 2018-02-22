%modules = (
	"QtMvvmCore" => "$basedir/src/mvvmcore",
	"QtMvvmWidgets" => "$basedir/src/mvvmwidgets",
	"QtMvvmQuick" => "$basedir/src/mvvmquick",
	"QtMvvmSettingsCore" => "$basedir/src/mvvmsettingscore"
);

# Force generation of camel case headers for classes inside QtDataSync namespaces
$publicclassregexp = "QtMvvm::(?!__helpertypes).+";

%classnames = (
	"message.h" => "Messages"
);
