#ifndef ENUMS
#define ENUMS

#include <QObject>

class Enums : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)

public:
    enum State
    {
        Idle,
        Working,
        Paused,
        Stopped,
        Completed
    };
};

#endif // ENUMS
