#include "qtmvvmdatasyncquick_global.h"

static void initResources()
{
#ifdef QT_STATIC
	Q_INIT_RESOURCE(qtmvvmdatasyncquick_module);
#endif
}

void QtMvvm::registerDataSyncQuick()
{
	initResources();
}
