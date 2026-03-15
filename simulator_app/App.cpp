#include "App.h"

#include "lvgl/lvgl.h"
#include "runtime/vsun_app.h"

namespace {

lv_timer_t* g_update_timer = nullptr;

void update_timer_cb(lv_timer_t* timer)
{
    (void)timer;
    vsun::app_update();
}

} // namespace

void App_Init(void)
{
    vsun::app_init();

    // Drive SUNNY_OS's update tick from LVGL's timer system.
    // This keeps the host simulator's main loop unmodified.
    if (!g_update_timer)
    {
        g_update_timer = lv_timer_create(update_timer_cb, 16, nullptr);
    }
}

void App_Uninit(void)
{
    if (g_update_timer)
    {
        lv_timer_del(g_update_timer);
        g_update_timer = nullptr;
    }

    vsun::app_uninit();
}
