#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <QUrl>

#include "base.h"

class SourceManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(QStringList sourcePathModel READ sourcePathModel NOTIFY sourcePathModelChanged)
    Q_PROPERTY(QUrl sourcePathUrl READ sourcePathUrl NOTIFY sourcePathUrlChanged)
    Q_PROPERTY(bool isSourcePathUrlValid READ isSourcePathUrlValid NOTIFY isSourcePathUrlValidChanged)

private:
    static const int m_MAX_MODEL_SIZE;
    static const QString m_URL_FILE_SCHEME;
    QString m_editText;
    QStringList m_sourcePathModel;
    QString m_sourcePath;
    QUrl m_sourcePathUrl;
    bool m_isSourcePathUrlValid;

public:
    explicit SourceManager(QObject *parent = NULL);
    ~SourceManager();

public:
    void initialize();
    QStringList sourcePathModel() const;
    QUrl sourcePathUrl() const;
    bool isSourcePathUrlValid() const;

private:
    void setEditText(const QString &editText);
    void setSourcePathModel(const QStringList &sourcePathModel);
    void setSourcePath(const QString &sourcePath);
    void setSourcePathUrl(const QUrl &sourcePathUrl);
    void setIsSourcePathUrlValid(bool isSourcePathUrlValid);

public slots:
    void onSourcePath(QString *sourcePath);
    void onIsSourcePathUrlValid(bool *isSourcePathUrlValid);
    void onUpdateEditText(const QString &editText);
    void onUpdateEditText(const QUrl &editText);
    void onGenerateThumbnails();

private slots:
    void onEditTextChanged(const QString &editText);

signals:
    void editTextChanged(const QString &editText);
    void sourcePathModel(QStringList *sourcePathModel);
    void sourcePathModelChanged(const QStringList &sourcePathModel);
    void sourcePathChanged(const QString &sourcePath);
    void sourcePathUrlChanged(const QUrl &sourcePathUrl);
    void isSourcePathUrlValidChanged(bool isSourcePathUrlValid);
};

#endif // SOURCEMANAGER_H
