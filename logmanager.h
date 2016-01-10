#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QMutex>
#include <QDebug>

class LogManager : public QObject
{
    Q_OBJECT

public:
    static const QString LOG_FILENAME;
    static const QString LOG_HEADER;

private:
    static const QString m_LOG_DATE_TIME_FORMAT;
    static QMutex m_mutex;
    QString m_logTag;

public:
    explicit LogManager(const QString &logTag, QObject *parent = NULL);
    ~LogManager();

public:
    static void messageHandler(QtMsgType messageType, const QMessageLogContext &messageLogContext, const QString &message);

protected:
    void debug(const QString &debugMessage);
    void warning(const QString &warningMessage);
    void error(const QString &errorMessage);

signals:
    void debugMessage(const QString &debugMessage);
    void warningMessage(const QString &warningMessage);
};

#endif // LOGMANAGER_H
