#include "Log.h"

Log::Log(): LogNWNX() {}

void Log::LogStr(const char* message) {
    wxLogMessage(message);
}
