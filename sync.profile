%modules = (
	"QtMvvmCore" => "$basedir/src/mvvmcore",
	"QtMvvmWidgets" => "$basedir/src/mvvmwidgets",
	"QtMvvmQuick" => "$basedir/src/mvvmquick",
	"QtMvvmDataSyncCore" => "$basedir/src/mvvmdatasynccore",
	"QtMvvmDataSyncWidgets" => "$basedir/src/mvvmdatasyncwidgets"
);

# Force generation of camel case headers for classes inside QtDataSync namespaces
$publicclassregexp = "QtMvvm::(?!__helpertypes|SettingsElements).+";

%classnames = (
	"message.h" => "Messages",
	"settingssetup.h" => "SettingsElements"
);
