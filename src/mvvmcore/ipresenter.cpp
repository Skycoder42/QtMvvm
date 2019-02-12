#include "ipresenter.h"
using namespace QtMvvm;

PresenterException::PresenterException(const QByteArray &what) :
	_what(what)
{}

PresenterException::PresenterException(const PresenterException * const other) :
	_what(other->_what)
{}

const char *PresenterException::what() const noexcept
{
	return _what.constData();
}

void PresenterException::raise() const
{
	throw (*this);
}

QTMVVM_EXCEPTION_BASE *PresenterException::clone() const
{
	return new PresenterException(this);
}
