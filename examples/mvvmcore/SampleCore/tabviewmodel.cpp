#include "tabviewmodel.h"

TabViewModel::TabViewModel(QObject *parent) :
	ViewModel(parent)
{}

TabViewModel::~TabViewModel()
{
	qInfo(Q_FUNC_INFO);
}
