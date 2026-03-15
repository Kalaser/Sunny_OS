#include "apps/camera/camera_view.h"

#include "core/theme.h"

namespace vsun::apps::camera {
namespace {

struct LayoutCtx {
    lv_obj_t* frame = nullptr;
    lv_obj_t* h_line = nullptr;
    lv_obj_t* v_line = nullptr;

    lv_point_t* hpts = nullptr;
    lv_point_t* vpts = nullptr;

    bool owns_points = false;
};

static void apply_layout(LayoutCtx* ctx, lv_obj_t* root)
{
    if(!ctx || !root) return;

    const lv_coord_t w = lv_obj_get_width(root);
    const lv_coord_t h = lv_obj_get_height(root);

    if(ctx->frame) {
        lv_obj_set_pos(ctx->frame, 2, 2);
        lv_obj_set_size(ctx->frame, (w > 4) ? (w - 4) : 0, (h > 4) ? (h - 4) : 0);
    }

    const lv_coord_t cx = w / 2;
    const lv_coord_t cy = h / 2;
    const lv_coord_t margin = 20;

    lv_coord_t max_half_x = LV_MIN(cx, (lv_coord_t)(w - cx));
    lv_coord_t max_half_y = LV_MIN(cy, (lv_coord_t)(h - cy));
    lv_coord_t max_half = LV_MIN(max_half_x, max_half_y);
    if(max_half > margin) max_half = (lv_coord_t)(max_half - margin);
    else max_half = 0;

    lv_coord_t half = (lv_coord_t)(LV_MIN(w, h) / 3);
    if(half > max_half) half = max_half;

    if(ctx->hpts && ctx->vpts) {
        ctx->hpts[0].x = (lv_coord_t)(cx - half);
        ctx->hpts[0].y = (lv_coord_t)cy;
        ctx->hpts[1].x = (lv_coord_t)(cx + half);
        ctx->hpts[1].y = (lv_coord_t)cy;

        ctx->vpts[0].x = (lv_coord_t)cx;
        ctx->vpts[0].y = (lv_coord_t)(cy - half);
        ctx->vpts[1].x = (lv_coord_t)cx;
        ctx->vpts[1].y = (lv_coord_t)(cy + half);

        if(ctx->h_line) lv_line_set_points(ctx->h_line, ctx->hpts, 2);
        if(ctx->v_line) lv_line_set_points(ctx->v_line, ctx->vpts, 2);
    }
}

static void on_root_event(lv_event_t* e)
{
    auto* ctx = static_cast<LayoutCtx*>(lv_event_get_user_data(e));
    lv_obj_t* root = static_cast<lv_obj_t*>(lv_event_get_current_target(e));

    switch(lv_event_get_code(e)) {
    case LV_EVENT_SIZE_CHANGED:
        apply_layout(ctx, root);
        break;
    case LV_EVENT_DELETE:
        if(ctx) {
            if(ctx->owns_points) {
                if(ctx->hpts) lv_mem_free(ctx->hpts);
                if(ctx->vpts) lv_mem_free(ctx->vpts);
            }
            lv_mem_free(ctx);
        }
        break;
    default:
        break;
    }
}

} // namespace

void CameraView::build(lv_obj_t* parent, lv_event_cb_t on_close, void* close_user)
{
    root_ = lv_obj_create(parent);
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(root_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(root_, 0, 0);
    lv_obj_clear_flag(root_, LV_OBJ_FLAG_SCROLLABLE);

    auto* ctx = static_cast<LayoutCtx*>(lv_mem_alloc(sizeof(LayoutCtx)));
    if(ctx) {
        *ctx = LayoutCtx{};
        static lv_point_t s_hpts[2] = {{0, 0}, {0, 0}};
        static lv_point_t s_vpts[2] = {{0, 0}, {0, 0}};

        ctx->owns_points = true;
        ctx->hpts = static_cast<lv_point_t*>(lv_mem_alloc(sizeof(lv_point_t) * 2));
        ctx->vpts = static_cast<lv_point_t*>(lv_mem_alloc(sizeof(lv_point_t) * 2));
        if(!ctx->hpts || !ctx->vpts) {
            if(ctx->hpts) lv_mem_free(ctx->hpts);
            if(ctx->vpts) lv_mem_free(ctx->vpts);
            ctx->hpts = s_hpts;
            ctx->vpts = s_vpts;
            ctx->owns_points = false;
        }
        lv_obj_add_event_cb(root_, on_root_event, LV_EVENT_SIZE_CHANGED, ctx);
        lv_obj_add_event_cb(root_, on_root_event, LV_EVENT_DELETE, ctx);
    }

    lv_obj_t* frame = lv_obj_create(root_);
    lv_obj_remove_style_all(frame);
    lv_obj_set_style_bg_opa(frame, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(frame, 2, 0);
    lv_obj_set_style_border_color(frame, core::theme::primary(), 0);
    lv_obj_set_style_border_opa(frame, LV_OPA_20, 0);
    lv_obj_set_style_radius(frame, 16, 0);
    if(ctx) ctx->frame = frame;

    lv_obj_t* rec = lv_label_create(root_);
    lv_label_set_text(rec, "REC 00:04:21");
    lv_obj_set_style_text_color(rec, core::theme::primary(), 0);
    lv_obj_set_style_text_opa(rec, LV_OPA_90, 0);
    lv_obj_align(rec, LV_ALIGN_TOP_LEFT, 20, 10);

    lv_obj_t* dot = lv_obj_create(root_);
    lv_obj_remove_style_all(dot);
    lv_obj_set_size(dot, 8, 8);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(0xFF0000), 0);
    lv_obj_align(dot, LV_ALIGN_TOP_LEFT, 10, 12);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, dot);
    lv_anim_set_exec_cb(&a, [](void* var, int32_t v) {
        lv_obj_set_style_opa(static_cast<lv_obj_t*>(var), (lv_opa_t)v, 0);
    });
    lv_anim_set_values(&a, 50, 255);
    lv_anim_set_time(&a, 500);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    lv_obj_t* fmt = lv_label_create(root_);
    lv_label_set_text(fmt, "4K 60FPS");
    lv_obj_set_style_text_color(fmt, core::theme::primary(), 0);
    lv_obj_set_style_text_opa(fmt, LV_OPA_90, 0);
    lv_obj_align(fmt, LV_ALIGN_BOTTOM_RIGHT, -10, -10);

    lv_obj_t* h = lv_line_create(root_);
    lv_obj_set_style_line_width(h, 2, 0);
    lv_obj_set_style_line_color(h, core::theme::primary(), 0);
    lv_obj_set_style_line_opa(h, LV_OPA_10, 0);

    lv_obj_t* v = lv_line_create(root_);
    lv_obj_set_style_line_width(v, 2, 0);
    lv_obj_set_style_line_color(v, core::theme::primary(), 0);
    lv_obj_set_style_line_opa(v, LV_OPA_10, 0);

    if(ctx) {
        ctx->h_line = h;
        ctx->v_line = v;
    }

    lv_obj_t* close_btn = lv_btn_create(root_);
    lv_obj_set_size(close_btn, 88, 22);
    lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(close_btn, core::theme::primary(), 0);
    lv_obj_set_style_radius(close_btn, 12, 0);
    if(on_close) lv_obj_add_event_cb(close_btn, on_close, LV_EVENT_CLICKED, close_user);

    lv_obj_t* close_txt = lv_label_create(close_btn);
    lv_label_set_text(close_txt, "CLOSE");
    lv_obj_set_style_text_color(close_txt, lv_color_black(), 0);
    lv_obj_center(close_txt);

    // Initial layout (real size will be applied again on LV_EVENT_SIZE_CHANGED)
    if(ctx) apply_layout(ctx, root_);
}

} // namespace vsun::apps::camera





