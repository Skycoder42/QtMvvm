%modules = (
	"QtMvvmCore" => "$basedir/src/mvvmcore",
	"QtMvvmWidgets" => "$basedir/src/mvvmwidgets",
	"QtMvvmQuick" => "$basedir/src/mvvmquick",
	"QtMvvmSettingsCore" => "$basedir/src/mvvmsettingscore",
	"QtMvvmSettingsWidgets" => "$basedir/src/mvvmsettingswidgets"
);

# Force generation of camel case headers for classes inside QtDataSync namespaces
$publicclassregexp = "QtMvvm::(?!__helpertypes|SettingsElements).+";

%classnames = (
	"message.h" => "Messages",
	"settingssetup.h" => "SettingsElements"
);
