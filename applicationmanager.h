#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>
#include <QDebug>

#include "base.h"
#include "sourcemanager.h"
#include "destinationmanager.h"
#include "filemanager.h"
#include "thumbnailgenerator.h"
#include "settingsmanager.h"

class ApplicationManager : public Base
{
    Q_OBJECT

private:
    SourceManager m_sourceManager;
    DestinationManager m_destinationManager;
    FileManager m_fileManager;
    ThumbnailGenerator m_thumbnailGenerator;
    SettingsManager m_settingsManager;

public:
    explicit ApplicationManager(QObject *parent = NULL);
    ~ApplicationManager();

public:
    void initialize();
    SourceManager *sourceManager();
    DestinationManager *destinationManager();
    FileManager *fileManager();
    ThumbnailGenerator *thumbnailGenerator();

public slots:
    void onCompleted();
};

#endif // APPLICATIONMANAGER_H
