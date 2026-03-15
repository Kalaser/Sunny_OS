#pragma once

#include "core/app_slice.h"
#include "apps/ai/ai_model.h"
#include "apps/ai/ai_view.h"

namespace vsun::apps::ai {

class AiSlice final : public core::AppSlice {
public:
    const char* id() const override { return "ai"; }
    const char* label() const override { return "AI VISION"; }

    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return view_.root(); }

private:
    AiModel model_{};
    AiView view_{};
    bool built_ = false;
};

} // namespace vsun::apps::ai
