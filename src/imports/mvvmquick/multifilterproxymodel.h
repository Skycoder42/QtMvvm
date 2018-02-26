#ifndef QTMVVM_MULTIFILTERPROXYMODEL_H
#define QTMVVM_MULTIFILTERPROXYMODEL_H

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>

namespace QtMvvm {

class MultiFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit MultiFilterProxyModel(QObject *parent = nullptr);

	void addFilterRole(int role);
	void addFilterRoles(const QList<int> &roles);
	void clearFilterRoles();

	void setFilter(const QRegularExpression &regex);

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
	QSet<int> _filterRoles;
	QRegularExpression _filterRegex;
};

}

#endif // QTMVVM_MULTIFILTERPROXYMODEL_H
