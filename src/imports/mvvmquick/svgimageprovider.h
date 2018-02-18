#ifndef QTMVVM_SVGIMAGEPROVIDER_H
#define QTMVVM_SVGIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

namespace QtMvvm {

class SvgImageProvider : public QQuickImageProvider
{
public:
	explicit SvgImageProvider();

	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

}

#endif // QTMVVM_SVGIMAGEPROVIDER_H
