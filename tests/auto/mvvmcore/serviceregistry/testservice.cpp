#include "testservice.h"

TestService::TestService(QObject *parent) :
	QObject{parent}
{}

WeakTestService::WeakTestService(QObject *parent) :
	TestService{parent}
{}
