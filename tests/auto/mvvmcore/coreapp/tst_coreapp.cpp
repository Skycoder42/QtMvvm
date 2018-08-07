#include <QtTest>
#include <QtMvvmCore/ServiceRegistry>
#include "testapp.h"
#include "testviewmodel.h"
using namespace QtMvvm;

QTMVVM_REGISTER_CORE_APP(TestApp)

class CoreAppTest : public QObject
{
	Q_OBJECT

public:
	using MsgFn = std::function<void()>;

private Q_SLOTS:
	void initTestCase();

	void testAppBoot();
	void testTypeMapping_data();
	void testTypeMapping();

	void testPresentVm();
	void testPresentVmArgs();
	void testPresentVmChild();
	void testPresentVmForResult();
	void testPresentVmContainer();
	void testPresentVmSingleton();

	void testPresentDialog();
	void testPresentMessage_data();
	void testPresentMessage();

Q_SIGNALS:
	void msgUnblock(bool success);

private:
	bool wasClosed = false;
	QPointer<ProgressControl> lastProgress;

	Q_INVOKABLE void doCloseTest();
};

Q_DECLARE_METATYPE(CoreAppTest::MsgFn)

void CoreAppTest::initTestCase()
{
	qRegisterMetaType<TestViewModel*>();
	qRegisterMetaType<TestSingleViewModel*>();
	qRegisterMetaType<MsgFn>("MsgFn");
	QtMvvm::ServiceRegistry::instance()->registerInterface<QtMvvm::IPresenter, TestPresenter>();
}

void CoreAppTest::testAppBoot()
{
	QVERIFY(coreApp);
	QVERIFY(coreApp->wasRegistered);
	QVERIFY(coreApp->startArgs.isEmpty());

	QSignalSpy startSpy{coreApp, &TestApp::appStarted};
	QCoreApplication::processEvents();
	QVERIFY(!coreApp->startArgs.isEmpty());
	QCOMPARE(coreApp->startArgs, QCoreApplication::arguments());
	QCOMPARE(startSpy.size(), 1);
}

void CoreAppTest::testTypeMapping_data()
{
	QTest::addColumn<QByteArray>("type");
	QTest::addColumn<int>("targetType");
	QTest::addColumn<QVariant>("value");
	QTest::addColumn<bool>("needRegister");
	QTest::addColumn<QVariant>("result");

	QTest::newRow("builtin.switch") << QByteArrayLiteral("switch")
									<< static_cast<int>(QMetaType::Bool)
									<< QVariant{QStringLiteral("true")}
									<< false
									<< QVariant{true};

	QTest::newRow("builtin.string") << QByteArrayLiteral("string")
									<< static_cast<int>(QMetaType::QString)
									<< QVariant{QStringLiteral("test")}
									<< false
									<< QVariant{QStringLiteral("test")};

	QTest::newRow("builtin.number") << QByteArrayLiteral("number")
									<< static_cast<int>(QMetaType::Double)
									<< QVariant{QStringLiteral("4.2")}
									<< false
									<< QVariant{4.2};

	QTest::newRow("builtin.range") << QByteArrayLiteral("range")
								   << static_cast<int>(QMetaType::Int)
								   << QVariant{QStringLiteral("55")}
								   << false
								   << QVariant{55};

	QTest::newRow("builtin.date") << QByteArrayLiteral("date")
								  << static_cast<int>(QMetaType::QDateTime)
								  << QVariant{QStringLiteral("2018-10-10T10:10")}
								  << false
								  << QVariant{QDateTime{{2018, 10, 10}, {10, 10}}};

	QTest::newRow("builtin.color") << QByteArrayLiteral("color")
								   << static_cast<int>(QMetaType::QColor)
								   << QVariant{QStringLiteral("#123456")}
								   << false
								   << QVariant{QColor{0x12, 0x34, 0x56}};

	QTest::newRow("builtin.url") << QByteArrayLiteral("url")
								 << static_cast<int>(QMetaType::QUrl)
								 << QVariant{QStringLiteral("file:///root.txt")}
								 << false
								 << QVariant{QUrl::fromLocalFile(QStringLiteral("/root.txt"))};

	QTest::newRow("builtin.var") << QByteArrayLiteral("var")
								 << static_cast<int>(QMetaType::QVariant)
								 << QVariant{42}
								 << false
								 << QVariant{42};

	QTest::newRow("builtin.variant") << QByteArrayLiteral("variant")
									 << static_cast<int>(QMetaType::QVariant)
									 << QVariant{43}
									 << false
									 << QVariant{43};

	QTest::newRow("builtin.selection") << QByteArrayLiteral("selection")
									   << static_cast<int>(QMetaType::QVariant)
									   << QVariant{44}
									   << false
									   << QVariant{44};

	QTest::newRow("builtin.list") << QByteArrayLiteral("list")
								  << static_cast<int>(QMetaType::QVariant)
								  << QVariant{45}
								  << false
								  << QVariant{45};

	QTest::newRow("builtin.radiolist") << QByteArrayLiteral("radiolist")
									   << static_cast<int>(QMetaType::QVariant)
									   << QVariant{46}
									   << false
									   << QVariant{46};

	QTest::newRow("known") << QByteArrayLiteral("int")
						   << static_cast<int>(QMetaType::Int)
						   << QVariant{QStringLiteral("66")}
						   << false
						   << QVariant{66};

	QTest::newRow("custom") << QByteArrayLiteral("salt")
							<< static_cast<int>(QMetaType::Double)
							<< QVariant{QStringLiteral("0.5")}
							<< true
							<< QVariant{0.5};

	QTest::newRow("invalid.unknown") << QByteArrayLiteral("sugar")
									 << static_cast<int>(QMetaType::Int)
									 << QVariant{QStringLiteral("66")}
									 << false
									 << QVariant{};

	QTest::newRow("invalid.inconvertible") << QByteArrayLiteral("color")
										   << static_cast<int>(QMetaType::QColor)
										   << QVariant{true}
										   << false
										   << QVariant{};
}

void CoreAppTest::testTypeMapping()
{
	QFETCH(QByteArray, type);
	QFETCH(int, targetType);
	QFETCH(QVariant, value);
	QFETCH(bool, needRegister);
	QFETCH(QVariant, result);

	if(needRegister)
		CoreApp::registerInputTypeMapping(type, targetType);

	auto res = CoreApp::safeCastInputType(type, value);
	if(result.isValid()) {
		if(targetType == QMetaType::QVariant)
			QVERIFY(res.userType() != targetType);
		else
			QCOMPARE(res.userType(), targetType);
		QVERIFY(res.isValid());
		QCOMPARE(res, result);
	} else {
		QCOMPARE(res.userType(), value.userType());
		QVERIFY(res.userType() != targetType);
	}
}

void CoreAppTest::testPresentVm()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[0]).isEmpty());
	QVERIFY(!std::get<2>(presenter->presented[0]));
}

void CoreAppTest::testPresentVmArgs()
{
	QVariantHash params {
		{QStringLiteral("key"), QStringLiteral("value")},
		{QStringLiteral("baum"), 42}
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>(params);

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[0]), params);
	QVERIFY(!std::get<2>(presenter->presented[0]));
}

void CoreAppTest::testPresentVmChild()
{
	QVariantHash params {
		{QStringLiteral("key"), QStringLiteral("value")},
		{QStringLiteral("baum"), 42}
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = static_cast<TestViewModel*>(std::get<0>(presenter->presented[0]));
	QVERIFY(vm);

	vm->presenChild(params);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[1]), params);
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
}

void CoreAppTest::testPresentVmForResult()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = static_cast<TestViewModel*>(std::get<0>(presenter->presented[0]));
	QVERIFY(vm);

	// show with result
	vm->presentResult(42);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[1]).isEmpty());
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);

	QVariant result{5.5};
	emit child->resultReady(result);
	QCOMPARE(vm->results.size(), 1);
	QCOMPARE(std::get<0>(vm->results[0]), 42u);
	QCOMPARE(std::get<1>(vm->results[0]), result);
	delete child;
	QCOMPARE(vm->results.size(), 1);

	//show without result
	vm->presentResult(24);
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 3);
	QCOMPARE(presenter->presented.size(), 3);

	child = std::get<0>(presenter->presented[2]);
	QVERIFY(child);

	delete child;
	QCOMPARE(vm->results.size(), 2);
	QCOMPARE(std::get<0>(vm->results[1]), 24u);
	QVERIFY(!std::get<1>(vm->results[1]).isValid());
}

void CoreAppTest::testPresentVmContainer()
{
	QVariantHash showParams {
		{QStringLiteral("hello"), 13},
		{QStringLiteral("qtmvvm_container_params"), QVariantHash{
			{QStringLiteral("test"), 42},
		}}
	};
	QVariantHash initMap {
		{QStringLiteral("test"), 42},
		{QStringLiteral("qtmvvm_container_for"), QByteArray{TestContainedViewModel::staticMetaObject.className()}},
	};

	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestContainedViewModel>(showParams);

	while(presentSpy.size() < 2)
		QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[0]), initMap);
	QVERIFY(!std::get<2>(presenter->presented[0]));

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestContainedViewModel::staticMetaObject);
	QCOMPARE(std::get<1>(presenter->presented[1]), showParams);
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
}

void CoreAppTest::testPresentVmSingleton()
{
	auto presenter = TestApp::presenter();
	presenter->presented.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::presentDone};
	CoreApp::show<TestSingleViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);

	auto vm = std::get<0>(presenter->presented[0]);
	QVERIFY(vm);
	QCOMPARE(vm->metaObject(), &TestSingleViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[0]).isEmpty());
	QVERIFY(!std::get<2>(presenter->presented[0]));

	QSignalSpy instSpy{vm, &ViewModel::instanceInvoked};

	CoreApp::show<TestSingleViewModel>();
	QVERIFY(!presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->presented.size(), 1);
	QCOMPARE(instSpy.size(), 1);

	// test single container
	CoreApp::show<TestContainedSingleViewModel>();

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 2);
	QCOMPARE(presenter->presented.size(), 2);

	auto child = std::get<0>(presenter->presented[1]);
	QVERIFY(child);
	QCOMPARE(child->metaObject(), &TestContainedSingleViewModel::staticMetaObject);
	QVERIFY(std::get<1>(presenter->presented[1]).isEmpty());
	QCOMPARE(std::get<2>(presenter->presented[1]), vm);
	QCOMPARE(instSpy.size(), 2);
}

void CoreAppTest::testPresentDialog()
{
	auto presenter = TestApp::presenter();
	presenter->dialogs.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::dialogDone};

	MessageConfig config;
	config.setType("test1")
			.setSubType("44")
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.addButton(MessageConfig::Ignore)
			.setButtonText(MessageConfig::Help, QStringLiteral("hell"))
			.setDefaultValue(42)
			.setViewProperty(QStringLiteral("test"), QStringLiteral("tset"));
	QPointer<MessageResult> result = CoreApp::showDialog(config);

	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->dialogs.size(), 1);

	auto conf = std::get<0>(presenter->dialogs[0]);
	QCOMPARE(std::get<1>(presenter->dialogs[0]), result.data());

	// test message conf
	QCOMPARE(conf.type(), "test1");
	QCOMPARE(conf.subType(), "44");
	QCOMPARE(conf.title(), QStringLiteral("title"));
	QCOMPARE(conf.text(), QStringLiteral("text"));
	QCOMPARE(conf.buttons(), MessageConfig::Ok | MessageConfig::Ignore | MessageConfig::Help);
	QHash<MessageConfig::StandardButton, QString> btnTxt {
		{MessageConfig::Help, QStringLiteral("hell")}
	};
	QCOMPARE(conf.buttonTexts(), btnTxt);
	QCOMPARE(conf.defaultValue(), 42);
	QVariantMap viewProps {
		{QStringLiteral("test"), QStringLiteral("tset")}
	};
	QCOMPARE(conf.viewProperties(), viewProps);

	// test result
	result->setAutoDelete(false);
	result->setCloseTarget(this, QStringLiteral("doCloseTest()"));
	QSignalSpy resultSpy{result.data(), &MessageResult::dialogDone};
	QVERIFY(!result->autoDelete());
	QVERIFY(!result->hasResult());
	wasClosed = false;
	result->discardMessage();
	QTRY_VERIFY(wasClosed);

	result->complete(MessageConfig::Ignore, 5.6);
	QVERIFY(resultSpy.wait());
	QCOMPARE(resultSpy.size(), 1);
	QCOMPARE(resultSpy.takeFirst()[0].toInt(), MessageConfig::Ignore);
	QVERIFY(result->hasResult());
	QCOMPARE(result->result(), 5.6);
	result->deleteLater();
}

void CoreAppTest::testPresentMessage_data()
{
	QTest::addColumn<MsgFn>("messageFn");
	QTest::addColumn<QtMvvm::MessageConfig>("config");
	QTest::addColumn<int>("btn");
	QTest::addColumn<QVariant>("result");

	QTest::newRow("msgbox.information") << MsgFn{[this](){
		information(QStringLiteral("title"),
					QStringLiteral("text"),
					this, [this](){
			emit msgUnblock(true);
		}, QStringLiteral("okay"));
	}} << MessageConfig{MessageConfig::TypeMessageBox, MessageConfig::SubTypeInformation}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.setButtonText(MessageConfig::Ok, QStringLiteral("okay"))
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{};

	QTest::newRow("msgbox.question") << MsgFn{[this](){
		question(QStringLiteral("title"),
				 QStringLiteral("text"),
				 this, [this](bool ok){
			emit msgUnblock(ok);
		}, QStringLiteral("yep"), QStringLiteral("nep"));
	}} << MessageConfig{MessageConfig::TypeMessageBox, MessageConfig::SubTypeQuestion}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.setButtonText(MessageConfig::Yes, QStringLiteral("yep"))
			.setButtonText(MessageConfig::No, QStringLiteral("nep"))
	   << static_cast<int>(MessageConfig::Yes)
	   << QVariant{};

	QTest::newRow("msgbox.warning") << MsgFn{[this](){
		warning(QStringLiteral("title"),
				QStringLiteral("text"),
				this, [this](){
			emit msgUnblock(true);
		}, QStringLiteral("okay"));
	}} << MessageConfig{MessageConfig::TypeMessageBox, MessageConfig::SubTypeWarning}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.setButtonText(MessageConfig::Ok, QStringLiteral("okay"))
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{};

	QTest::newRow("msgbox.critical") << MsgFn{[this](){
		critical(QStringLiteral("title"),
				 QStringLiteral("text"),
				 this, [this](){
			emit msgUnblock(true);
		}, QStringLiteral("okay"));
	}} << MessageConfig{MessageConfig::TypeMessageBox, MessageConfig::SubTypeCritical}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.setButtonText(MessageConfig::Ok, QStringLiteral("okay"))
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{};

	QTest::newRow("msgbox.about") << MsgFn{[this](){
		auto res = about({});
		connect(res, &MessageResult::dialogDone,
				this, [this](MessageConfig::StandardButton btn){
			QCOMPARE(btn, MessageConfig::Ok);
			emit msgUnblock(true);
		});
	}} << MessageConfig{MessageConfig::TypeMessageBox, MessageConfig::SubTypeAbout}
			.setTitle(tr("%1 — Version %2")
					  .arg(QGuiApplication::applicationDisplayName(), QCoreApplication::applicationVersion()))
			.setViewProperty(QStringLiteral("addQtVersion"), true)
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{};

	QTest::newRow("input") << MsgFn{[this](){
		getInput<int>(QStringLiteral("title"),
					  QStringLiteral("text"),
					  this, [this](int value, bool ok){
			emit msgUnblock(ok && value == 42);
		}, 24, {
			{QStringLiteral("someProp"), true}
		}, QStringLiteral("okay"), QStringLiteral("break"));
	}} << MessageConfig{MessageConfig::TypeInputDialog, QMetaType::typeName(QMetaType::Int)}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("text"))
			.setDefaultValue(24)
			.setButtonText(MessageConfig::Ok, QStringLiteral("okay"))
			.setButtonText(MessageConfig::Cancel, QStringLiteral("break"))
			.setViewProperty(QStringLiteral("someProp"), true)
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{42};

	QTest::newRow("path.dir") << MsgFn{[this](){
		getExistingDirectory(this, [this](const QUrl &url){
			emit msgUnblock(url == QUrl::fromLocalFile(QStringLiteral("/dir/path.txt")));
		}, QStringLiteral("title"), QUrl::fromLocalFile(QStringLiteral("/dir")));
	}} << MessageConfig{MessageConfig::TypeFileDialog, MessageConfig::SubTypeDir}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QUrl::fromLocalFile(QStringLiteral("/dir")))
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{QUrl::fromLocalFile(QStringLiteral("/dir/path.txt"))};

	QTest::newRow("path.open.file") << MsgFn{[this](){
		getOpenFile(this, [this](const QUrl &url){
			emit msgUnblock(url == QUrl::fromLocalFile(QStringLiteral("/dir/path.txt")));
		}, QStringLiteral("title"), {
			QStringLiteral("text/plain")
		}, QUrl::fromLocalFile(QStringLiteral("/dir")));
	}} << MessageConfig{MessageConfig::TypeFileDialog, MessageConfig::SubTypeOpenFile}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QUrl::fromLocalFile(QStringLiteral("/dir")))
			.setViewProperty(QStringLiteral("mimeTypes"), QStringList{QStringLiteral("text/plain")})
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{QUrl::fromLocalFile(QStringLiteral("/dir/path.txt"))};

	QTest::newRow("path.open.files") << MsgFn{[this](){
		getOpenFiles(this, [this](const QList<QUrl> &urls){
			emit msgUnblock(urls == QList<QUrl>{QUrl::fromLocalFile(QStringLiteral("/dir/path.txt"))});
		}, QStringLiteral("title"), {
			QStringLiteral("text/plain")
		}, QUrl::fromLocalFile(QStringLiteral("/dir")));
	}} << MessageConfig{MessageConfig::TypeFileDialog, MessageConfig::SubTypeOpenFiles}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QUrl::fromLocalFile(QStringLiteral("/dir")))
			.setViewProperty(QStringLiteral("mimeTypes"), QStringList{QStringLiteral("text/plain")})
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant::fromValue(QList<QUrl>{QUrl::fromLocalFile(QStringLiteral("/dir/path.txt"))});

	QTest::newRow("path.save") << MsgFn{[this](){
		getSaveFile(this, [this](const QUrl &url){
			emit msgUnblock(url == QUrl::fromLocalFile(QStringLiteral("/dir/path.txt")));
		}, QStringLiteral("title"), {
			QStringLiteral("text/plain")
		}, QUrl::fromLocalFile(QStringLiteral("/dir")));
	}} << MessageConfig{MessageConfig::TypeFileDialog, MessageConfig::SubTypeSaveFile}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QUrl::fromLocalFile(QStringLiteral("/dir")))
			.setViewProperty(QStringLiteral("mimeTypes"), QStringList{QStringLiteral("text/plain")})
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{QUrl::fromLocalFile(QStringLiteral("/dir/path.txt"))};

	QTest::newRow("color") << MsgFn{[this](){
		getColor(this, [this](const QColor &color){
			emit msgUnblock(color == Qt::green);
		}, QStringLiteral("title"), Qt::red);
	}} << MessageConfig{MessageConfig::TypeColorDialog, MessageConfig::SubTypeRgb}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QColor{Qt::red})
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant::fromValue<QColor>(Qt::green);

	QTest::newRow("color") << MsgFn{[this](){
		getColor(this, [this](const QColor &color){
			emit msgUnblock(color == QColor{0xab, 0xcd, 0xef, 0x01});
		}, QStringLiteral("title"), QColor{0x12, 0x34, 0x56, 0x78}, true);
	}} << MessageConfig{MessageConfig::TypeColorDialog, MessageConfig::SubTypeArgb}
			.setTitle(QStringLiteral("title"))
			.setDefaultValue(QColor{0x12, 0x34, 0x56, 0x78})
	   << static_cast<int>(MessageConfig::Ok)
	   << QVariant{QColor{0xab, 0xcd, 0xef, 0x01}};

	QTest::newRow("progress.progress") << MsgFn{[this](){
		lastProgress = showProgress(this,
									QStringLiteral("title"),
									QStringLiteral("label"),
									42, 24, true, 33,
									QStringLiteral("break"));
		QVERIFY(lastProgress->autoDelete());
		QVERIFY(!lastProgress->isIndeterminate());
		QCOMPARE(lastProgress->minimum(), 24);
		QCOMPARE(lastProgress->maximum(), 42);
		QCOMPARE(lastProgress->progress(), 33);
		// triggered from gui in core
		QString called;
		connect(lastProgress, &ProgressControl::changeLabel,
				this, [&](QString label){
			called = std::move(label);
		}, Qt::DirectConnection);
		lastProgress->updateLabel(QStringLiteral("update"));
		QCOMPARE(called, QStringLiteral("update"));

		connect(lastProgress, &ProgressControl::closed,
				this, [this](){
			emit msgUnblock(true);
		}, Qt::QueuedConnection);
		// triggered from core in gui
		connect(lastProgress, &ProgressControl::closeRequested,
				this, [this](){
			lastProgress->notifyClosed();
		}, Qt::QueuedConnection);
		// triggered from gui in core
		connect(lastProgress, &ProgressControl::canceled,
				lastProgress, [this](MessageConfig::StandardButton btn) {
			QCOMPARE(btn, MessageConfig::Cancel);
			lastProgress->close();
		}, Qt::QueuedConnection);
	}} << MessageConfig{MessageConfig::TypeProgressDialog, MessageConfig::SubTypeProgress}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("label"))
			.setButtonText(MessageConfig::Cancel, QStringLiteral("break"))
	   << static_cast<int>(MessageConfig::Cancel)
	   << QVariant{};

	QTest::newRow("progress.indeterminate") << MsgFn{[this](){
		lastProgress = showIndeterminateProgress(this,
												 QStringLiteral("title"),
												 QStringLiteral("label"),
												 false);
		QVERIFY(lastProgress->autoDelete());
		QVERIFY(lastProgress->isIndeterminate());
		// triggered from gui in core
		connect(lastProgress, &ProgressControl::closed,
				this, [this](){
			emit msgUnblock(true);
		}, Qt::QueuedConnection);
	}} << MessageConfig{MessageConfig::TypeProgressDialog, MessageConfig::SubTypeProgress}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("label"))
			.setButtons(MessageConfig::NoButton)
	   << static_cast<int>(MessageConfig::NoButton)
	   << QVariant{};

	QTest::newRow("progress.busy") << MsgFn{[this](){
		lastProgress = showBusy(this,
								QStringLiteral("title"),
								QStringLiteral("label"),
								false,
								QStringLiteral("break"));
		QVERIFY(lastProgress->autoDelete());
		QVERIFY(lastProgress->isIndeterminate());
		// triggered from gui in core
		connect(lastProgress, &ProgressControl::closed,
				this, [this](){
			emit msgUnblock(true);
		}, Qt::QueuedConnection);
	}} << MessageConfig{MessageConfig::TypeProgressDialog, MessageConfig::SubTypeBusy}
			.setTitle(QStringLiteral("title"))
			.setText(QStringLiteral("label"))
			.setButtons(MessageConfig::NoButton)
	   << static_cast<int>(MessageConfig::NoButton)
	   << QVariant{};
}

void CoreAppTest::testPresentMessage()
{
	QFETCH(MsgFn, messageFn);
	QFETCH(QtMvvm::MessageConfig, config);
	QFETCH(int, btn);
	QFETCH(QVariant, result);

	QVERIFY(messageFn);
	auto presenter = TestApp::presenter();
	presenter->dialogs.clear();
	QSignalSpy presentSpy{presenter, &TestPresenter::dialogDone};

	messageFn();
	QVERIFY(presentSpy.wait());
	QCOMPARE(presentSpy.size(), 1);
	QCOMPARE(presenter->dialogs.size(), 1);

	auto conf = std::get<0>(presenter->dialogs[0]);
	auto res = std::get<1>(presenter->dialogs[0]);

	bool isProgress = false;
	if(config.type() == MessageConfig::TypeProgressDialog &&
	   !config.defaultValue().isValid()) {
		isProgress = true;
		config.setDefaultValue(QVariant::fromValue<ProgressControl*>(lastProgress));
	}

	// test message conf
	QCOMPARE(conf.type(), config.type());
	QCOMPARE(conf.subType(), config.subType());
	QCOMPARE(conf.title(), config.title());
	if(conf.subType() != MessageConfig::SubTypeAbout)
		QCOMPARE(conf.text(), config.text());
	QCOMPARE(conf.buttons(), config.buttons());
	QCOMPARE(conf.buttonTexts(), config.buttonTexts());
	QCOMPARE(conf.defaultValue(), config.defaultValue());
	QCOMPARE(conf.viewProperties(), config.viewProperties());

	//complete it
	QSignalSpy unblockSpy{this, &CoreAppTest::msgUnblock};
	if(isProgress){
		if(config.buttons().testFlag(MessageConfig::Cancel))
			lastProgress->requestCancel(MessageConfig::Cancel);
		else
			lastProgress->notifyClosed();
	} else
		res->complete(static_cast<MessageConfig::StandardButton>(btn), result);
	QVERIFY(unblockSpy.wait());
	QCOMPARE(unblockSpy.size(), 1);
	QVERIFY(unblockSpy.takeFirst()[0].toBool());
}

void CoreAppTest::doCloseTest()
{
	wasClosed = true;
}

QTEST_MAIN(CoreAppTest)

#include "tst_coreapp.moc"
