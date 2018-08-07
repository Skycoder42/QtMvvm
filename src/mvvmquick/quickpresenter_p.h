#ifndef QTMVVM_QUICKPRESENTER_P_H
#define QTMVVM_QUICKPRESENTER_P_H

#include <QtCore/QHash>
#include <QtCore/QPointer>

#include "qtmvvmquick_global.h"
#include "quickpresenter.h"

namespace QtMvvm {

class Q_MVVMQUICK_EXPORT QuickPresenterPrivate
{
	Q_DISABLE_COPY(QuickPresenterPrivate)
	friend class QtMvvm::QuickPresenter;

public:
	QuickPresenterPrivate() = default;

	static QuickPresenter *currentPresenter();
	static void setQmlPresenter(QObject *presenter);

private:
	QPointer<QObject> qmlPresenter;
	InputViewFactory *inputViewFactory = nullptr;

	QHash<const QMetaObject *, QUrl> explicitMappings;
	QStringList searchDirs{QStringLiteral(":/qtmvvm/views")};
};

}

#endif // QTMVVM_QUICKPRESENTER_P_H
