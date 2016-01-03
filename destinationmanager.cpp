#include <QFileInfo>

#include "destinationmanager.h"
#include "utils.h"

const int DestinationManager::m_MAX_MODEL_SIZE(5);
const QString DestinationManager::m_URL_FILE_SCHEME("file:///");

DestinationManager::DestinationManager(QObject *parent) :
    Base("DSTM", parent),
    m_editText(""),
    m_destinationPathModel(),
    m_destinationPath(""),
    m_destinationPathUrl(""),
    m_isDestinationPathUrlValid(false)
{
    QObject::connect(this, SIGNAL(editTextChanged(QString)), this, SLOT(onEditTextChanged(QString)));

    this->debug("Destination manager created");
}

DestinationManager::~DestinationManager()
{
    this->debug("Destination manager disposed of");
}

void DestinationManager::initialize()
{
    QStringList destination_path_model;
    emit this->destinationPathModel(&destination_path_model);
    this->setDestinationPathModel(destination_path_model);

    this->debug("Initialized");
}

QStringList DestinationManager::destinationPathModel() const
{
    return m_destinationPathModel;
}

QUrl DestinationManager::destinationPathUrl() const
{
    return m_destinationPathUrl;
}

bool DestinationManager::isDestinationPathUrlValid() const
{
    return m_isDestinationPathUrlValid;
}

void DestinationManager::setEditText(const QString &editText)
{
    if (m_editText == editText)
    {
        return;
    }

    m_editText = editText;

    this->debug("Edit text changed: " + m_editText);

    emit this->editTextChanged(m_editText);
}

void DestinationManager::setDestinationPathModel(const QStringList &destinationPathModel)
{
    if (m_destinationPathModel == destinationPathModel)
    {
        return;
    }

    m_destinationPathModel = destinationPathModel;

    this->debug("Destination path model changed: " + m_destinationPathModel.join('|'));

    emit this->destinationPathModelChanged(m_destinationPathModel);
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

void DestinationManager::onDestinationPath(QString *destinationPath)
{
    if (destinationPath == NULL)
    {
        return;
    }

    *destinationPath = m_destinationPath;
}

void DestinationManager::onIsDestinationPathUrlValid(bool *isDestinationPathUrlValid)
{
    if (isDestinationPathUrlValid == NULL)
    {
        return;
    }

    *isDestinationPathUrlValid = m_isDestinationPathUrlValid;
}

void DestinationManager::onUpdateEditText(const QString &editText)
{
    this->setEditText(editText);
}

void DestinationManager::onUpdateEditText(const QUrl &editText)
{
    QString edit_text = Utils::urlToString(editText);
    this->setEditText(edit_text);
}

void DestinationManager::onGenerateThumbnails()
{
    // Do not change the model if the current path is invalid.
    if (!m_isDestinationPathUrlValid)
    {
        return;
    }

    // Get the destination path model.
    QStringList destination_path_model = m_destinationPathModel;

    // Check whether the model already contains the current path.
    if (destination_path_model.contains(m_editText))
    {
        // Move the current path to the top.
        destination_path_model.removeAll(m_editText);
        destination_path_model.prepend(m_editText);
        this->setDestinationPathModel(destination_path_model);

        return;
    }

    // Make room for another entry if necessary.
    while (destination_path_model.size() >= m_MAX_MODEL_SIZE)
    {
        // Remove the oldest (last) entry from the model.
        destination_path_model.removeLast();
    }

    // Prepend the current path to the model.
    destination_path_model.prepend(m_editText);
    this->setDestinationPathModel(destination_path_model);
}

void DestinationManager::onEditTextChanged(const QString &editText)
{
    QFileInfo destination_path_info(editText);
    bool is_destination_path_valid = destination_path_info.exists() && destination_path_info.isDir();
    this->setIsDestinationPathUrlValid(is_destination_path_valid);

    if (!m_isDestinationPathUrlValid)
    {
        // Do not update the destination path URL.

        return;
    }

    this->setDestinationPath(editText);

    QString destiantion_path(editText);
    QUrl destination_path_url(destiantion_path.prepend(m_URL_FILE_SCHEME));
    this->setDestinationPathUrl(destination_path_url);
}
