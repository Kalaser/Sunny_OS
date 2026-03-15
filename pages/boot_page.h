#pragma once

#include <cstdint>

#include "lvgl.h"

#include "services/page_manager.h"

namespace vsun::pages {

class BootPage final : public services::IPage {
public:
    struct Callbacks {
        void (*done)(void* user) = nullptr;
        void* user = nullptr;
    };

    explicit BootPage(const Callbacks& cb) : cb_(cb) {}

    const char* id() const override { return "boot"; }

    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return root_; }

    void on_show() override;
    void on_hide() override;
    void on_update() override;

private:
    static void anim_set_arc_value(void* var, int32_t v);

    void maybe_finish();
    void update_loading_text(std::uint32_t now_ms);

    Callbacks cb_{};
    lv_obj_t* root_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* subtitle_ = nullptr;
    lv_obj_t* arc_ = nullptr;

    std::uint32_t start_ms_ = 0;
    std::uint32_t last_text_ms_ = 0;
    std::uint8_t dots_ = 0;
    bool finished_ = false;
    bool built_ = false;
};

} // namespace vsun::pages
