#include "windowmanager.h"

WindowManager::WindowManager(QObject *parent) :
    Base("WM", parent),
    #if defined(Q_OS_WIN)
    m_windowFlags(Qt::Window)
    #else
    m_windowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint)
    #endif
{
    this->debug("Window manager manager created");
}

WindowManager::~WindowManager()
{
    this->debug("Window manager manager created");
}

Qt::WindowFlags WindowManager::windowFlags() const
{
    return m_windowFlags;
}
