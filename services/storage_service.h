#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

// StorageService: lightweight key/value persistence (simulator-friendly).
// Goals:
// - Provide a usable persistence API first; swap backend later (NVS/FlashDB/LittleFS/etc).
// - On RTOS, consider abstracting the backend behind a port layer (e.g. port/storage.h).
namespace vsun::services {

class StorageService {
public:
    static StorageService& instance();

    // Simulator: initialize with a filename (relative or absolute); load/save target this file.
    void init(std::string filename);
    void load();
    void save();

    void set_u32(const std::string& key, uint32_t v);
    bool get_u32(const std::string& key, uint32_t& out) const;

    void set_str(const std::string& key, std::string v);
    bool get_str(const std::string& key, std::string& out) const;

private:
    std::string filename_;
    std::unordered_map<std::string, std::string> kv_;
};

} // namespace vsun::services

