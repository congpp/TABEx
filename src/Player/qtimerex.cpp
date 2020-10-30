#include "qtimerex.h"

QTimerEx::QTimerEx()
{
    reset();
}

void QTimerEx::reset()
{
    m_nStartTicks = 0;
    m_nPausedTicks = 0;
    m_bPaused = false;
    m_bStarted = false;
}

void QTimerEx::start()
{
    //Start the timer
    m_bStarted = true;

    //Unpause the timer
    m_bPaused = false;

    //Get the current clock time
    m_timer.start();
    m_nStartTicks = 0;
}

qint64 QTimerEx::restart()
{
    start();
    return  m_nStartTicks;
}

void QTimerEx::stop()
{
    //Stop the timer
    m_bStarted = false;

    //Unpause the timer
    m_bPaused = false;
}

void QTimerEx::pause()
{
    //If the timer is running and isn't already paused
    if ((m_bStarted == true) && (m_bPaused == false))
    {
        //Pause the timer
        m_bPaused = true;

        //Calculate the paused ticks
        m_nPausedTicks = m_timer.elapsed() - m_nStartTicks;
    }
}

void QTimerEx::resume()
{
    //If the timer is paused
    if (m_bPaused == true)
    {
        //Unpause the timer
        m_bPaused = false;

        //Reset the starting ticks
        m_nStartTicks = m_timer.elapsed() - m_nPausedTicks;

        //Reset the paused ticks
        m_nPausedTicks = 0;
    }
}

qint64 QTimerEx::elapsed()
{
    //If the timer is running
    if (m_bStarted == true)
    {
        //If the timer is paused
        if (m_bPaused == true)
        {
            //Return the number of ticks when the timer was paused
            return m_nPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            qint64 tick = m_timer.elapsed();
            return tick - m_nStartTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

bool QTimerEx::isStarted()
{
    return m_bStarted;
}

bool QTimerEx::isPaused()
{
    return m_bPaused;
}
