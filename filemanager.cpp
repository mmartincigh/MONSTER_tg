#include <QDir>

#include "filemanager.h"
#include "utils.h"

const QString FileManager::m_MP4_NAME_FILTER("*.mp4");
const QString FileManager::m_AVI_NAME_FILTER("*.avi");
const QString FileManager::m_WMV_NAME_FILTER("*.wmv");
const QStringList FileManager::m_NAME_FILTERS(QStringList() << m_MP4_NAME_FILTER << m_AVI_NAME_FILTER << m_WMV_NAME_FILTER);

FileManager::FileManager(QObject *parent) :
    Base("FM", parent),
    m_videoFiles(),
    m_videoFilesCount(0),
    m_overwriteOutputFiles(false)
{
    this->debug("File manager created");
}

FileManager::~FileManager()
{
    this->debug("File manager disposed of");
}

void FileManager::initialize()
{
    this->debug("Initialized");
}

QStringList FileManager::videoFiles() const
{
    return m_videoFiles;
}

int FileManager::videoFilesCount() const
{
    return m_videoFilesCount;
}

bool FileManager::overwriteOutputFiles() const
{
    return m_overwriteOutputFiles;
}

void FileManager::setOverwriteOutputFiles(bool overwriteOutputFiles)
{
    if (m_overwriteOutputFiles == overwriteOutputFiles)
    {
        return;
    }

    m_overwriteOutputFiles = overwriteOutputFiles;

    this->debug("Overwrite output files changed: " + QString(m_overwriteOutputFiles ? "true" : "false"));

    emit this->overwriteOutputFilesChanged(m_overwriteOutputFiles);
}

void FileManager::setVideoFiles(const QStringList &videoFiles)
{
    m_videoFiles = videoFiles;

    this->debug("Video files changed");

    emit this->videoFilesChanged(m_videoFiles);
}

void FileManager::setVideoFilesCount(int videoFilesCount)
{
    if (m_videoFilesCount == videoFilesCount)
    {
        return;
    }

    m_videoFilesCount = videoFilesCount;

    this->debug("Video files count changed: " + QString::number(m_videoFilesCount));

    emit this->videoFilesCountChanged(m_videoFilesCount);
}

void FileManager::onSourcePathChanged(const QString &sourcePath)
{
    this->debug("Source path: " + sourcePath);
    if (sourcePath.isEmpty())
    {
        this->error("Source path is empty");

        return;
    }

    this->onReloadVideoFiles(sourcePath);
}

void FileManager::onVideoFiles(QStringList *videoFiles)
{
    if (videoFiles == NULL)
    {
        return;
    }

    *videoFiles = m_videoFiles;
}

void FileManager::onOverwriteOutputFiles(bool *overwriteOutputFiles)
{
    if (overwriteOutputFiles == NULL)
    {
        return;
    }

    *overwriteOutputFiles = m_overwriteOutputFiles;
}

void FileManager::onReloadVideoFiles()
{
    QString source_path;
    emit this->sourcePath(&source_path);
    this->debug("Source path: " + source_path);
    if (source_path.isEmpty())
    {
        this->error("Source path is empty");

        return;
    }

    this->onReloadVideoFiles(source_path);
}

void FileManager::onReloadVideoFiles(const QString &sourcePath)
{
    if (sourcePath.isEmpty())
    {
        this->error("Source path is empty");

        return;
    }

    // Enumerate the contents of the source path.
    this->debug("Source path: " + sourcePath);
    QDir source_directory(sourcePath);
    source_directory.setNameFilters(m_NAME_FILTERS);
    source_directory.setFilter(QDir::Files);
    source_directory.setSorting(QDir::Name);
    this->setVideoFiles(source_directory.entryList());
    this->setVideoFilesCount(m_videoFiles.size());

    if (m_videoFilesCount == 0)
    {
        this->warning("The source path contains no video files");

        return;
    }
}

void FileManager::onReloadVideoFiles(const QUrl &sourcePathUrl)
{
    if (sourcePathUrl.isEmpty())
    {
        this->error("Source path URL is empty");

        return;
    }

    // Enumerate the contents of the source path.
    QString source_path = Utils::urlToString(sourcePathUrl);
    this->debug("Source path: " + source_path);
    QDir source_directory(source_path);
    source_directory.setNameFilters(m_NAME_FILTERS);
    source_directory.setFilter(QDir::Files);
    source_directory.setSorting(QDir::Name);
    this->setVideoFiles(source_directory.entryList());
    this->setVideoFilesCount(m_videoFiles.size());

    if (m_videoFilesCount == 0)
    {
        this->warning("The source path contains no video files");

        return;
    }
}
