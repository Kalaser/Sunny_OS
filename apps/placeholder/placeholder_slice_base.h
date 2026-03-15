#pragma once

#include "core/app_slice.h"
#include "apps/placeholder/placeholder_model.h"
#include "apps/placeholder/placeholder_view.h"

namespace vsun::apps::placeholder {

// 占位 App 的通用 slice：
// - 很多 App 目前还没有独立业务逻辑与 UI，仅需要一个可展示的页面
// - 通过继承 PlaceholderSliceBase，只需要提供 id/label 即可出现在 Launcher 中
class PlaceholderSliceBase : public core::AppSlice {
public:
    void ensure_built(lv_obj_t* parent) override;
    lv_obj_t* root() const override { return view_.root(); }

protected:
    explicit PlaceholderSliceBase(const char* id, const char* label)
        : id_(id), label_(label) {}

    const char* id() const override { return id_; }
    const char* label() const override { return label_; }

private:
    const char* id_;
    const char* label_;
    PlaceholderModel model_{};
    PlaceholderView view_{};
    bool built_ = false;
};

} // namespace vsun::apps::placeholder
