#ifndef CHIGHRESTIMER_H
#define CHIGHRESTIMER_H

class HighResTimer
{
public:
    HighResTimer();
    ~HighResTimer();

    static int Init();

    void Reset();
    double ElapsedMillis();
    __int64 ElapsedMicro();
    
private:
    LARGE_INTEGER start;
    LARGE_INTEGER stop;

    static double frequency;
};

#endif //CHIGHRESTIMER_H