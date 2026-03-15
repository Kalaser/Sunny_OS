#include "port/time.h"

#include <ctime>
#include <cstring>

namespace vsun::port {

bool get_local_time_hhmm(char out[6])
{
    if(!out) return false;

    std::time_t now = std::time(nullptr);
    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &now);
#else
    local_tm = *std::localtime(&now);
#endif

    char buf[6]{};
    if(std::strftime(buf, sizeof(buf), "%H:%M", &local_tm) == 0) return false;
    std::memcpy(out, buf, sizeof(buf));
    return true;
}

} // namespace vsun::port

