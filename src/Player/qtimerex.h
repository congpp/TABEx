#ifndef QTIMEREX_H
#define QTIMEREX_H
#include <QElapsedTimer>

class QTimerEx
{
public:
    QTimerEx();

    void reset();

    void start();

    qint64 restart();

    void stop();

    void pause();

    void resume();

    qint64 elapsed();

    bool isStarted();

    bool isPaused();

private:
    //The clock time when the timer started
    qint64 m_nStartTicks;

    //The ticks stored when the timer was paused
    qint64 m_nPausedTicks;

    //The timer status
    bool m_bPaused;
    bool m_bStarted;

    QElapsedTimer m_timer;
};

#endif // QTIMEREX_H
