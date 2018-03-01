#ifndef SAMPLEDATA_H
#define SAMPLEDATA_H

#include <QtCore/QObject>

class SampleData
{
	Q_GADGET

	Q_PROPERTY(QString key MEMBER key USER true)

public:
	QString key;
};

Q_DECLARE_METATYPE(SampleData)

#endif // SAMPLEDATA_H
