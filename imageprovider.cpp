#include "imageprovider.h"

#include <QDebug>

ImageProvider::ImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Image),
    m_noImage(":/images/thumbnails/noThumbnail")
{
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "IP  > Request image:" << id;

    QImage result = m_image;

    if (result.isNull())
    {
        qDebug() << "IP  > Requested image is null, loading \"no image\" instead";

        result = m_noImage;
    }

    if (size)
    {
        *size = result.size();

        qDebug() << "IP  > Image size:" << *size;
    }

    if (requestedSize.width() > 0
            && requestedSize.height() > 0)
    {
        result = result.scaled(requestedSize.width(), requestedSize.height(), Qt::KeepAspectRatio);

        qDebug() << "IP  > Image scaled:" << result.size();
    }

    return result;
}

void ImageProvider::onUpdateImage(const QImage &image)
{
    if (m_image == image)
    {
        return;
    }

    m_image = image;

    emit this->imageChanged(m_image);
}
