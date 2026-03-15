#include "desktop/desktop_slice.h"

namespace vsun::desktop {

void DesktopSlice::on_root_size(lv_event_t* e)
{
    auto* self = static_cast<DesktopSlice*>(lv_event_get_user_data(e));
    if(!self) return;
    if(lv_event_get_code(e) == LV_EVENT_SIZE_CHANGED) self->relayout();
}

void DesktopSlice::on_launcher_clicked(lv_event_t* e)
{
    auto* self = static_cast<DesktopSlice*>(lv_event_get_user_data(e));
    if(!self) return;
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) self->do_open_launcher();
}

void DesktopSlice::on_power_clicked(lv_event_t* e)
{
    auto* self = static_cast<DesktopSlice*>(lv_event_get_user_data(e));
    if(!self) return;
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) self->do_shutdown();
}

void DesktopSlice::build(lv_obj_t* parent)
{
    model_.reset();
    view_.build(parent);

    if(view_.root()) lv_obj_add_event_cb(view_.root(), on_root_size, LV_EVENT_SIZE_CHANGED, this);
    if(view_.launcher_btn()) lv_obj_add_event_cb(view_.launcher_btn(), on_launcher_clicked, LV_EVENT_CLICKED, this);
    if(view_.power_btn()) lv_obj_add_event_cb(view_.power_btn(), on_power_clicked, LV_EVENT_CLICKED, this);

    relayout();
}

void DesktopSlice::relayout()
{
    view_.relayout();
}

void DesktopSlice::do_open_launcher()
{
    if(cb_.open_launcher) cb_.open_launcher(cb_.user);
}

void DesktopSlice::do_shutdown()
{
    if(cb_.shutdown) cb_.shutdown(cb_.user);
}

} // namespace vsun::desktop
