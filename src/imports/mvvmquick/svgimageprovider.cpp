#include "svgimageprovider.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QPainter>
#include <QtGui/QScreen>

#include <QtSvg/QSvgRenderer>

using namespace QtMvvm;

SvgImageProvider::SvgImageProvider() :
	QQuickImageProvider(Image)
{}

QImage SvgImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
	auto path = QStringLiteral(":/%1.svg").arg(id);
	QScopedPointer<QSvgRenderer, QScopedPointerDeleteLater> renderer(new QSvgRenderer(path));
	if(renderer->isValid()) {
		QSize tSize;
		if(requestedSize.isValid())
			tSize = requestedSize;
		else {
			tSize = renderer->defaultSize();
			tSize *= QGuiApplication::primaryScreen()->devicePixelRatio();
		}

		QImage image(tSize, QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::transparent);
		QPainter painter(&image);
		renderer->render(&painter);
		*size = image.size();

		return image;
	} else {
		*size = QSize();
		return QImage();
	}
}
