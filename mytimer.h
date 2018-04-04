#ifndef _MYTIME_H__
#define _MYTIME_H__

#include <sys/time.h>

class Timer
{
  private:
    double start_time;
    double get_current_time()
    {
        struct timeval time;
        gettimeofday(&time, 0);
        return 1.0 * time.tv_sec + time.tv_usec / 1000000.0;
    }

  public:
    Timer()
    {
        reset();
    }
    void reset()
    {
        start_time = get_current_time();
    }
    double get_time()
    {
        return get_current_time() - start_time;
    }
};

#endif