#include <QFileInfo>

#include "sourcemanager.h"
#include "utils.h"

const int SourceManager::m_MAX_MODEL_SIZE(5);
#if defined(Q_OS_WIN)
const QString SourceManager::m_URL_FILE_SCHEME("file:///");
#else
const QString SourceManager::m_URL_FILE_SCHEME("file://");
#endif

SourceManager::SourceManager(QObject *parent) :
    Base("SRCM", parent),
    m_editText(""),
    m_sourcePathModel(),
    m_sourcePath(""),
    m_sourcePathUrl(""),
    m_isSourcePathUrlValid(false)
{
    QObject::connect(this, SIGNAL(editTextChanged(QString)), this, SLOT(onEditTextChanged(QString)));

    this->debug("Source manager created");
}

SourceManager::~SourceManager()
{
    this->debug("Source manager disposed of");
}

void SourceManager::initialize()
{
    QStringList source_path_model;
    emit this->sourcePathModel(&source_path_model);
    this->setSourcePathModel(source_path_model);

    this->debug("Initialized");
}

QStringList SourceManager::sourcePathModel() const
{
    return m_sourcePathModel;
}

QUrl SourceManager::sourcePathUrl() const
{
    return m_sourcePathUrl;
}

bool SourceManager::isSourcePathUrlValid() const
{
    return m_isSourcePathUrlValid;
}

void SourceManager::setEditText(const QString &editText)
{
    if (m_editText == editText)
    {
        return;
    }

    m_editText = editText;

    this->debug("Edit text changed: " + m_editText);

    emit this->editTextChanged(m_editText);
}

void SourceManager::setSourcePathModel(const QStringList &sourcePathModel)
{
    if (m_sourcePathModel == sourcePathModel)
    {
        return;
    }

    m_sourcePathModel = sourcePathModel;

    this->debug("Source path model changed: " + m_sourcePathModel.join('|'));

    emit this->sourcePathModelChanged(m_sourcePathModel);
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

    this->debug("Is source path URL valid changed: " + QString(m_isSourcePathUrlValid ? "true" : "false"));

    emit this->isSourcePathUrlValidChanged(m_isSourcePathUrlValid);
}

void SourceManager::onSourcePath(QString *sourcePath)
{
    if (sourcePath == NULL)
    {
        return;
    }

    *sourcePath = m_sourcePath;
}

void SourceManager::onIsSourcePathUrlValid(bool *isSourcePathUrlValid)
{
    if (isSourcePathUrlValid == NULL)
    {
        return;
    }

    *isSourcePathUrlValid = m_isSourcePathUrlValid;
}

void SourceManager::onUpdateEditText(const QString &editText)
{
    this->setEditText(editText);
}

void SourceManager::onUpdateEditText(const QUrl &editText)
{
    QString edit_text = Utils::urlToString(editText);
    this->setEditText(edit_text);
}

void SourceManager::onGenerateThumbnails()
{
    // Do not change the model if the current path is invalid.
    if (!m_isSourcePathUrlValid)
    {
        return;
    }

    // Get the source path model.
    QStringList source_path_model = m_sourcePathModel;

    // Check whether the model already contains the current path.
    if (source_path_model.contains(m_editText))
    {
        // Move the current path to the top.
        source_path_model.removeAll(m_editText);
        source_path_model.prepend(m_editText);
        this->setSourcePathModel(source_path_model);

        return;
    }

    // Make room for another entry if necessary.
    while (source_path_model.size() >= m_MAX_MODEL_SIZE)
    {
        // Remove the oldest (last) entry from the model.
        source_path_model.removeLast();
    }

    // Prepend the current path to the model.
    source_path_model.prepend(m_editText);
    this->setSourcePathModel(source_path_model);
}

void SourceManager::onEditTextChanged(const QString &editText)
{
    QFileInfo source_path_info(editText);
    bool is_source_path_valid = source_path_info.exists() && source_path_info.isDir();
    this->setIsSourcePathUrlValid(is_source_path_valid);

    if (!m_isSourcePathUrlValid)
    {
        // Do not update the source path URL.

        return;
    }

    this->setSourcePath(editText);

    QString source_path(editText);
    QUrl source_path_url(source_path.prepend(m_URL_FILE_SCHEME));
    this->setSourcePathUrl(source_path_url);
}
