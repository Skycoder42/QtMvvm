#ifndef QTMVVM_QUICKPRESENTER_P_H
#define QTMVVM_QUICKPRESENTER_P_H

#include <QtCore/QHash>
#include <QtCore/QPointer>

#include "qtmvvmquick_global.h"
#include "quickpresenter.h"

namespace QtMvvm {

class Q_MVVMQUICK_EXPORT QuickPresenterPrivate
{
	friend class QtMvvm::QuickPresenter;

public:
	QuickPresenterPrivate();

	static QuickPresenter *currentPresenter();
	static void setQmlPresenter(QObject *presenter);

private:
	QHash<const QMetaObject *, QUrl> explicitMappings;
	QStringList searchDirs;

	QPointer<QObject> qmlPresenter;
	InputViewFactory *inputViewFactory;
};

}

#endif // QTMVVM_QUICKPRESENTER_P_H
