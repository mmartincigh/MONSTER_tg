#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>

#include "base.h"

class SettingsManager : public Base
{
    Q_OBJECT

private:
    static const QString m_SOURCE_PATH_MODEL_KEY;
    static const QStringList m_FALL_BACK_SOURCE_PATH_MODEL;
    static const QString m_DESTINATION_PATH_MODEL_KEY;
    static const QStringList m_FALL_BACK_DESTINATION_PATH_MODEL;
    static const QString m_THUMBNAIL_ROWS_KEY;
    static const int m_FALL_BACK_THUMBNAIL_ROWS;
    static const QString m_THUMBNAIL_COLUMNS_KEY;
    static const int m_FALL_BACK_THUMBNAIL_COLUMNS;
    static const QString m_THUMBNAIL_MAX_WIDTH_KEY;
    static const int m_FALL_BACK_THUMBNAIL_MAX_WIDTH;
    static const QString m_THUMBNAIL_MAX_HEIGHT_KEY;
    static const int m_FALL_BACK_THUMBNAIL_MAX_HEIGHT;
    QSettings m_settings;

public:
    explicit SettingsManager(QObject *parent = NULL);
    ~SettingsManager();

private:
    QStringList sourcePathModel() const;
    QStringList destinationPathModel() const;
    int thumbnailRows() const;
    int thumbnailColumns() const;
    int thumbnailMaxWidth() const;
    int thumbnailMaxHeight() const;

public slots:
    void onSourcePathModel(QStringList *sourcePathModel);
    void onSourcePathModelChanged(const QStringList &sourcePathModel);
    void onDestinationPathModel(QStringList *destinationPathModel);
    void onDestinationPathModelChanged(const QStringList &destinationPathModel);
    void onThumbnailRows(int *thumbnailRows);
    void onThumbnailRowsChanged(int thumbnailRows);
    void onThumbnailColumns(int *thumbnailColumns);
    void onThumbnailColumnsChanged(int thumbnailColumns);
    void onThumbnailMaxWidth(int *thumbnailMaxWidth);
    void onThumbnailMaxWidthChanged(int thumbnailMaxWidth);
    void onThumbnailMaxHeight(int *thumbnailMaxHeight);
    void onThumbnailMaxHeightChanged(int thumbnailMaxHeight);
};

#endif // SETTINGSMANAGER_H
