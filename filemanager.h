#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QUrl>

#include "base.h"

class FileManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(int videoFilesCount READ videoFilesCount NOTIFY videoFilesCountChanged)
    Q_PROPERTY(bool overwriteOutputFiles READ overwriteOutputFiles WRITE setOverwriteOutputFiles NOTIFY overwriteOutputFilesChanged)

private:
    static const QString m_MP4_NAME_FILTER;
    static const QString m_AVI_NAME_FILTER;
    static const QString m_WMV_NAME_FILTER;
    static const QStringList m_NAME_FILTERS;
    QStringList m_videoFiles;
    int m_videoFilesCount;
    bool m_overwriteOutputFiles;

public:
    explicit FileManager(QObject *parent = NULL);
    ~FileManager();

public:
    void initialize();
    QStringList videoFiles() const;
    int videoFilesCount() const;
    bool overwriteOutputFiles() const;
    void setOverwriteOutputFiles(bool overwriteOutputFiles);

private:
    void setVideoFiles(const QStringList &videoFiles);
    void setVideoFilesCount(int videoFilesCount);

public slots:
    void onSourcePathChanged(const QString &sourcePath);
    void onVideoFiles(QStringList *videoFiles);
    void onOverwriteOutputFiles(bool *overwriteOutputFiles);
    void onReloadVideoFiles();
    void onReloadVideoFiles(const QString &sourcePath);
    void onReloadVideoFiles(const QUrl &sourcePathUrl);

signals:
    void videoFilesChanged(const QStringList &videofiles);
    void videoFilesCountChanged(bool videoFilesCount);
    void overwriteOutputFilesChanged(bool overwriteOutputFiles);
    void sourcePath(QString *sourcePath);
};

#endif // FILEMANAGER_H
