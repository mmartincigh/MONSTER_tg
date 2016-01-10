#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>

#include "base.h"

class WindowManager : public Base
{
    Q_OBJECT
    Q_PROPERTY(Qt::WindowFlags windowFlags READ windowFlags CONSTANT)

private:
    Qt::WindowFlags m_windowFlags;

public:
    explicit WindowManager(QObject *parent = NULL);
    ~WindowManager();

public:
    Qt::WindowFlags windowFlags() const;
};

#endif // WINDOWMANAGER_H
