cmake_minimum_required(VERSION 3.10)
project(SunnyOsSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Compile SUNNY_OS with a Win32 host.
# Only LVGL itself is taken from Wand (source-level dependency).
set(LVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../Sunny_OS/libs/lvgl")
set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(THIRD_PARTY_DIR "${APP_DIR}/third_party")
set(RES_DIR "${APP_DIR}/resources")
set(RES_GEN_DIR "${RES_DIR}/generated")

include_directories(
    ${LVGL_DIR}
    ${APP_DIR}
    ${THIRD_PARTY_DIR}
    ${THIRD_PARTY_DIR}/lv_fs_if
    ${RES_GEN_DIR}
)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DLV_CONF_INCLUDE_SIMPLE)
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-D_WIN32_WINNT=0x0A00)
add_definitions(-DSUNNY_CONNECTIVITY_USE_MOCK=1)

# 1. SUNNY_OS Win32 host (entry point).
set(HOST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/host_win32/sunny_os_host_win32.cpp"
)

# 2. LVGL core + drivers
file(GLOB_RECURSE LVGL_SOURCES "${LVGL_DIR}/src/*.c")
set(LV_DRIVERS_SOURCES "${THIRD_PARTY_DIR}/lv_drivers/win32drv/win32drv.c")
file(GLOB LV_FS_SOURCES "${THIRD_PARTY_DIR}/lv_fs_if/*.c")

# 3. SUNNY_OS sources
file(GLOB_RECURSE SUNNY_SOURCES
    "${APP_DIR}/apps/*.cpp"
    "${APP_DIR}/core/*.cpp"
    "${APP_DIR}/desktop/*.cpp"
    "${APP_DIR}/launcher/*.cpp"
    "${APP_DIR}/pages/*.cpp"
    "${APP_DIR}/runtime/vsun_app.cpp"
    "${APP_DIR}/runtime/vsun_layout.cpp"
    "${APP_DIR}/port/time_std.cpp"
    "${APP_DIR}/port/system_win32.cpp"
    "${APP_DIR}/sunny_hal/hal_sim.cpp"
    "${APP_DIR}/services/datacenter.cpp"
    "${APP_DIR}/services/connectivity_service.cpp"
    "${APP_DIR}/services/system_status_service.cpp"
    "${APP_DIR}/services/sensor_service.cpp"
    "${APP_DIR}/services/storage_service.cpp"
    "${APP_DIR}/services/resource_pool.cpp"
    "${APP_DIR}/services/page_manager.cpp"
    "${APP_DIR}/services/ui_publish_queue.cpp"
    "${RES_GEN_DIR}/sunny_resources.c"
)

# 3.5 Resources generation (images/fonts/strings -> C + headers + optional bins)
file(GLOB_RECURSE ASSET_INPUTS
    "${RES_DIR}/manifest.json"
    "${RES_DIR}/input/*"
    "${APP_DIR}/tools/assets/gen_assets.py"
)

add_custom_command(
    OUTPUT "${RES_GEN_DIR}/sunny_resources.c" "${RES_GEN_DIR}/sunny_resources.h"
    COMMAND python "${APP_DIR}/tools/assets/gen_assets.py"
    DEPENDS ${ASSET_INPUTS}
    WORKING_DIRECTORY "${APP_DIR}"
    VERBATIM
)
add_custom_target(sunny_assets DEPENDS "${RES_GEN_DIR}/sunny_resources.c" "${RES_GEN_DIR}/sunny_resources.h")

set(ALL_SOURCES
    ${HOST_SOURCES}
    ${LVGL_SOURCES}
    ${LV_DRIVERS_SOURCES}
    ${LV_FS_SOURCES}
    ${SUNNY_SOURCES}
)

add_executable(SunnyOsSimulator ${ALL_SOURCES})
add_dependencies(SunnyOsSimulator sunny_assets)

if(WIN32)
    target_link_libraries(SunnyOsSimulator gdi32 user32 kernel32 comctl32 winmm)
    # Uncomment the following line to hide the console window.
    # target_link_options(SunnyOsSimulator PRIVATE -mwindows)
endif()
