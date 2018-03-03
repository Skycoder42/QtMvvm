#ifndef TESTDUMMY_H
#define TESTDUMMY_H

#include <QObject>

class TestDummy : public QObject //TODO remove again
{
	Q_OBJECT
public:
	explicit TestDummy(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TESTDUMMY_H
