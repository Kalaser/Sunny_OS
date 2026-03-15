#pragma once

#include <array>
#include <memory>

#include "lvgl.h"

#include "apps/ai/ai_slice.h"
#include "apps/camera/camera_slice.h"
#include "apps/health/health_slice.h"
#include "apps/maps/maps_slice.h"
#include "apps/music/music_slice.h"
#include "apps/notes/notes_slice.h"
#include "apps/radio/radio_slice.h"
#include "apps/settings/settings_slice.h"
#include "apps/translate/translate_slice.h"
#include "apps/weather/weather_slice.h"
#include "core/app_entry.h"
#include "core/app_slice.h"
#include "desktop/desktop_slice.h"
#include "launcher/launcher_slice.h"
#include "pages/boot_page.h"
#include "pages/shutdown_page.h"
#include "services/page_manager.h"
#include "services/connectivity_service.h"
#include "services/sensor_service.h"
#include "services/system_status_service.h"
#include "services/ui_publish_queue.h"
#include "sunny_resources.h"

namespace vsun::core {

class AppController {
public:
    AppController() = default;
    ~AppController();
    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

    void init();
    void update();
    void uninit();

private:
    // Concrete page type for the app chrome. Defined in .cpp.
    class AppPageImpl;

    static void on_root_size(lv_event_t* e);

    // Callbacks for slices.
    static void on_open_app(const char* app_id, void* user);
    static void on_open_launcher(void* user);
    static void on_go_desktop(void* user);
    static void on_shutdown(void* user);
    static void on_boot_done(void* user);

    void open_launcher();
    void go_desktop();
    void open_app(const char* app_id);
    void close_app();
    void shutdown();
    void boot_done();

    void relayout();

    AppSlice* find_slice(const char* id);
    const AppEntry* find_entry(const char* id) const;

    lv_obj_t* root_ = nullptr;

    desktop::DesktopSlice desktop_{};
    launcher::LauncherSlice launcher_{};

    apps::maps::MapsSlice maps_{};
    apps::ai::AiSlice ai_{};
    apps::camera::CameraSlice camera_{};
    apps::translate::TranslateSlice translate_{};
    apps::weather::WeatherSlice weather_{};
    apps::music::MusicSlice music_{};
    apps::notes::NotesSlice notes_{};
    apps::health::HealthSlice health_{};
    apps::radio::RadioSlice radio_{};
    apps::settings::SettingsSlice settings_{};

    std::array<AppSlice*, 10> slices_{};

    static constexpr std::array<AppEntry, 10> kEntries{{
        {"maps", "MAPS", SUNNY_IMG(MYNAUI_MAP_SOLID)},
        {"ai", "AI VISION", SUNNY_IMG(MYNAUI_EYE_SOLID)},
        {"camera", "RECORDER", SUNNY_IMG(MYNAUI_CAMERA_SOLID)},
        {"translate", "TRANSLATE", SUNNY_IMG(MYNAUI_REFRESH_SOLID)},
        {"weather", "WEATHER", SUNNY_IMG(MYNAUI_CLOUD_SUN_SOLID)},
        {"music", "MUSIC", SUNNY_IMG(MYNAUI_MUSIC_SOLID)},
        {"notes", "NOTES", SUNNY_IMG(MYNAUI_EDIT_SOLID)},
        {"health", "HEALTH", SUNNY_IMG(MYNAUI_HEART_SOLID)},
        {"radio", "RADIO", SUNNY_IMG(MYNAUI_WIFI_MEDIUM_SOLID)},
        {"settings", "SETTINGS", SUNNY_IMG(MYNAUI_COG_SOLID)},
    }};

    std::unique_ptr<services::PageManager> page_mgr_;
    std::unique_ptr<services::IPage> desktop_page_;
    std::unique_ptr<services::IPage> launcher_page_;
    std::unique_ptr<services::IPage> app_page_;
    AppPageImpl* app_page_impl_ = nullptr;

    std::unique_ptr<pages::BootPage> boot_page_;
    std::unique_ptr<pages::ShutdownPage> shutdown_page_;

    services::SystemStatusService sys_status_{};
    services::ConnectivityService connectivity_{};
    services::SensorService sensors_{};
};

} // namespace vsun::core
