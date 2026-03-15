#include "services/page_manager.h"

#include "config/ui_config.h"

namespace vsun::services {

PageManager::PageManager(lv_obj_t* parent) : parent_(parent) {}

void PageManager::set_parent(lv_obj_t* parent)
{
    parent_ = parent;
}

void PageManager::anim_set_x(void* var, int32_t v)
{
    lv_obj_set_x(static_cast<lv_obj_t*>(var), (lv_coord_t)v);
}

void PageManager::anim_set_opa(void* var, int32_t v)
{
    lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
}

void PageManager::hide_ready_cb(lv_anim_t* a)
{
    lv_obj_t* obj = a ? static_cast<lv_obj_t*>(a->var) : nullptr;
    if(obj) lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

void PageManager::cancel_anims(lv_obj_t* obj)
{
    if(!obj) return;
    lv_anim_del(obj, anim_set_x);
    lv_anim_del(obj, anim_set_opa);
}

void PageManager::apply_fullscreen(lv_obj_t* obj)
{
    if(!parent_ || !obj) return;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, lv_obj_get_width(parent_), lv_obj_get_height(parent_));
    lv_obj_update_layout(obj);
}

void PageManager::show_page(IPage* page)
{
    if(!page || !parent_) return;
    page->ensure_built(parent_);
    lv_obj_t* r = page->root();
    if(!r) return;
    apply_fullscreen(r);
    lv_obj_clear_flag(r, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(r);
}

void PageManager::hide_page(IPage* page)
{
    if(!page) return;
    lv_obj_t* r = page->root();
    if(r) lv_obj_add_flag(r, LV_OBJ_FLAG_HIDDEN);
}

void PageManager::transition(IPage* from, IPage* to, bool is_push, Transition t)
{
    if(!to) return;

    show_page(to);

    lv_obj_t* to_r = to->root();
    lv_obj_t* from_r = from ? from->root() : nullptr;

    if(!to_r) return;

    if(!from_r || t == Transition::None) {
        if(from) {
            from->on_hide();
            hide_page(from);
        }
        to->on_show();
        return;
    }

    cancel_anims(from_r);
    cancel_anims(to_r);

    const lv_coord_t dx = SUNNY_UI_TRANSITION_SLIDE_DX;
    const int32_t opa0 = 0;
    const int32_t opa1 = 255;

    if(t == Transition::Fade) {
        lv_obj_set_style_opa(to_r, (lv_opa_t)opa0, 0);
        lv_obj_set_style_opa(from_r, (lv_opa_t)opa1, 0);
        lv_obj_set_x(to_r, 0);
        lv_obj_set_x(from_r, 0);
    } else {
        // SlideH: push slides in from right; pop slides in from left.
        lv_obj_set_x(to_r, is_push ? dx : -dx);
        lv_obj_set_style_opa(to_r, (lv_opa_t)opa0, 0);
        lv_obj_set_style_opa(from_r, (lv_opa_t)opa1, 0);
    }

    // to: x -> 0 (optional), opa -> 255
    {
        lv_anim_t ax;
        lv_anim_init(&ax);
        lv_anim_set_var(&ax, to_r);
        lv_anim_set_exec_cb(&ax, anim_set_x);
        lv_anim_set_time(&ax, anim_time_ms_);
        lv_anim_set_path_cb(&ax, lv_anim_path_ease_out);
        lv_anim_set_values(&ax, lv_obj_get_x(to_r), 0);
        if(t == Transition::SlideH) lv_anim_start(&ax);
    }
    {
        lv_anim_t ao;
        lv_anim_init(&ao);
        lv_anim_set_var(&ao, to_r);
        lv_anim_set_exec_cb(&ao, anim_set_opa);
        lv_anim_set_time(&ao, anim_time_ms_);
        lv_anim_set_path_cb(&ao, lv_anim_path_ease_out);
        lv_anim_set_values(&ao, opa0, opa1);
        lv_anim_start(&ao);
    }

    // from: x -> -dx (push) / +dx (pop), opa -> 0, then hide
    {
        lv_anim_t ax;
        lv_anim_init(&ax);
        lv_anim_set_var(&ax, from_r);
        lv_anim_set_exec_cb(&ax, anim_set_x);
        lv_anim_set_time(&ax, anim_time_ms_);
        lv_anim_set_path_cb(&ax, lv_anim_path_ease_out);
        lv_anim_set_values(&ax, lv_obj_get_x(from_r), is_push ? -dx : dx);
        if(t == Transition::SlideH) lv_anim_start(&ax);
    }
    {
        lv_anim_t ao;
        lv_anim_init(&ao);
        lv_anim_set_var(&ao, from_r);
        lv_anim_set_exec_cb(&ao, anim_set_opa);
        lv_anim_set_time(&ao, anim_time_ms_);
        lv_anim_set_path_cb(&ao, lv_anim_path_ease_out);
        lv_anim_set_values(&ao, opa1, opa0);
        lv_anim_set_ready_cb(&ao, hide_ready_cb);
        lv_anim_start(&ao);
    }

    from->on_hide();
    to->on_show();
}

void PageManager::push(IPage* page, Transition t)
{
    if(!parent_ || !page) return;
    IPage* cur = top();
    stack_.push_back(page);
    transition(cur, page, true, t);
}

void PageManager::replace(IPage* page, Transition t)
{
    if(!parent_ || !page) return;
    IPage* cur = top();
    if(!stack_.empty()) stack_.pop_back();
    stack_.push_back(page);
    transition(cur, page, true, t);
}

void PageManager::pop(Transition t)
{
    if(stack_.size() <= 1) return;
    IPage* cur = stack_.back();
    stack_.pop_back();
    IPage* next = top();
    transition(cur, next, false, t);
}

void PageManager::pop_to_root(Transition t)
{
    while(stack_.size() > 1) {
        pop(t);
        // If animation is enabled, repeated pops would stack animations.
        // For now, do it in one jump after the first animated pop.
        t = Transition::None;
    }
}

IPage* PageManager::top() const
{
    if(stack_.empty()) return nullptr;
    return stack_.back();
}

void PageManager::relayout_all()
{
    if(!parent_) return;
    for(IPage* p : stack_) {
        if(!p) continue;
        lv_obj_t* r = p->root();
        if(!r) continue;
        apply_fullscreen(r);
    }
}

void PageManager::update()
{
    if(IPage* cur = top()) cur->on_update();
}

} // namespace vsun::services
