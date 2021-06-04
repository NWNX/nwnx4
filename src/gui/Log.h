#ifndef MAIN_LOG_H
#define MAIN_LOG_H

#include "stdwx.h"
#include "../misc/log.h"

class Log: public LogNWNX {
public:
    Log();
protected:
    void LogStr(const char* message);
};


#endif //MAIN_LOG_H
