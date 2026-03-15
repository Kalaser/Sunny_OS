#include "desktop/desktop_view.h"

#include <cstdlib>

#include "config/ui_config.h"
#include "core/theme.h"
#include "sunny_resources.h"

namespace vsun::desktop {
namespace {

static constexpr std::int32_t kFp = 256; // 8.8 fixed-point

static inline std::int32_t fp_from_px(lv_coord_t v) { return (std::int32_t)v * kFp; }
static inline lv_coord_t px_from_fp(std::int32_t v) { return (lv_coord_t)(v / kFp); }
static inline int iabs(int v) { return v < 0 ? -v : v; }

static void clamp_particle_to_bounds(DesktopParticle& p, lv_coord_t x0, lv_coord_t y0, lv_coord_t x1, lv_coord_t y1)
{
    const std::int32_t min_cx = fp_from_px(x0 + p.r);
    const std::int32_t max_cx = fp_from_px(x1 - p.r);
    const std::int32_t min_cy = fp_from_px(y0 + p.r);
    const std::int32_t max_cy = fp_from_px(y1 - p.r);

    if(p.cx < min_cx) p.cx = min_cx;
    if(p.cx > max_cx) p.cx = max_cx;
    if(p.cy < min_cy) p.cy = min_cy;
    if(p.cy > max_cy) p.cy = max_cy;
}

static void move_particle(DesktopParticle& p, lv_coord_t x0, lv_coord_t y0, lv_coord_t x1, lv_coord_t y1)
{
    const std::int32_t min_cx = fp_from_px(x0 + p.r);
    const std::int32_t max_cx = fp_from_px(x1 - p.r);
    const std::int32_t min_cy = fp_from_px(y0 + p.r);
    const std::int32_t max_cy = fp_from_px(y1 - p.r);

    p.cx += p.vx;
    p.cy += p.vy;

    if(p.cx < min_cx) {
        p.cx = min_cx;
        p.vx = -p.vx;
    } else if(p.cx > max_cx) {
        p.cx = max_cx;
        p.vx = -p.vx;
    }

    if(p.cy < min_cy) {
        p.cy = min_cy;
        p.vy = -p.vy;
    } else if(p.cy > max_cy) {
        p.cy = max_cy;
        p.vy = -p.vy;
    }
}

static void resolve_pair_collision(DesktopParticle& a, DesktopParticle& b)
{
    const int ax = (int)px_from_fp(a.cx);
    const int ay = (int)px_from_fp(a.cy);
    const int bx = (int)px_from_fp(b.cx);
    const int by = (int)px_from_fp(b.cy);
    const int dx = bx - ax;
    const int dy = by - ay;
    const int rr = (int)(a.r + b.r);
    const int dist2 = dx * dx + dy * dy;
    const int rr2 = rr * rr;
    if(dist2 >= rr2) return;

    // Simple, cheap collision response:
    // - reflect velocities on the dominant axis
    // - separate by 1px to avoid sticking
    if(iabs(dx) >= iabs(dy)) {
        a.vx = -a.vx;
        b.vx = -b.vx;
        const std::int32_t sep = (dx >= 0) ? kFp : -kFp;
        a.cx -= sep;
        b.cx += sep;
    } else {
        a.vy = -a.vy;
        b.vy = -b.vy;
        const std::int32_t sep = (dy >= 0) ? kFp : -kFp;
        a.cy -= sep;
        b.cy += sep;
    }
}

static void apply_particle_style(lv_obj_t* obj, bool primary)
{
    if(!obj) return;
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, primary ? core::theme::primary() : core::theme::divider(), 0);
    lv_obj_set_style_bg_opa(obj, (lv_opa_t)(primary ? 22 : 18), 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static std::int32_t rand_vel()
{
    // Around 0.7..1.4 px per tick (16ms), fixed-point.
    const int mag = (int)lv_rand(180, 360);
    const int sign = (lv_rand(0, 1) == 0) ? -1 : 1;
    return (std::int32_t)(sign * mag);
}

} // namespace

void DesktopView::on_root_event(lv_event_t* e)
{
    if(!e) return;
    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;
    auto* self = static_cast<DesktopView*>(lv_event_get_user_data(e));
    if(!self) return;

#if SUNNY_UI_DESKTOP_BG_PARTICLES_ENABLE
    if(self->bg_timer_) {
        lv_timer_del(self->bg_timer_);
        self->bg_timer_ = nullptr;
    }
#endif
}

void DesktopView::on_bg_timer(lv_timer_t* t)
{
    if(!t) return;
    auto* self = static_cast<DesktopView*>(t->user_data);
    if(!self || !self->root_ || !self->bg_) return;

    if(lv_obj_has_flag(self->root_, LV_OBJ_FLAG_HIDDEN)) return;
    if(!self->particles_inited_) return;

    const lv_coord_t x0 = self->bounds_x0_;
    const lv_coord_t y0 = self->bounds_y0_;
    const lv_coord_t x1 = self->bounds_x1_;
    const lv_coord_t y1 = self->bounds_y1_;
    if(x1 <= x0 || y1 <= y0) return;

    // Move and bounce.
    for(auto& p : self->particles_) {
        if(!p.obj) continue;
        move_particle(p, x0, y0, x1, y1);
    }

    // Pairwise collisions.
    for(std::size_t i = 0; i < self->particles_.size(); i++) {
        for(std::size_t j = i + 1; j < self->particles_.size(); j++) {
            auto& a = self->particles_[i];
            auto& b = self->particles_[j];
            if(!a.obj || !b.obj) continue;
            resolve_pair_collision(a, b);
        }
    }

    // Clamp after collision separation and apply positions.
    for(auto& p : self->particles_) {
        if(!p.obj) continue;
        clamp_particle_to_bounds(p, x0, y0, x1, y1);
        const lv_coord_t cx = px_from_fp(p.cx);
        const lv_coord_t cy = px_from_fp(p.cy);
        lv_obj_set_pos(p.obj, cx - p.r, cy - p.r);
    }
}

void DesktopView::build(lv_obj_t* parent)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    bg_ = lv_obj_create(root_);
    lv_obj_remove_style_all(bg_);
    lv_obj_set_size(bg_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(bg_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bg_, 0, 0);
    lv_obj_clear_flag(bg_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_move_background(bg_);

#if SUNNY_UI_DESKTOP_BG_PARTICLES_ENABLE
    // Timer runs even when hidden; it checks LV_OBJ_FLAG_HIDDEN and returns early.
    bg_timer_ = lv_timer_create(on_bg_timer, SUNNY_UI_DESKTOP_BG_PARTICLES_PERIOD_MS, this);
#endif

    lv_obj_add_event_cb(root_, on_root_event, LV_EVENT_DELETE, this);

    status_bar_.build(root_);

    content_host_ = lv_obj_create(root_);
    lv_obj_remove_style_all(content_host_);
    lv_obj_set_style_bg_opa(content_host_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content_host_, 0, 0);
    lv_obj_clear_flag(content_host_, LV_OBJ_FLAG_SCROLLABLE);

    title_ = lv_label_create(content_host_);
    lv_label_set_text(title_, "Sunny DESKTOP");
    lv_obj_set_style_text_color(title_, core::theme::secondary(), 0);
    lv_obj_set_style_text_letter_space(title_, 3, 0);
    lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, SUNNY_UI_DESKTOP_TITLE_Y);

    power_btn_ = lv_btn_create(content_host_);
    lv_obj_set_size(power_btn_, SUNNY_UI_DESKTOP_POWER_BTN_PX, SUNNY_UI_DESKTOP_POWER_BTN_PX);
    lv_obj_set_style_bg_opa(power_btn_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(power_btn_, 2, 0);
    lv_obj_set_style_border_color(power_btn_, core::theme::primary(), 0);
    lv_obj_set_style_border_opa(power_btn_, LV_OPA_80, 0);
    lv_obj_set_style_radius(power_btn_, SUNNY_UI_DESKTOP_POWER_BTN_RADIUS, 0);

    power_icon_ = lv_img_create(power_btn_);
    lv_img_set_src(power_icon_, SUNNY_IMG(MYNAUI_POWER_SOLID));
    lv_obj_set_style_img_recolor(power_icon_, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(power_icon_, LV_OPA_COVER, 0);
    lv_obj_center(power_icon_);

    launcher_btn_ = lv_btn_create(content_host_);
    lv_obj_set_size(launcher_btn_, SUNNY_UI_DESKTOP_BUTTON_W, SUNNY_UI_DESKTOP_BUTTON_H);
    lv_obj_set_style_bg_opa(launcher_btn_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(launcher_btn_, 2, 0);
    lv_obj_set_style_border_color(launcher_btn_, core::theme::primary(), 0);
    lv_obj_set_style_border_opa(launcher_btn_, LV_OPA_70, 0);
    lv_obj_set_style_radius(launcher_btn_, SUNNY_UI_DESKTOP_BUTTON_RADIUS, 0);

    launcher_icon_ = lv_img_create(launcher_btn_);
    lv_img_set_src(launcher_icon_, SUNNY_IMG(MYNAUI_MENU_SOLID));
    lv_obj_set_style_img_recolor(launcher_icon_, core::theme::primary(), 0);
    lv_obj_set_style_img_recolor_opa(launcher_icon_, LV_OPA_COVER, 0);
    lv_obj_align(launcher_icon_, LV_ALIGN_LEFT_MID, 16, 0);

    launcher_label_ = lv_label_create(launcher_btn_);
    lv_label_set_text(launcher_label_, "AIGLASSES");
    lv_obj_set_style_text_color(launcher_label_, core::theme::primary(), 0);
    lv_obj_set_style_text_letter_space(launcher_label_, 2, 0);
    lv_obj_align(launcher_label_, LV_ALIGN_LEFT_MID, 46, 0);

    hint_root_ = lv_obj_create(root_);
    lv_obj_remove_style_all(hint_root_);
    lv_obj_set_size(hint_root_, lv_pct(100), SUNNY_UI_HINT_BAR_H);
    lv_obj_set_style_bg_opa(hint_root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hint_root_, 0, 0);
    lv_obj_align(hint_root_, LV_ALIGN_BOTTOM_MID, 0, 0);

    hint_label_ = lv_label_create(hint_root_);
    lv_label_set_text(hint_label_, "Tap LAUNCHER to enter  (Swipe down in Launcher to return)");
    lv_obj_set_style_text_color(hint_label_, core::theme::hint(), 0);
    lv_obj_set_style_text_opa(hint_label_, LV_OPA_80, 0);
    lv_obj_center(hint_label_);

    relayout();
}

void DesktopView::relayout()
{
    if(!root_) return;

    const lv_coord_t w = lv_obj_get_width(root_);
    const lv_coord_t h = lv_obj_get_height(root_);

    if(bg_) {
        lv_obj_set_pos(bg_, 0, 0);
        lv_obj_set_size(bg_, w, h);
        lv_obj_move_background(bg_);
    }

    // Particle bounds: keep them away from the status/hint bars.
    bounds_x0_ = 8;
    bounds_x1_ = (w > 8) ? (w - 8) : 0;
    bounds_y0_ = SUNNY_UI_STATUS_BAR_H + 6;
    bounds_y1_ = (h > (SUNNY_UI_HINT_BAR_H + 6)) ? (h - SUNNY_UI_HINT_BAR_H - 6) : 0;

#if SUNNY_UI_DESKTOP_BG_PARTICLES_ENABLE
    // Init particles on first layout or after resize.
    if(!particles_inited_ && w > 0 && h > 0 && bg_) {
        for(std::size_t i = 0; i < particles_.size(); i++) {
            auto& p = particles_[i];
            p.r = (lv_coord_t)lv_rand(3, 7);
            p.obj = lv_obj_create(bg_);
            apply_particle_style(p.obj, (i % 3) != 0);
            lv_obj_set_size(p.obj, (lv_coord_t)(p.r * 2), (lv_coord_t)(p.r * 2));

            const lv_coord_t minx = bounds_x0_ + p.r;
            const lv_coord_t maxx = (bounds_x1_ > p.r) ? (bounds_x1_ - p.r) : minx;
            const lv_coord_t miny = bounds_y0_ + p.r;
            const lv_coord_t maxy = (bounds_y1_ > p.r) ? (bounds_y1_ - p.r) : miny;

            const lv_coord_t cx = (lv_coord_t)lv_rand((uint32_t)minx, (uint32_t)maxx);
            const lv_coord_t cy = (lv_coord_t)lv_rand((uint32_t)miny, (uint32_t)maxy);
            p.cx = fp_from_px(cx);
            p.cy = fp_from_px(cy);
            p.vx = rand_vel();
            p.vy = rand_vel();
            lv_obj_set_pos(p.obj, cx - p.r, cy - p.r);
        }
        particles_inited_ = true;
    }

    // On resize, nudge particles back into bounds to avoid "lost" particles.
    if((w != last_bg_w_ || h != last_bg_h_) && particles_inited_) {
        for(auto& p : particles_) {
            if(!p.obj) continue;
            clamp_particle_to_bounds(p, bounds_x0_, bounds_y0_, bounds_x1_, bounds_y1_);
        }
    }
#endif

    last_bg_w_ = w;
    last_bg_h_ = h;

    const lv_coord_t top = SUNNY_UI_STATUS_BAR_H;
    const lv_coord_t bottom = SUNNY_UI_HINT_BAR_H;
    const lv_coord_t ch = (h > (top + bottom)) ? (h - top - bottom) : 0;

    if(content_host_) {
        lv_obj_set_pos(content_host_, 0, top);
        lv_obj_set_size(content_host_, w, ch);
    }

    // Keep the button comfortably sized on small windows
    if(launcher_btn_) {
        const lv_coord_t bw = LV_MIN((lv_coord_t)SUNNY_UI_DESKTOP_BUTTON_W, (lv_coord_t)(w - 24));
        const lv_coord_t bh = SUNNY_UI_DESKTOP_BUTTON_H;
        lv_obj_set_size(launcher_btn_, (bw > 0) ? bw : 0, bh);
        lv_obj_align(launcher_btn_, LV_ALIGN_CENTER, 0, SUNNY_UI_DESKTOP_LAUNCHER_BTN_Y + SUNNY_UI_DESKTOP_GROUP_DY);
    }

    // Title: place it above the AIGLASSES button
    if(title_) {
        if(launcher_btn_) {
            lv_obj_align_to(title_, launcher_btn_, LV_ALIGN_OUT_TOP_MID, 0, SUNNY_UI_DESKTOP_TITLE_TO_LAUNCHER_DY);
        } else {
            lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, SUNNY_UI_DESKTOP_TITLE_Y);
        }
    }

    if(power_btn_) {
        const lv_coord_t p = LV_MIN((lv_coord_t)SUNNY_UI_DESKTOP_POWER_BTN_PX, (lv_coord_t)(LV_MIN(w, ch) - 28));
        lv_obj_set_size(power_btn_, (p > 0) ? p : 0, (p > 0) ? p : 0);
        // Keep it round if the size is overridden dynamically.
        lv_obj_set_style_radius(power_btn_, (p > 0) ? (p / 2) : 0, 0);

        // Place the power button centered below the AIGLASSES button
        if(launcher_btn_) {
            lv_obj_align_to(power_btn_, launcher_btn_, LV_ALIGN_OUT_BOTTOM_MID, 0, SUNNY_UI_DESKTOP_POWER_TO_LAUNCHER_DY);
        } else {
            lv_obj_align(power_btn_, LV_ALIGN_CENTER, 0, SUNNY_UI_DESKTOP_POWER_BTN_Y);
        }

        // If it would overlap the hint bar, move it above the launcher area.
        const lv_coord_t py = lv_obj_get_y(power_btn_);
        if(py + lv_obj_get_height(power_btn_) > ch - 4) {
            if(launcher_btn_) {
                lv_obj_align_to(power_btn_, launcher_btn_, LV_ALIGN_OUT_TOP_MID, 0, -SUNNY_UI_DESKTOP_POWER_SPACING_Y);
            } else {
                lv_obj_align(power_btn_, LV_ALIGN_CENTER, 0, SUNNY_UI_DESKTOP_POWER_BTN_Y);
            }
        }
    }
}

} // namespace vsun::desktop
