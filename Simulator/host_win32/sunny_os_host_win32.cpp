// SUNNY_OS Win32 host (simulator).
// This file replaces Wand's LVGL.Simulator.cpp as the entry point.

#include <Windows.h>

#include "config/ui_config.h"
#include "lv_drivers/win32drv/win32drv.h"
#include "lv_fs_if/lv_fs_if.h"
#include "lvgl.h"
#include "runtime/vsun_app.h"

int main()
{
    lv_init();
    lv_fs_if_init();

    const HINSTANCE instance = GetModuleHandleW(nullptr);
    const HICON icon = LoadIconW(nullptr, IDI_APPLICATION);

    if(!lv_win32_init(
           instance,
           SW_SHOW,
           SUNNY_UI_SIM_HOR_RES,
           SUNNY_UI_SIM_VER_RES,
           icon)) {
        return -1;
    }

    lv_win32_add_all_input_devices_to_group(nullptr);

    vsun::app_init();

    while(!lv_win32_quit_signal) {
        lv_timer_handler();
        vsun::app_update();
        Sleep(1);
    }

    vsun::app_uninit();
    return 0;
}
