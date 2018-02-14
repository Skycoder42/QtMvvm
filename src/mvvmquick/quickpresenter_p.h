#ifndef QTMVVM_QUICKPRESENTER_P_H
#define QTMVVM_QUICKPRESENTER_P_H

#include <QtCore/QHash>

#include "qtmvvmquick_global.h"
#include "quickpresenter.h"

namespace QtMvvm {

class QuickPresenterPrivate
{
public:
	QuickPresenterPrivate();

	QHash<const QMetaObject *, QUrl> explicitMappings;
	QStringList searchDirs;

	static QuickPresenter *currentPresenter();
};

}

#endif // QTMVVM_QUICKPRESENTER_P_H
