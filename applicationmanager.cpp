#include "applicationmanager.h"

ApplicationManager::ApplicationManager(QObject *parent) :
    Base("AM", parent),
    m_sourceManager(this),
    m_destinationManager(this),
    m_fileManager(this),
    m_thumbnailGenerator(this),
    m_settingsManager(this),
    m_windowManager(this)
{
    QObject::connect(&m_sourceManager, SIGNAL(sourcePathModel(QStringList*)), &m_settingsManager, SLOT(onSourcePathModel(QStringList*)));
    QObject::connect(&m_sourceManager, SIGNAL(sourcePathModelChanged(QStringList)), &m_settingsManager, SLOT(onSourcePathModelChanged(QStringList)));
    QObject::connect(&m_sourceManager, SIGNAL(sourcePathChanged(QString)), &m_fileManager, SLOT(onSourcePathChanged(QString)));
    QObject::connect(&m_sourceManager, SIGNAL(isSourcePathUrlValidChanged(bool)), &m_thumbnailGenerator, SLOT(onIsSourcePathUrlValidChanged(bool)));

    QObject::connect(&m_destinationManager, SIGNAL(destinationPathModel(QStringList*)), &m_settingsManager, SLOT(onDestinationPathModel(QStringList*)));
    QObject::connect(&m_destinationManager, SIGNAL(destinationPathModelChanged(QStringList)), &m_settingsManager, SLOT(onDestinationPathModelChanged(QStringList)));
    QObject::connect(&m_destinationManager, SIGNAL(isDestinationPathUrlValidChanged(bool)), &m_thumbnailGenerator, SLOT(onIsDestinationPathUrlValidChanged(bool)));

    QObject::connect(&m_fileManager, SIGNAL(sourcePath(QString*)), &m_sourceManager, SLOT(onSourcePath(QString*)));

    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailRows(int*)), &m_settingsManager, SLOT(onThumbnailRows(int*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailRowsChanged(int)), &m_settingsManager, SLOT(onThumbnailRowsChanged(int)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailColumns(int*)), &m_settingsManager, SLOT(onThumbnailColumns(int*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailColumnsChanged(int)), &m_settingsManager, SLOT(onThumbnailColumnsChanged(int)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailMaxWidth(int*)), &m_settingsManager, SLOT(onThumbnailMaxWidth(int*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailMaxWidthChanged(int)), &m_settingsManager, SLOT(onThumbnailMaxWidthChanged(int)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailMaxHeight(int*)), &m_settingsManager, SLOT(onThumbnailMaxHeight(int*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(thumbnailMaxHeightChanged(int)), &m_settingsManager, SLOT(onThumbnailMaxHeightChanged(int)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(isSourcePathUrlValid(bool*)), &m_sourceManager, SLOT(onIsSourcePathUrlValid(bool*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(isDestinationPathUrlValid(bool*)), &m_destinationManager, SLOT(onIsDestinationPathUrlValid(bool*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(sourcePath(QString*)), &m_sourceManager, SLOT(onSourcePath(QString*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(destinationPath(QString*)), &m_destinationManager, SLOT(onDestinationPath(QString*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(videoFiles(QStringList*)), &m_fileManager, SLOT(onVideoFiles(QStringList*)));
    QObject::connect(&m_thumbnailGenerator, SIGNAL(overwriteOutputFiles(bool*)), &m_fileManager, SLOT(onOverwriteOutputFiles(bool*)));

    this->debug("Application manager created");
}

ApplicationManager::~ApplicationManager()
{
    this->debug("Application manager disposed of");
}

void ApplicationManager::initialize()
{
    m_sourceManager.initialize();
    m_destinationManager.initialize();
    m_fileManager.initialize();
    m_thumbnailGenerator.initialize();

    this->debug("Initialized");
}

SourceManager *ApplicationManager::sourceManager()
{
    return &m_sourceManager;
}

DestinationManager *ApplicationManager::destinationManager()
{
    return &m_destinationManager;
}

FileManager *ApplicationManager::fileManager()
{
    return &m_fileManager;
}

ThumbnailGenerator *ApplicationManager::thumbnailGenerator()
{
    return &m_thumbnailGenerator;
}

WindowManager *ApplicationManager::windowManager()
{
    return &m_windowManager;
}

void ApplicationManager::onCompleted()
{
    this->debug("GUI Completed");
}
