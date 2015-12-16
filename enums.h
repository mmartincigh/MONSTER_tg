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

    enum AspectRatio
    {
        Ratio_4_3,
        Ratio_16_9
    };
};

#endif // ENUMS
