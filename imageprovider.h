#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

private:
    QImage m_image;
    QImage m_noImage;

public:
    explicit ImageProvider();

public:
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

public slots:
    void onUpdateImage(const QImage &image);

signals:
    void imageChanged(const QImage &image);
};

#endif // IMAGEPROVIDER_H
