#include "services/storage_service.h"

#include <fstream>
#include <sstream>

namespace vsun::services {

StorageService& StorageService::instance()
{
    static StorageService s;
    return s;
}

void StorageService::init(std::string filename)
{
    filename_ = std::move(filename);
}

void StorageService::load()
{
    if(filename_.empty()) return;
    std::ifstream in(filename_, std::ios::in);
    if(!in) return;

    kv_.clear();
    std::string line;
    while(std::getline(in, line)) {
        if(line.empty()) continue;
        if(line[0] == '#') continue;
        const auto eq = line.find('=');
        if(eq == std::string::npos) continue;
        const std::string key = line.substr(0, eq);
        const std::string value = line.substr(eq + 1);
        if(key.empty()) continue;
        kv_[key] = value;
    }
}

void StorageService::save()
{
    if(filename_.empty()) return;
    std::ofstream out(filename_, std::ios::out | std::ios::trunc);
    if(!out) return;

    out << "# SUNNY_OS StorageService (key=value)\n";
    for(const auto& it : kv_) {
        out << it.first << "=" << it.second << "\n";
    }
}

void StorageService::set_u32(const std::string& key, uint32_t v)
{
    kv_[key] = std::to_string(v);
}

bool StorageService::get_u32(const std::string& key, uint32_t& out) const
{
    const auto it = kv_.find(key);
    if(it == kv_.end()) return false;
    try {
        out = (uint32_t)std::stoul(it->second);
        return true;
    } catch(...) {
        return false;
    }
}

void StorageService::set_str(const std::string& key, std::string v)
{
    kv_[key] = std::move(v);
}

bool StorageService::get_str(const std::string& key, std::string& out) const
{
    const auto it = kv_.find(key);
    if(it == kv_.end()) return false;
    out = it->second;
    return true;
}

} // namespace vsun::services

