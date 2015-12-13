#include <QFileInfo>

#include "sourcemanager.h"
#include "utils.h"

const QString SourceManager::m_URL_FILE_SCHEME("file:///");

SourceManager::SourceManager(QObject *parent) :
    Base("SRCM", parent),
    m_sourcePath(""),
    m_sourcePathUrl(""),
    m_isSourcePathUrlValid(false)
{
    QObject::connect(this, SIGNAL(sourcePathChanged(QString)), this, SLOT(onSourcePathChanged(QString)));
    QObject::connect(this, SIGNAL(sourcePathUrlChanged(QUrl)), this, SLOT(onSourcePathUrlChanged(QUrl)));

    this->debug("Source manager created");
}

SourceManager::~SourceManager()
{
    this->debug("Source manager disposed of");
}

void SourceManager::initialize()
{
    QUrl source_path_url;
    emit this->sourcePathUrl(&source_path_url);
    this->setSourcePathUrl(source_path_url);

    this->debug("Initialized");
}

QString SourceManager::sourcePath() const
{
    return m_sourcePath;
}

QUrl SourceManager::sourcePathUrl() const
{
    return m_sourcePathUrl;
}

bool SourceManager::isSourcePathUrlValid() const
{
    return m_isSourcePathUrlValid;
}

void SourceManager::setSourcePath(const QString &sourcePath)
{
    if (m_sourcePath == sourcePath)
    {
        return;
    }

    m_sourcePath = sourcePath;

    this->debug("Source path changed: " + m_sourcePath);

    emit this->sourcePathChanged(m_sourcePath);
}

void SourceManager::setSourcePathUrl(const QUrl &sourcePathUrl)
{
    if (m_sourcePathUrl == sourcePathUrl)
    {
        return;
    }

    m_sourcePathUrl = sourcePathUrl;

    this->debug("Source path URL changed: " + m_sourcePathUrl.toString());

    emit this->sourcePathUrlChanged(m_sourcePathUrl);
}

void SourceManager::setIsSourcePathUrlValid(bool isSourcePathUrlValid)
{
    if (m_isSourcePathUrlValid == isSourcePathUrlValid)
    {
        return;
    }

    m_isSourcePathUrlValid = isSourcePathUrlValid;
    emit this->isSourcePathUrlValidChanged(m_isSourcePathUrlValid);

    this->debug("Is source path URL valid changed: " + QString(m_isSourcePathUrlValid ? "true" : "false"));
}

void SourceManager::onUpdateSourcePath(const QString &sourcePath)
{
    this->setSourcePath(sourcePath);
}

void SourceManager::onUpdateSourcePathUrl(const QUrl &sourcePathUrl)
{
    this->setSourcePathUrl(sourcePathUrl);
}

void SourceManager::onIsSourcePathUrlValid(bool *isSourcePathUrlValid)
{
    if (isSourcePathUrlValid == NULL)
    {
        return;
    }

    *isSourcePathUrlValid = m_isSourcePathUrlValid;
}

void SourceManager::onSourcePathUrl(QUrl *sourcePathUrl)
{
    if (sourcePathUrl == NULL)
    {
        return;
    }

    *sourcePathUrl = m_sourcePath;
}

void SourceManager::onSourcePathChanged(const QString &sourcePath)
{
    QFileInfo source_path_info(sourcePath);
    bool is_source_path_valid = source_path_info.exists() && source_path_info.isDir();
    this->setIsSourcePathUrlValid(is_source_path_valid);

    if (!m_isSourcePathUrlValid)
    {
        // Do not update the source path URL.

        return;
    }

    QString source_path(sourcePath);
    QUrl source_path_url(source_path.prepend(m_URL_FILE_SCHEME));
    this->setSourcePathUrl(source_path_url);
}

void SourceManager::onSourcePathUrlChanged(const QUrl &sourcePathUrl)
{
    QString source_path = Utils::urlToString(sourcePathUrl);
    this->setSourcePath(source_path);
}
