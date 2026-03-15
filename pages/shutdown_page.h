#pragma once

#include <cstdint>

#include "lvgl.h"

#include "services/page_manager.h"

namespace vsun::pages {

class ShutdownPage final : public services::IPage {
public:
    const char* id() const override { return "shutdown"; }

    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return root_; }

    void on_show() override;
    void on_hide() override;
    void on_update() override;

private:
    static void anim_set_arc_value(void* var, int32_t v);
    static void anim_set_root_opa(void* var, int32_t v);
    void update_loading_text(std::uint32_t now_ms);

    lv_obj_t* root_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* subtitle_ = nullptr;
    lv_obj_t* arc_ = nullptr;

    std::uint32_t start_ms_ = 0;
    std::uint32_t last_text_ms_ = 0;
    std::uint32_t shutdown_ms_ = 0;
    std::uint8_t dots_ = 0;
    bool requested_ = false;
    bool built_ = false;
};

} // namespace vsun::pages
