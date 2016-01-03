#ifndef THUMBNAILGENERATOR_H
#define THUMBNAILGENERATOR_H

#include <QThread>
#include <QSharedPointer>

#include "base.h"
#include "thumbnailgeneratorimpl.h"

class ThumbnailGenerator : public Base
{
    Q_OBJECT
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(int thumbnailRows READ thumbnailRows NOTIFY thumbnailRowsChanged)
    Q_PROPERTY(int thumbnailColumns READ thumbnailColumns NOTIFY thumbnailColumnsChanged)
    Q_PROPERTY(int thumbnailMaxWidth READ thumbnailMaxWidth NOTIFY thumbnailMaxWidthChanged)
    Q_PROPERTY(int thumbnailMaxHeight READ thumbnailMaxHeight NOTIFY thumbnailMaxHeightChanged)
    Q_PROPERTY(Enums::State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString stateDescription READ stateDescription NOTIFY stateDescriptionChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString progressString READ progressString NOTIFY progressStringChanged)
    Q_PROPERTY(int errors READ errors NOTIFY errorsChanged)
    Q_PROPERTY(int warnings READ warnings NOTIFY warningsChanged)
    Q_PROPERTY(int skipped READ skipped NOTIFY skippedChanged)
    Q_PROPERTY(int overwritten READ overwritten NOTIFY overwrittenChanged)
    Q_PROPERTY(int processed READ processed NOTIFY processedChanged)
    Q_PROPERTY(QString currentInputFile READ currentInputFile NOTIFY currentInputFileChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)

private:
    QMutex m_thumbnailGeneratorMutex;
    QWaitCondition m_thumbnailGeneratorWaitCondition;
    QThread m_thumbnailGeneratorImplThread;
    QSharedPointer<ThumbnailGeneratorImpl> m_thumbnailGeneratorImplSptr;

public:
    explicit ThumbnailGenerator(QObject *parent = NULL);
    ~ThumbnailGenerator();

public:
    void initialize();
    bool isEnabled() const;
    int thumbnailRows() const;
    int thumbnailColumns() const;
    int thumbnailMaxWidth() const;
    int thumbnailMaxHeight() const;
    Enums::State state() const;
    QString stateDescription() const;
    float progress() const;
    QString progressString() const;
    int errors() const;
    int warnings() const;
    int skipped() const;
    int overwritten() const;
    int processed() const;
    QString currentInputFile() const;
    QUrl thumbnailUrl() const;

public slots:
    void onUpdateThumbnailRows(int thumbnailRows);
    void onUpdateThumbnailColumns(int thumbnailColumns);
    void onUpdateThumbnailMaxWidth(int thumbnailMaxWidth);
    void onUpdateThumbnailMaxHeight(int thumbnailMaxHeight);
    void onIsSourcePathUrlValidChanged(bool isSourcePathUrlValid);
    void onIsDestinationPathUrlValidChanged(bool isDestinationPathUrlValid);
    void onGenerateThumbnails();
    void onPause();
    void onResume();
    void onStop();

signals:
    void thumbnailRows(int *thumbnailRows);
    void thumbnailColumns(int *thumbnailColumns);
    void thumbnailMaxWidth(int *thumbnailMaxWidth);
    void thumbnailMaxHeight(int *thumbnailMaxHeight);
    void isEnabledChanged(bool isEnabled);
    void thumbnailRowsChanged(int thumbnailRows);
    void thumbnailColumnsChanged(int thumbnailColumns);
    void thumbnailMaxWidthChanged(int thumbnailMaxWidth);
    void thumbnailMaxHeightChanged(int thumbnailMaxHeight);
    void stateChanged(Enums::State state);
    void stateDescriptionChanged(const QString &stateDescription);
    void progressChanged(float progress);
    void progressStringChanged(const QString &progressString);
    void errorsChanged(int errors);
    void warningsChanged(int warnings);
    void skippedChanged(int skipped);
    void overwrittenChanged(int overwritten);
    void processedChanged(int processed);
    void currentInputFileChanged(const QString &currentInputFile);
    void thumbnailUrlChanged(const QUrl &thumbnailUrl);
    void isSourcePathUrlValid(bool *isSourcePathUrlValid);
    void isDestinationPathUrlValid(bool *isDestinationPathUrlValid);
    void sourcePath(QString *sourcePath);
    void destinationPath(QString *destinationPath);
    void videoFiles(QStringList *videoFiles);
    void overwriteOutputFiles(bool *overwriteOutputFiles);
};

#endif // THUMBNAILGENERATOR_H
