#ifndef DESTINATIONMANAGER_H
#define DESTINATIONMANAGER_H

#include <QUrl>

#include "base.h"

class DestinationManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(QString destinationPath READ destinationPath NOTIFY destinationPathChanged)
    Q_PROPERTY(QUrl destinationPathUrl READ destinationPathUrl NOTIFY destinationPathUrlChanged)
    Q_PROPERTY(bool isDestinationPathUrlValid READ isDestinationPathUrlValid NOTIFY isDestinationPathUrlValidChanged)

private:
    static const QString m_URL_FILE_SCHEME;
    QString m_destinationPath;
    QUrl m_destinationPathUrl;
    bool m_isDestinationPathUrlValid;

public:
    explicit DestinationManager(QObject *parent = NULL);
    ~DestinationManager();

public:
    void initialize();
    QString destinationPath() const;
    QUrl destinationPathUrl() const;
    bool isDestinationPathUrlValid() const;

private:
    void setDestinationPath(const QString &destinationPath);
    void setDestinationPathUrl(const QUrl &destinationPathUrl);
    void setIsDestinationPathUrlValid(bool isDestinationPathUrlValid);

public slots:
    void onUpdateDestinationPath(const QString &destinationPath);
    void onUpdateDestinationPathUrl(const QUrl &destinationPathUrl);
    void onIsDestinationPathUrlValid(bool *isDestinationPathUrlValid);
    void onDestinationPathUrl(QUrl *destinationPathUrl);

private slots:
    void onDestinationPathChanged(const QString &destinationPath);
    void onDestinationPathUrlChanged(const QUrl &destinationPathUrl);

signals:
    void destinationPathUrl(QUrl *destinationPath);
    void destinationPathChanged(const QString &destinationPath);
    void destinationPathUrlChanged(const QUrl &destinationPathUrl);
    void isDestinationPathUrlValidChanged(bool isDestinationPathUrlValid);
};

#endif // DESTINATIONMANAGER_H
