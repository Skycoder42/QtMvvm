#ifndef QTMVVM_INPUTVIEWFACTORY_P_H
#define QTMVVM_INPUTVIEWFACTORY_P_H

#include "qtmvvmquick_global.h"
#include "inputviewfactory.h"

namespace QtMvvm {

class InputViewFactoryPrivate
{
	Q_DISABLE_COPY(InputViewFactoryPrivate)

public:
	InputViewFactoryPrivate();

	QHash<QByteArray, QUrl> simpleInputs;
	QHash<QByteArray, QUrl> simpleDelegates;
	QHash<QByteArray, QSharedPointer<Formatter>> formatters;
	QHash<QByteArray, QByteArray> inputAliases;
	QHash<QByteArray, QByteArray> delegateAliases;
	QHash<QByteArray, QByteArray> formatterAliases;
};

}

#endif // QTMVVM_INPUTVIEWFACTORY_P_H
