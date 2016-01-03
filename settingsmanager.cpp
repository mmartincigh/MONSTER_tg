#include <QStandardPaths>
#include <QUrl>

#include "settingsmanager.h"

const QString SettingsManager::m_SOURCE_PATH_MODEL_KEY("sourceManager/sourcePathModel");
const QStringList SettingsManager::m_FALL_BACK_SOURCE_PATH_MODEL;
const QString SettingsManager::m_DESTINATION_PATH_MODEL_KEY("sourceManager/destinationPathModel");
const QStringList SettingsManager::m_FALL_BACK_DESTINATION_PATH_MODEL;
const QString SettingsManager::m_THUMBNAIL_ROWS_KEY("thumbnailGenerator/thumbnailRows");
const int SettingsManager::m_FALL_BACK_THUMBNAIL_ROWS(6);
const QString SettingsManager::m_THUMBNAIL_COLUMNS_KEY("thumbnailGenerator/thumbnailColumns");
const int SettingsManager::m_FALL_BACK_THUMBNAIL_COLUMNS(3);
const QString SettingsManager::m_THUMBNAIL_MAX_WIDTH_KEY("thumbnailGenerator/thumbnailMaxWidth");
const int SettingsManager::m_FALL_BACK_THUMBNAIL_MAX_WIDTH(600);
const QString SettingsManager::m_THUMBNAIL_MAX_HEIGHT_KEY("thumbnailGenerator/thumbnailMaxHeight");
const int SettingsManager::m_FALL_BACK_THUMBNAIL_MAX_HEIGHT(600);

SettingsManager::SettingsManager(QObject *parent) :
    Base("SM", parent),
    m_settings(this)
{
    this->debug("Settings manager created");
}

SettingsManager::~SettingsManager()
{
    this->debug("Settings manager disposed of");
}

QStringList SettingsManager::sourcePathModel() const
{
    return m_settings.value(m_SOURCE_PATH_MODEL_KEY, m_FALL_BACK_SOURCE_PATH_MODEL).toStringList();
}

QStringList SettingsManager::destinationPathModel() const
{
    return m_settings.value(m_DESTINATION_PATH_MODEL_KEY, m_FALL_BACK_DESTINATION_PATH_MODEL).toStringList();
}

int SettingsManager::thumbnailRows() const
{
    return m_settings.value(m_THUMBNAIL_ROWS_KEY, m_FALL_BACK_THUMBNAIL_ROWS).toInt();
}

int SettingsManager::thumbnailColumns() const
{
    return m_settings.value(m_THUMBNAIL_COLUMNS_KEY, m_FALL_BACK_THUMBNAIL_COLUMNS).toInt();
}

int SettingsManager::thumbnailMaxWidth() const
{
    return m_settings.value(m_THUMBNAIL_MAX_WIDTH_KEY, m_FALL_BACK_THUMBNAIL_MAX_WIDTH).toInt();
}

int SettingsManager::thumbnailMaxHeight() const
{
    return m_settings.value(m_THUMBNAIL_MAX_HEIGHT_KEY, m_FALL_BACK_THUMBNAIL_MAX_HEIGHT).toInt();
}

void SettingsManager::onSourcePathModel(QStringList *sourcePathModel)
{
    if (sourcePathModel == NULL)
    {
        return;
    }

    *sourcePathModel = this->sourcePathModel();

    this->debug("Source path model: " + (*sourcePathModel).join('|'));
}

void SettingsManager::onSourcePathModelChanged(const QStringList &sourcePathModel)
{
    QStringList default_source_path_model = this->sourcePathModel();

    if (default_source_path_model == sourcePathModel)
    {
        return;
    }

    m_settings.setValue(m_SOURCE_PATH_MODEL_KEY, sourcePathModel);

    this->debug("Source path model changed: " + sourcePathModel.join('|'));
}

void SettingsManager::onDestinationPathModel(QStringList *destinationPathModel)
{
    if (destinationPathModel == NULL)
    {
        return;
    }

    *destinationPathModel = this->destinationPathModel();

    this->debug("Destination path model: " + (*destinationPathModel).join('|'));
}

void SettingsManager::onDestinationPathModelChanged(const QStringList &destinationPathModel)
{
    QStringList default_destination_path_model = this->destinationPathModel();

    if (default_destination_path_model == destinationPathModel)
    {
        return;
    }

    m_settings.setValue(m_DESTINATION_PATH_MODEL_KEY, destinationPathModel);

    this->debug("Destination path model changed: " + destinationPathModel.join('|'));
}

void SettingsManager::onThumbnailRows(int *thumbnailRows)
{
    if (thumbnailRows == NULL)
    {
        return;
    }

    *thumbnailRows = this->thumbnailRows();

    this->debug("Thumbnail rows: " + QString::number(*thumbnailRows));
}

void SettingsManager::onThumbnailRowsChanged(int thumbnailRows)
{
    int default_thumbnail_rows = this->thumbnailRows();

    if (default_thumbnail_rows == thumbnailRows)
    {
        return;
    }

    m_settings.setValue(m_THUMBNAIL_ROWS_KEY, thumbnailRows);

    this->debug("Thumbnail rows changed: " + QString::number(thumbnailRows));
}

void SettingsManager::onThumbnailColumns(int *thumbnailColumns)
{
    if (thumbnailColumns == NULL)
    {
        return;
    }

    *thumbnailColumns = this->thumbnailColumns();

    this->debug("Thumbnail columns: " + QString::number(*thumbnailColumns));
}

void SettingsManager::onThumbnailColumnsChanged(int thumbnailColumns)
{
    int default_thumbnail_columns = this->thumbnailColumns();

    if (default_thumbnail_columns == thumbnailColumns)
    {
        return;
    }

    m_settings.setValue(m_THUMBNAIL_COLUMNS_KEY, thumbnailColumns);

    this->debug("Thumbnail columns changed: " + QString::number(thumbnailColumns));
}

void SettingsManager::onThumbnailMaxWidth(int *thumbnailMaxWidth)
{
    if (thumbnailMaxWidth == NULL)
    {
        return;
    }

    *thumbnailMaxWidth = this->thumbnailMaxWidth();

    this->debug("Thumbnail max width: " + QString::number(*thumbnailMaxWidth));
}

void SettingsManager::onThumbnailMaxWidthChanged(int thumbnailMaxWidth)
{
    int default_thumbnail_max_width = this->thumbnailMaxWidth();

    if (default_thumbnail_max_width == thumbnailMaxWidth)
    {
        return;
    }

    m_settings.setValue(m_THUMBNAIL_MAX_WIDTH_KEY, thumbnailMaxWidth);

    this->debug("Thumbnail max width changed: " + QString::number(thumbnailMaxWidth));
}

void SettingsManager::onThumbnailMaxHeight(int *thumbnailMaxHeight)
{
    if (thumbnailMaxHeight == NULL)
    {
        return;
    }

    *thumbnailMaxHeight = this->thumbnailMaxHeight();

    this->debug("Thumbnail max height: " + QString::number(*thumbnailMaxHeight));
}

void SettingsManager::onThumbnailMaxHeightChanged(int thumbnailMaxHeight)
{
    int default_thumbnail_max_height = this->thumbnailMaxWidth();

    if (default_thumbnail_max_height == thumbnailMaxHeight)
    {
        return;
    }

    m_settings.setValue(m_THUMBNAIL_MAX_HEIGHT_KEY, thumbnailMaxHeight);

    this->debug("Thumbnail max height changed: " + QString::number(thumbnailMaxHeight));
}
