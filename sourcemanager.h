#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <QUrl>

#include "base.h"

class SourceManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(QString sourcePath READ sourcePath NOTIFY sourcePathChanged)
    Q_PROPERTY(QUrl sourcePathUrl READ sourcePathUrl NOTIFY sourcePathUrlChanged)
    Q_PROPERTY(bool isSourcePathUrlValid READ isSourcePathUrlValid NOTIFY isSourcePathUrlValidChanged)

private:
    static const QString m_URL_FILE_SCHEME;
    QString m_sourcePath;
    QUrl m_sourcePathUrl;
    bool m_isSourcePathUrlValid;

public:
    explicit SourceManager(QObject *parent = NULL);
    ~SourceManager();

public:
    void initialize();
    QString sourcePath() const;
    QUrl sourcePathUrl() const;
    bool isSourcePathUrlValid() const;

private:
    void setSourcePath(const QString &sourcePath);
    void setSourcePathUrl(const QUrl &sourcePathUrl);
    void setIsSourcePathUrlValid(bool isSourcePathUrlValid);

public slots:
    void onUpdateSourcePath(const QString &sourcePath);
    void onUpdateSourcePathUrl(const QUrl &sourcePathUrl);
    void onIsSourcePathUrlValid(bool *isSourcePathUrlValid);
    void onSourcePathUrl(QUrl *sourcePathUrl);

private slots:
    void onSourcePathChanged(const QString &sourcePath);
    void onSourcePathUrlChanged(const QUrl &sourcePathUrl);

signals:
    void sourcePathUrl(QUrl *sourcePath);
    void sourcePathChanged(const QString &sourcePath);
    void sourcePathUrlChanged(const QUrl &sourcePathUrl);
    void isSourcePathUrlValidChanged(bool isSourcePathUrlValid);
};

#endif // SOURCEMANAGER_H
