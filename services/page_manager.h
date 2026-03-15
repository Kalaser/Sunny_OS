#pragma once

#include <cstdint>
#include <vector>

#include "lvgl.h"

namespace vsun::services {

class IPage {
public:
    virtual ~IPage() = default;
    virtual const char* id() const = 0;

    // PageManager is responsible for attaching the page root under `parent`.
    virtual void ensure_built(lv_obj_t* parent) = 0;
    virtual lv_obj_t* root() const = 0;

    // Called on navigation transitions.
    virtual void on_show() {}
    virtual void on_hide() {}

    // Optional: periodic update (if you prefer page-local ticks).
    virtual void on_update() {}
};

class PageManager {
public:
    enum class Transition : std::uint8_t {
        None = 0,
        SlideH,
        Fade,
    };

    explicit PageManager(lv_obj_t* parent);

    void set_parent(lv_obj_t* parent);
    lv_obj_t* parent() const { return parent_; }

    void set_anim_time(std::uint16_t ms) { anim_time_ms_ = ms; }
    std::uint16_t anim_time() const { return anim_time_ms_; }

    void push(IPage* page, Transition t = Transition::SlideH);
    void replace(IPage* page, Transition t = Transition::SlideH);
    void pop(Transition t = Transition::SlideH);
    void pop_to_root(Transition t = Transition::SlideH);

    IPage* top() const;
    std::size_t size() const { return stack_.size(); }

    // Call when the parent size might have changed.
    void relayout_all();

    void update();

private:
    static void anim_set_x(void* var, int32_t v);
    static void anim_set_opa(void* var, int32_t v);
    static void hide_ready_cb(lv_anim_t* a);

    void cancel_anims(lv_obj_t* obj);
    void show_page(IPage* page);
    void hide_page(IPage* page);
    void apply_fullscreen(lv_obj_t* obj);
    void transition(IPage* from, IPage* to, bool is_push, Transition t);

    lv_obj_t* parent_ = nullptr;
    std::vector<IPage*> stack_;
    std::uint16_t anim_time_ms_ = 220;
};

} // namespace vsun::services
