
#include "fps.h"

//////////////////////////////////////////////////////////////////////////
// Calculate FPS
QFPS::QFPS()
{

}

void QFPS::start()
{
    m_timer.restart();
    m_bStart = true;
    m_nFrames = 0;
}

void QFPS::stop()
{
    m_bStart = false;
}

bool QFPS::isStart()
{
    return m_bStart;
}

void QFPS::incFrame()
{
    m_nFrames++;
}

float QFPS::getCurrentFPS()
{
    if (!m_bStart || m_timer.elapsed() < 1000)
        return -1;

    float fFPS = m_nFrames / (m_timer.elapsed() / 1000.0f);

    m_timer.restart();
    m_nFrames = 0;

    return fFPS;
}

int QFPS::getFPSDifference(int iExpectedFPS)
{
    int actualTime = m_timer.elapsed();
    int expectedTime = iExpectedFPS * m_nFrames;
    return actualTime - expectedTime;
}
