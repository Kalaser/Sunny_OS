#include "vsun_app.h"

#include "core/app_controller.h"

namespace vsun {
namespace {

core::AppController* g_controller = nullptr;

} // namespace

void app_init()
{
    if(!g_controller) g_controller = new core::AppController();
    g_controller->init();
}

void app_update()
{
    if(g_controller) g_controller->update();
}

void app_uninit()
{
    if(g_controller) g_controller->uninit();
}

} // namespace vsun
