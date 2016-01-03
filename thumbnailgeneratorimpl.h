#ifndef THUMBNAILGENERATORIMPL_H
#define THUMBNAILGENERATORIMPL_H

#include <QImage>
#include <QUrl>
#include <QWaitCondition>

#include "base.h"
#include "enums.h"

class ThumbnailGeneratorImpl : public Base
{
    Q_OBJECT

private:
    static const QString m_CURRENT_INPUT_FILE_NONE;
    static const QString m_OUTPUT_FILE_EXTENSION;
    bool m_isEnabled;
    int m_thumbnailRows;
    int m_thumbnailColumns;
    int m_thumbnailMaxWidth;
    int m_thumbnailMaxHeight;
    QImage m_noThumbnailImage;
    Enums::State m_state;
    QString m_stateDescription;
    bool m_pause;
    bool m_stop;
    float m_progress;
    QString m_progressString;
    int m_errors;
    int m_warnings;
    int m_skipped;
    int m_overwritten;
    int m_processed;
    QString m_currentInputFile;
    QUrl m_thumbnailUrl;
    QMutex *m_mutex;
    QWaitCondition *m_waitCondition;

public:
    explicit ThumbnailGeneratorImpl(QMutex *mutex, QWaitCondition *waitCondition, QObject *parent = NULL);
    ~ThumbnailGeneratorImpl();

public:
    void initialize();
    bool isEnabled() const;
    int thumbnailRows() const;
    int thumbnailColumns() const;
    int thumbnailMaxWidth() const;
    int thumbnailMaxHeight() const;
    Enums::State state() const;
    QString stateDescription() const;
    void setPause(bool pause);
    void setStop(bool stop);
    float progress() const;
    QString progressString() const;
    int errors() const;
    int warnings() const;
    int skipped() const;
    int overwritten() const;
    int processed() const;
    QString currentInputFile() const;
    QUrl thumbnailUrl() const;

private:
    void setIsEnabled(bool isEnabled);
    void setThumbnailRows(int thumbnailRows);
    void setThumbnailColumns(int thumbnailColumns);
    void setThumbnailMaxWidth(int thumbnailMaxWidth);
    void setThumbnailMaxHeight(int thumbnailMaxHeight);
    void setNoThumbnailImage(const QImage &noThumbnailImage);
    void setState(Enums::State state);
    void setStateDescription(const QString &stateDescription);
    void setProgress(float progress);
    void setProgressString(const QString &progressString);
    void setErrors(int errors);
    void setWarnings(int warnings);
    void setSkipped(int skipped);
    void setOverwritten(int overwritten);
    void setProcessed(int processed);
    void setCurrentInputFile(const QString &currentInputFile);
    void setThumbnailUrl(const QUrl &thumbnailUrl);
    bool checkIfEnabled();
    bool processStateCheckpoint();

public slots:
    void onUpdateThumbnailRows(int thumbnailRows);
    void onUpdateThumbnailColumns(int thumbnailColumns);
    void onUpdateThumbnailMaxWidth(int thumbnailMaxWidth);
    void onUpdateThumbnailMaxHeight(int thumbnailMaxHeight);
    void onIsSourcePathUrlValidChanged(bool isSourcePathUrlValid);
    void onIsDestinationPathUrlValidChanged(bool isDestinationPathUrlValid);
    void onGenerateThumbnails();

private slots:
    void onStateChanged(Enums::State state);
    void onProgressChanged(float progress);

signals:
    void thumbnailRows(int *thumbnailRows);
    void thumbnailColumns(int *thumbnailColumns);
    void thumbnailMaxWidth(int *thumbnailMaxWidth);
    void thumbnailMaxHeight(int *thumbnailMaxHeight);
    void isEnabledChanged(bool isEnabled);
    void thumbnailRowsChanged(int thumbnailRows);
    void thumbnailColumnsChanged(int thumbnailCoolumns);
    void thumbnailMaxWidthChanged(int thumbnailMaxWidth);
    void thumbnailMaxHeightChanged(int thumbnailMaxHeight);
    void noThumbnailImageChanged(const QImage &noThumbnailImage);
    void stateChanged(Enums::State state);
    void stateDescriptionChanged(const QString &stateDescription);
    void pauseChanged(bool pause);
    void stopChanged(bool stop);
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
    void running();
    void paused();
    void stopped();
};

#endif // THUMBNAILGENERATORIMPL_H
