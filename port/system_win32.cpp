#include "port/system.h"

#if defined(_WIN32)
#include "lv_drivers/win32drv/win32drv.h"
#endif

namespace vsun::port {

void request_shutdown()
{
#if defined(_WIN32)
    // LVGL Win32 driver exposes this quit flag for the simulator host loop.
    lv_win32_quit_signal = true;
#endif
}

} // namespace vsun::port

