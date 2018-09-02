#ifndef BASE_H
#define BASE_H

#include "logmanager.h"

class Base : public LogManager
{
public:
    explicit Base(const QString &logTag, QObject *parent = nullptr);
};

#endif // BASE_H
