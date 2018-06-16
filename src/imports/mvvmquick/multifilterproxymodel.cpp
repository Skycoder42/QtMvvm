#include "multifilterproxymodel.h"
using namespace QtMvvm;

MultiFilterProxyModel::MultiFilterProxyModel(QObject *parent) :
	QSortFilterProxyModel(parent),
	_filterRoles()
{}

void MultiFilterProxyModel::addFilterRole(int role)
{
	_filterRoles.insert(role);
	invalidateFilter();
}

void MultiFilterProxyModel::addFilterRoles(const QList<int> &roles)
{
	_filterRoles.unite(QSet<int>::fromList(roles));
	invalidateFilter();
}

void MultiFilterProxyModel::clearFilterRoles()
{
	_filterRoles.clear();
	invalidateFilter();
}

void MultiFilterProxyModel::setFilter(const QRegularExpression &regex)
{
	_filterRegex = regex;
	invalidateFilter();
}

bool MultiFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(!_filterRegex.isValid() || _filterRoles.isEmpty())
		return true;

	for(auto role : _filterRoles) {
		auto rData = sourceModel()->data(sourceModel()->index(source_row, 0, source_parent), role);
		//try as stringlist
		auto strList = rData.toStringList();
		for(const auto &str : strList) {
			if(_filterRegex.match(str).hasMatch())
				return true;
		}
		//try as string
		auto str = rData.toString();
		if(!str.isNull() && _filterRegex.match(str).hasMatch())
			return true;
	}

	return false;
}
