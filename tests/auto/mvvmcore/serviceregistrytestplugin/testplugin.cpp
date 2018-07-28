#include "testplugin.h"

TestPlugin::TestPlugin(QObject *parent) :
	QObject{parent}
{}

int TestPlugin::magicNumber() const
{
	return 72;
}
