%modules = (
	"QtMvvmCore" => "$basedir/src/mvvmcore",
	"QtMvvmWidgets" => "$basedir/src/mvvmwidgets",
	"QtMvvmQuick" => "$basedir/src/mvvmquick",
	"QtMvvmDataSyncCore" => "$basedir/src/mvvmdatasynccore",
	"QtMvvmDataSyncWidgets" => "$basedir/src/mvvmdatasyncwidgets",
	"QtMvvmDataSyncQuick" => "$basedir/src/mvvmdatasyncquick"
);

# Force generation of camel case headers for classes inside QtDataSync namespaces
$publicclassregexp = "QtMvvm::(?!__helpertypes|SettingsElements).+";

%classnames = (
	"message.h" => "Messages,MessageConfig,MessageResult,ProgressControl",
	"settingssetup.h" => "SettingsElements",
	"injection.h" => "Injection",
);
