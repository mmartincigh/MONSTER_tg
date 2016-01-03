#ifndef DESTINATIONMANAGER_H
#define DESTINATIONMANAGER_H

#include <QUrl>

#include "base.h"

class DestinationManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(QStringList destinationPathModel READ destinationPathModel NOTIFY destinationPathModelChanged)
    Q_PROPERTY(QUrl destinationPathUrl READ destinationPathUrl NOTIFY destinationPathUrlChanged)
    Q_PROPERTY(bool isDestinationPathUrlValid READ isDestinationPathUrlValid NOTIFY isDestinationPathUrlValidChanged)

private:
    static const int m_MAX_MODEL_SIZE;
    static const QString m_URL_FILE_SCHEME;
    QString m_editText;
    QStringList m_destinationPathModel;
    QString m_destinationPath;
    QUrl m_destinationPathUrl;
    bool m_isDestinationPathUrlValid;

public:
    explicit DestinationManager(QObject *parent = NULL);
    ~DestinationManager();

public:
    void initialize();
    QStringList destinationPathModel() const;
    QUrl destinationPathUrl() const;
    bool isDestinationPathUrlValid() const;

private:
    void setEditText(const QString &editText);
    void setDestinationPathModel(const QStringList &destinationPathModel);
    void setDestinationPath(const QString &destinationPath);
    void setDestinationPathUrl(const QUrl &destinationPathUrl);
    void setIsDestinationPathUrlValid(bool isDestinationPathUrlValid);

public slots:
    void onDestinationPath(QString *destinationPath);
    void onIsDestinationPathUrlValid(bool *isDestinationPathUrlValid);
    void onUpdateEditText(const QString &editText);
    void onUpdateEditText(const QUrl &editText);
    void onGenerateThumbnails();

private slots:
    void onEditTextChanged(const QString &editText);

signals:
    void editTextChanged(const QString &editText);
    void destinationPathModel(QStringList *destinationPathModel);
    void destinationPathModelChanged(const QStringList &destinationPathModel);
    void destinationPathChanged(const QString &destinationPath);
    void destinationPathUrlChanged(const QUrl &destinationPathUrl);
    void isDestinationPathUrlValidChanged(bool isDestinationPathUrlValid);
};

#endif // DESTINATIONMANAGER_H
