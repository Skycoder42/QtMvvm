#include "tabviewmodel.h"
#include <QtMvvmCore/Messages>

TabViewModel::TabViewModel(QObject *parent) :
	ViewModel(parent)
{}

TabViewModel::~TabViewModel()
{
	qInfo(Q_FUNC_INFO);
}

void TabViewModel::addTab()
{
	QtMvvm::getInput<QString>(tr("New Tab"), tr("Enter a tab title:"), this, [this](QString res, bool ok) {
		if(ok) {
			show<TabItemViewModel>({
									   {QStringLiteral("title"), res}
								   });
		}
	});
}



TabItemViewModel::TabItemViewModel(QObject *parent) :
	ViewModel(parent),
	_title(tr("No Title"))
{}

TabItemViewModel::~TabItemViewModel()
{
	qInfo(Q_FUNC_INFO);
}

QString TabItemViewModel::title() const
{
	return _title;
}

void TabItemViewModel::onInit(const QVariantHash &params)
{
	_title = params.value(QStringLiteral("title"), _title).toString();
	emit titleChanged(_title);
}
