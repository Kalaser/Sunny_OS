#include "core/app_controller.h"

#include <cstring>

#include "core/theme.h"
#include "config/ui_config.h"
#include "services/resource_pool.h"
#include "services/storage_service.h"
#include "port/system.h"
#include "sunny_resources.h"

namespace vsun::core {
namespace {

static constexpr lv_coord_t kHeaderH = SUNNY_UI_APP_HEADER_H;

} // namespace

AppController::~AppController() = default;

class DesktopPage final : public services::IPage {
public:
    explicit DesktopPage(desktop::DesktopSlice* slice) : slice_(slice) {}
    const char* id() const override { return "desktop"; }

    void ensure_built(lv_obj_t* parent) override
    {
        if(built_ || !slice_) return;
        slice_->build(parent);
        built_ = true;
    }

    lv_obj_t* root() const override
    {
        return slice_ ? slice_->root() : nullptr;
    }

    void on_show() override
    {
        if(slice_) slice_->relayout();
    }

private:
    desktop::DesktopSlice* slice_ = nullptr;
    bool built_ = false;
};

class LauncherPage final : public services::IPage {
public:
    LauncherPage(launcher::LauncherSlice* slice, const AppEntry* entries, std::size_t count)
        : slice_(slice), entries_(entries), count_(count) {}
    const char* id() const override { return "launcher"; }

    void ensure_built(lv_obj_t* parent) override
    {
        if(built_ || !slice_) return;
        slice_->build(parent, entries_, count_);
        built_ = true;
    }

    lv_obj_t* root() const override
    {
        return slice_ ? slice_->root() : nullptr;
    }

    void on_show() override
    {
        if(slice_) slice_->relayout(false);
    }

private:
    launcher::LauncherSlice* slice_ = nullptr;
    const AppEntry* entries_ = nullptr;
    std::size_t count_ = 0;
    bool built_ = false;
};

class AppController::AppPageImpl final : public services::IPage {
public:
    AppPageImpl(AppSlice** slices, std::size_t slice_count, services::PageManager* mgr)
        : slices_(slices), slice_count_(slice_count), mgr_(mgr) {}
    const char* id() const override { return "app"; }

    void ensure_built(lv_obj_t* parent) override
    {
        if(built_ || !parent) return;

        root_ = lv_obj_create(parent);
        lv_obj_remove_style_all(root_);
        lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
        lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(root_, lv_color_black(), 0);
        lv_obj_set_style_border_width(root_, 0, 0);
        lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(root_, LV_OBJ_FLAG_HIDDEN);

        header_ = lv_obj_create(root_);
        lv_obj_remove_style_all(header_);
        lv_obj_set_style_bg_opa(header_, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(header_, 0, 0);
        lv_obj_clear_flag(header_, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(header_, 0, 0);
        lv_obj_set_size(header_, lv_pct(100), kHeaderH);

        lv_obj_t* divider = lv_obj_create(header_);
        lv_obj_remove_style_all(divider);
        lv_obj_set_size(divider, lv_pct(100), 1);
        lv_obj_set_style_bg_opa(divider, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(divider, theme::divider(), 0);
        lv_obj_align(divider, LV_ALIGN_BOTTOM_MID, 0, 0);

        back_btn_ = lv_btn_create(header_);
        lv_obj_set_size(back_btn_, 70, 22);
        lv_obj_align(back_btn_, LV_ALIGN_LEFT_MID, 8, 0);
        lv_obj_set_style_bg_opa(back_btn_, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(back_btn_, 1, 0);
        lv_obj_set_style_border_color(back_btn_, theme::primary(), 0);
        lv_obj_set_style_border_opa(back_btn_, LV_OPA_80, 0);
        lv_obj_add_event_cb(back_btn_, on_back, LV_EVENT_CLICKED, this);

        lv_obj_set_flex_flow(back_btn_, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(back_btn_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_left(back_btn_, 8, 0);
        lv_obj_set_style_pad_right(back_btn_, 8, 0);
        lv_obj_set_style_pad_top(back_btn_, 0, 0);
        lv_obj_set_style_pad_bottom(back_btn_, 0, 0);
        lv_obj_set_style_pad_column(back_btn_, 6, 0);

        lv_obj_t* back_icon = lv_img_create(back_btn_);
        lv_img_set_src(back_icon, SUNNY_IMG(MYNAUI16_ARROW_LEFT_SOLID));
        lv_obj_set_style_img_recolor(back_icon, theme::primary(), 0);
        lv_obj_set_style_img_recolor_opa(back_icon, LV_OPA_COVER, 0);

        lv_obj_t* back_label = lv_label_create(back_btn_);
        lv_label_set_text(back_label, "BACK");
        lv_obj_set_style_text_color(back_label, theme::primary(), 0);

        title_ = lv_label_create(header_);
        lv_label_set_text(title_, "APP");
        lv_obj_set_style_text_color(title_, theme::secondary(), 0);
        lv_obj_align(title_, LV_ALIGN_CENTER, 0, 0);

        content_ = lv_obj_create(root_);
        lv_obj_remove_style_all(content_);
        lv_obj_set_style_bg_opa(content_, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(content_, 0, 0);
        lv_obj_clear_flag(content_, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_pos(content_, 0, kHeaderH);
        lv_obj_set_size(content_, lv_pct(100), lv_pct(100));

        built_ = true;
    }

    lv_obj_t* root() const override { return root_; }

    void set_active(AppSlice* slice, const char* title_text)
    {
        active_ = slice;
        if(title_) lv_label_set_text(title_, title_text ? title_text : "APP");
    }

    void on_show() override
    {
        if(!built_ || !slices_ || slice_count_ == 0) return;
        if(!active_) return;

        // Ensure built and show only active slice.
        active_->ensure_built(content_);
        for(std::size_t i = 0; i < slice_count_; i++) {
            AppSlice* s = slices_[i];
            if(!s || !s->root()) continue;
            if(s == active_) lv_obj_clear_flag(s->root(), LV_OBJ_FLAG_HIDDEN);
            else lv_obj_add_flag(s->root(), LV_OBJ_FLAG_HIDDEN);
        }
        active_->on_show();
        relayout();
    }

    void on_hide() override
    {
        if(active_) active_->on_hide();
    }

    void relayout()
    {
        if(!root_ || !content_ || !header_) return;
        const lv_coord_t w = lv_obj_get_width(root_);
        const lv_coord_t h = lv_obj_get_height(root_);
        lv_obj_set_pos(header_, 0, 0);
        lv_obj_set_size(header_, w, kHeaderH);
        lv_obj_set_pos(content_, 0, kHeaderH);
        lv_obj_set_size(content_, w, (h > kHeaderH) ? (h - kHeaderH) : 0);
    }

private:
    static void on_back(lv_event_t* e)
    {
        auto* self = static_cast<AppPageImpl*>(lv_event_get_user_data(e));
        if(!self || !self->mgr_) return;
        if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;
        self->mgr_->pop(services::PageManager::Transition::SlideH);
    }

    AppSlice** slices_ = nullptr;
    std::size_t slice_count_ = 0;
    services::PageManager* mgr_ = nullptr;

    lv_obj_t* root_ = nullptr;
    lv_obj_t* header_ = nullptr;
    lv_obj_t* back_btn_ = nullptr;
    lv_obj_t* title_ = nullptr;
    lv_obj_t* content_ = nullptr;

    AppSlice* active_ = nullptr;
    bool built_ = false;
};

void AppController::on_root_size(lv_event_t* e)
{
    auto* self = static_cast<AppController*>(lv_event_get_user_data(e));
    if(!self) return;
    if(lv_event_get_code(e) == LV_EVENT_SIZE_CHANGED) self->relayout();
}

void AppController::on_open_app(const char* app_id, void* user)
{
    static_cast<AppController*>(user)->open_app(app_id);
}

void AppController::on_open_launcher(void* user)
{
    static_cast<AppController*>(user)->open_launcher();
}

void AppController::on_go_desktop(void* user)
{
    static_cast<AppController*>(user)->go_desktop();
}

void AppController::on_shutdown(void* user)
{
    static_cast<AppController*>(user)->shutdown();
}

void AppController::on_boot_done(void* user)
{
    static_cast<AppController*>(user)->boot_done();
}

AppSlice* AppController::find_slice(const char* id)
{
    if(!id) return nullptr;
    for(auto* s : slices_) {
        if(s && std::strcmp(s->id(), id) == 0) return s;
    }
    return nullptr;
}

const AppEntry* AppController::find_entry(const char* id) const
{
    if(!id) return nullptr;
    for(const auto& e : kEntries) {
        if(std::strcmp(e.id, id) == 0) return &e;
    }
    return nullptr;
}

void AppController::init()
{
    services::ResourcePool::init();
    services::StorageService::instance().init("sunny_os.kv");
    services::StorageService::instance().load();
    sys_status_.init();
    connectivity_.init();
    sensors_.init();

    lv_obj_t* scr = lv_scr_act();
    theme::apply_screen_base(scr);

    root_ = lv_obj_create(scr);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(root_, on_root_size, LV_EVENT_SIZE_CHANGED, this);

    // Slice registry (apps only; desktop/launcher are pages).
    slices_ = {&maps_, &ai_, &camera_, &translate_, &weather_, &music_, &notes_, &health_, &radio_, &settings_};

    // Desktop callbacks.
    desktop::DesktopSliceCallbacks dcb{};
    dcb.open_launcher = &AppController::on_open_launcher;
    dcb.shutdown = &AppController::on_shutdown;
    dcb.user = this;
    desktop_.set_callbacks(dcb);

    // Launcher callbacks.
    launcher::LauncherSliceCallbacks lcb{};
    lcb.open_app_by_id = &AppController::on_open_app;
    lcb.go_desktop = &AppController::on_go_desktop;
    lcb.user = this;
    launcher_.set_callbacks(lcb);

    // PageManager replaces the old manual navigation mechanism.
    page_mgr_ = std::make_unique<services::PageManager>(root_);
    desktop_page_ = std::make_unique<DesktopPage>(&desktop_);
    launcher_page_ = std::make_unique<LauncherPage>(&launcher_, kEntries.data(), kEntries.size());
    auto app_impl = std::make_unique<AppPageImpl>(slices_.data(), slices_.size(), page_mgr_.get());
    app_page_impl_ = app_impl.get();
    app_page_ = std::move(app_impl);

    pages::BootPage::Callbacks bcb{};
    bcb.done = &AppController::on_boot_done;
    bcb.user = this;
    boot_page_ = std::make_unique<pages::BootPage>(bcb);

    shutdown_page_ = std::make_unique<pages::ShutdownPage>();

    page_mgr_->push(boot_page_.get(), services::PageManager::Transition::None);
    relayout();
}

void AppController::open_launcher()
{
    if(!page_mgr_ || !launcher_page_) return;
    if(page_mgr_->top() == launcher_page_.get()) return;
    page_mgr_->push(launcher_page_.get(), services::PageManager::Transition::SlideH);
}

void AppController::go_desktop()
{
    if(!page_mgr_) return;
    page_mgr_->pop_to_root(services::PageManager::Transition::SlideH);
}

void AppController::boot_done()
{
    if(!page_mgr_ || !desktop_page_) return;
    page_mgr_->replace(desktop_page_.get(), services::PageManager::Transition::Fade);
}

void AppController::open_app(const char* app_id)
{
    if(!page_mgr_ || !app_page_ || !app_page_impl_) return;
    if(!app_id) return;

    AppSlice* slice = find_slice(app_id);
    if(!slice) return;

    const AppEntry* entry = find_entry(app_id);
    app_page_impl_->set_active(slice, entry ? entry->label : app_id);

    if(page_mgr_->top() == app_page_.get()) {
        // Same page instance; just refresh content.
        app_page_->on_show();
        return;
    }

    page_mgr_->push(app_page_.get(), services::PageManager::Transition::SlideH);
}

void AppController::close_app()
{
    if(!page_mgr_) return;
    if(page_mgr_->top() == app_page_.get()) page_mgr_->pop(services::PageManager::Transition::SlideH);
}

void AppController::shutdown()
{
    // Persist state before exiting.
    services::StorageService::instance().save();

    if(!page_mgr_ || !shutdown_page_) {
        vsun::port::request_shutdown();
        return;
    }

    page_mgr_->push(shutdown_page_.get(), services::PageManager::Transition::Fade);
}

void AppController::relayout()
{
    if(!root_ || !page_mgr_) return;

    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_update_layout(root_);

    page_mgr_->relayout_all();
    if(app_page_impl_ && app_page_impl_->root()) app_page_impl_->relayout();
}

void AppController::update()
{
    // Drain any cross-thread publications before updating UI/pages.
    services::UiPublishQueue::instance().drain();
    sys_status_.update(lv_tick_get());
    connectivity_.update(lv_tick_get());
    sensors_.update(lv_tick_get());
    if(page_mgr_) page_mgr_->update();
}

void AppController::uninit()
{
    services::StorageService::instance().save();
    services::ResourcePool::uninit();
}

} // namespace vsun::core
