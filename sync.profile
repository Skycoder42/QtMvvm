%modules = (
	"QtMvvmCore" => "$basedir/src/mvvmcore",
	"QtMvvmWidgets" => "$basedir/src/mvvmwidgets",
	"QtMvvmQuick" => "$basedir/src/mvvmquick"
);

# Force generation of camel case headers for classes inside QtDataSync namespaces
$publicclassregexp = "QtMvvm::(?!__helpertypes).+";
