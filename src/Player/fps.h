#pragma once
#include <QTime>

//////////////////////////////////////////////////////////////////////////
// Calculate FPS
class QFPS
{
public:
    QFPS();
   
    void start();
    void stop();
    bool isStart();

    void incFrame();
    
    //calculate fps updated per second
    //retrun negative value if fps hasn't updated yet
    float getCurrentFPS();

    //calculate difference between actural time elapsed with m_nFrames and iExpectedFPS * m_nFrames
    //return value is micro-second
    //negative value means too fast, positive value means to slow
    int getFPSDifference(int iExpectedFPS);
private:
    int m_nFrames = 0;
    QTime m_timer;
    bool m_bStart = false;
};
