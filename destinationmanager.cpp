#include <QFileInfo>

#include "destinationmanager.h"
#include "utils.h"

const QString DestinationManager::m_URL_FILE_SCHEME("file:///");

DestinationManager::DestinationManager(QObject *parent) :
    Base("DSTM", parent),
    m_destinationPath(""),
    m_destinationPathUrl(""),
    m_isDestinationPathUrlValid(false)
{
    QObject::connect(this, SIGNAL(destinationPathChanged(QString)), this, SLOT(onDestinationPathChanged(QString)));
    QObject::connect(this, SIGNAL(destinationPathUrlChanged(QUrl)), this, SLOT(onDestinationPathUrlChanged(QUrl)));

    this->debug("Destination manager created");
}

DestinationManager::~DestinationManager()
{
    this->debug("Destination manager disposed of");
}

void DestinationManager::initialize()
{
    QUrl destination_path_url;
    emit this->destinationPathUrl(&destination_path_url);
    this->setDestinationPathUrl(destination_path_url);

    this->debug("Initialized");
}

QString DestinationManager::destinationPath() const
{
    return m_destinationPath;
}

QUrl DestinationManager::destinationPathUrl() const
{
    return m_destinationPathUrl;
}

bool DestinationManager::isDestinationPathUrlValid() const
{
    return m_isDestinationPathUrlValid;
}

void DestinationManager::setDestinationPath(const QString &destinationPath)
{
    if (m_destinationPath == destinationPath)
    {
        return;
    }

    m_destinationPath = destinationPath;

    this->debug("Destination path changed: " + m_destinationPath);

    emit this->destinationPathChanged(m_destinationPath);
}

void DestinationManager::setDestinationPathUrl(const QUrl &destinationPathUrl)
{
    if (m_destinationPathUrl == destinationPathUrl)
    {
        return;
    }

    m_destinationPathUrl = destinationPathUrl;

    this->debug("Destination path URL changed: " + m_destinationPathUrl.toString());

    emit this->destinationPathUrlChanged(m_destinationPathUrl);
}

void DestinationManager::setIsDestinationPathUrlValid(bool isDestinationPathUrlValid)
{
    if (m_isDestinationPathUrlValid == isDestinationPathUrlValid)
    {
        return;
    }

    m_isDestinationPathUrlValid = isDestinationPathUrlValid;
    emit this->isDestinationPathUrlValidChanged(m_isDestinationPathUrlValid);

    this->debug("Is destination path URL valid changed: " + QString(m_isDestinationPathUrlValid ? "true" : "false"));
}

void DestinationManager::onUpdateDestinationPath(const QString &destinationPath)
{
    this->setDestinationPath(destinationPath);
}

void DestinationManager::onUpdateDestinationPathUrl(const QUrl &destinationPathUrl)
{
    this->setDestinationPathUrl(destinationPathUrl);
}

void DestinationManager::onIsDestinationPathUrlValid(bool *isDestinationPathUrlValid)
{
    if (isDestinationPathUrlValid == NULL)
    {
        return;
    }

    *isDestinationPathUrlValid = m_isDestinationPathUrlValid;
}

void DestinationManager::onDestinationPathUrl(QUrl *destinationPathUrl)
{
    if (destinationPathUrl == NULL)
    {
        return;
    }

    *destinationPathUrl = m_destinationPathUrl;
}

void DestinationManager::onDestinationPathChanged(const QString &destinationPath)
{
    //this->debug(Q_FUNC_INFO);

    QFileInfo destination_path_info(destinationPath);
    bool is_destination_path_valid = destination_path_info.exists() && destination_path_info.isDir();
    this->setIsDestinationPathUrlValid(is_destination_path_valid);

    if (!m_isDestinationPathUrlValid)
    {
        // Do not update the destination path URL.

        return;
    }

    QString destination_path(destinationPath);
    QUrl destination_path_url(destination_path.prepend(m_URL_FILE_SCHEME));
    this->setDestinationPathUrl(destination_path_url);
}

void DestinationManager::onDestinationPathUrlChanged(const QUrl &destinationPathUrl)
{
    //this->debug(Q_FUNC_INFO);

    QString destination_path = Utils::urlToString(destinationPathUrl);
    this->setDestinationPath(destination_path);
}
