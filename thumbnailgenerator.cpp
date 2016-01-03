#include "thumbnailgenerator.h"

ThumbnailGenerator::ThumbnailGenerator(QObject *parent) :
    Base("TG", parent),
    m_thumbnailGeneratorMutex(),
    m_thumbnailGeneratorWaitCondition(),
    m_thumbnailGeneratorImplThread(this),
    m_thumbnailGeneratorImplSptr(new ThumbnailGeneratorImpl(&m_thumbnailGeneratorMutex, &m_thumbnailGeneratorWaitCondition))
{
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailRows(int*)), this, SIGNAL(thumbnailRows(int*)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailColumns(int*)), this, SIGNAL(thumbnailColumns(int*)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailMaxWidth(int*)), this, SIGNAL(thumbnailMaxWidth(int*)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailMaxHeight(int*)), this, SIGNAL(thumbnailMaxHeight(int*)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(isEnabledChanged(bool)), this, SIGNAL(isEnabledChanged(bool)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailRowsChanged(int)), this, SIGNAL(thumbnailRowsChanged(int)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailColumnsChanged(int)), this, SIGNAL(thumbnailColumnsChanged(int)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailMaxWidthChanged(int)), this, SIGNAL(thumbnailMaxWidthChanged(int)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailMaxHeightChanged(int)), this, SIGNAL(thumbnailMaxHeightChanged(int)));
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(stateChanged(Enums::State)), this, SIGNAL(stateChanged(Enums::State)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(stateDescriptionChanged(QString)), this, SIGNAL(stateDescriptionChanged(QString)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(progressChanged(float)), this, SIGNAL(progressChanged(float)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(progressStringChanged(QString)), this, SIGNAL(progressStringChanged(QString)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(errorsChanged(int)), this, SIGNAL(errorsChanged(int)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(warningsChanged(int)), this, SIGNAL(warningsChanged(int)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(skippedChanged(int)), this, SIGNAL(skippedChanged(int)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(overwrittenChanged(int)), this, SIGNAL(overwrittenChanged(int)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(processedChanged(int)), this, SIGNAL(processedChanged(int)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(currentInputFileChanged(QString)), this, SIGNAL(currentInputFileChanged(QString)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(thumbnailUrlChanged(QUrl)), this, SIGNAL(thumbnailUrlChanged(QUrl)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(isSourcePathUrlValid(bool*)), this, SIGNAL(isSourcePathUrlValid(bool*)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(isDestinationPathUrlValid(bool*)), this, SIGNAL(isDestinationPathUrlValid(bool*)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(sourcePath(QString*)), this, SIGNAL(sourcePath(QString*)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(destinationPath(QString*)), this, SIGNAL(destinationPath(QString*)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(videoFiles(QStringList*)), this, SIGNAL(videoFiles(QStringList*)), Qt::BlockingQueuedConnection);
    QObject::connect(m_thumbnailGeneratorImplSptr.data(), SIGNAL(overwriteOutputFiles(bool*)), this, SIGNAL(overwriteOutputFiles(bool*)), Qt::BlockingQueuedConnection);

    this->debug("Thumbnail generator created");
}

ThumbnailGenerator::~ThumbnailGenerator()
{
    this->onStop();

    m_thumbnailGeneratorImplThread.quit();
    m_thumbnailGeneratorImplThread.wait(3000);

    this->debug("Thumbnail generator disposed of");
}

void ThumbnailGenerator::initialize()
{
    m_thumbnailGeneratorImplSptr.data()->initialize();
    m_thumbnailGeneratorImplSptr.data()->moveToThread(&m_thumbnailGeneratorImplThread);

    m_thumbnailGeneratorImplThread.start();

    this->debug("Initialized");
}

bool ThumbnailGenerator::isEnabled() const
{
    return m_thumbnailGeneratorImplSptr.data()->isEnabled();
}

int ThumbnailGenerator::thumbnailRows() const
{
    return m_thumbnailGeneratorImplSptr.data()->thumbnailRows();
}

int ThumbnailGenerator::thumbnailColumns() const
{
    return m_thumbnailGeneratorImplSptr.data()->thumbnailColumns();
}

int ThumbnailGenerator::thumbnailMaxWidth() const
{
    return m_thumbnailGeneratorImplSptr.data()->thumbnailMaxWidth();
}

int ThumbnailGenerator::thumbnailMaxHeight() const
{
    return m_thumbnailGeneratorImplSptr.data()->thumbnailMaxHeight();
}

Enums::State ThumbnailGenerator::state() const
{
    return m_thumbnailGeneratorImplSptr.data()->state();
}

QString ThumbnailGenerator::stateDescription() const
{
    return m_thumbnailGeneratorImplSptr.data()->stateDescription();
}

float ThumbnailGenerator::progress() const
{
    return m_thumbnailGeneratorImplSptr.data()->progress();
}

QString ThumbnailGenerator::progressString() const
{
    return m_thumbnailGeneratorImplSptr.data()->progressString();
}

int ThumbnailGenerator::errors() const
{
    return m_thumbnailGeneratorImplSptr.data()->errors();
}

int ThumbnailGenerator::warnings() const
{
    return m_thumbnailGeneratorImplSptr.data()->warnings();
}

int ThumbnailGenerator::skipped() const
{
    return m_thumbnailGeneratorImplSptr.data()->skipped();
}

int ThumbnailGenerator::overwritten() const
{
    return m_thumbnailGeneratorImplSptr.data()->overwritten();
}

int ThumbnailGenerator::processed() const
{
    return m_thumbnailGeneratorImplSptr.data()->processed();
}

QString ThumbnailGenerator::currentInputFile() const
{
    return m_thumbnailGeneratorImplSptr.data()->currentInputFile();
}

QUrl ThumbnailGenerator::thumbnailUrl() const
{
    return m_thumbnailGeneratorImplSptr.data()->thumbnailUrl();
}

void ThumbnailGenerator::onUpdateThumbnailRows(int thumbnailRows)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onUpdateThumbnailRows", Qt::QueuedConnection, Q_ARG(int, thumbnailRows));
}

void ThumbnailGenerator::onUpdateThumbnailColumns(int thumbnailColumns)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onUpdateThumbnailColumns", Qt::QueuedConnection, Q_ARG(int, thumbnailColumns));
}

void ThumbnailGenerator::onUpdateThumbnailMaxWidth(int thumbnailMaxWidth)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onUpdateThumbnailMaxWidth", Qt::QueuedConnection, Q_ARG(int, thumbnailMaxWidth));
}

void ThumbnailGenerator::onUpdateThumbnailMaxHeight(int thumbnailMaxHeight)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onUpdateThumbnailMaxHeight", Qt::QueuedConnection, Q_ARG(int, thumbnailMaxHeight));
}

void ThumbnailGenerator::onIsSourcePathUrlValidChanged(bool isSourcePathUrlValid)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onIsSourcePathUrlValidChanged", Qt::QueuedConnection, Q_ARG(bool, isSourcePathUrlValid));
}

void ThumbnailGenerator::onIsDestinationPathUrlValidChanged(bool isDestinationPathUrlValid)
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onIsDestinationPathUrlValidChanged", Qt::QueuedConnection, Q_ARG(bool, isDestinationPathUrlValid));
}

void ThumbnailGenerator::onGenerateThumbnails()
{
    QMetaObject::invokeMethod(m_thumbnailGeneratorImplSptr.data(), "onGenerateThumbnails", Qt::QueuedConnection);
}

void ThumbnailGenerator::onPause()
{
    m_thumbnailGeneratorMutex.lock();
    m_thumbnailGeneratorImplSptr.data()->setPause(true);
    m_thumbnailGeneratorMutex.unlock();

    this->debug("Pause event set");
}

void ThumbnailGenerator::onResume()
{
    m_thumbnailGeneratorMutex.lock();
    m_thumbnailGeneratorImplSptr.data()->setPause(false);
    m_thumbnailGeneratorWaitCondition.wakeAll();
    m_thumbnailGeneratorMutex.unlock();

    this->debug("Pause event reset");
}

void ThumbnailGenerator::onStop()
{
    m_thumbnailGeneratorMutex.lock();
    m_thumbnailGeneratorImplSptr.data()->setPause(false);
    m_thumbnailGeneratorImplSptr.data()->setStop(true);
    m_thumbnailGeneratorWaitCondition.wakeAll();
    m_thumbnailGeneratorMutex.unlock();

    this->debug("Stop event set");
}
