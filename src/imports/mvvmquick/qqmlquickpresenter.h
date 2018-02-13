#ifndef QTMVVM_QQMLQUICKPRESENTER_H
#define QTMVVM_QQMLQUICKPRESENTER_H

#include <QObject>

namespace QtMvvm {

class QQmlQuickPresenter : public QObject
{
	Q_OBJECT

public:
	explicit QQmlQuickPresenter(QObject *parent = nullptr);
};

}

#endif // QTMVVM_QQMLQUICKPRESENTER_H
