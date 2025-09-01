#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>

/// Cache class for a resource T, holding onto weak_ptr references to its
/// resources.
template<class T>
class ResourceCache
{
public:
    std::optional<std::shared_ptr<T>> find(const std::filesystem::path& path)
    {
        auto it = m_map.find(path);
        if (it != m_map.end()) {
            if (auto shared = it->second.lock()) {
                return shared;
            }
            m_map.erase(it);
        }
        return std::nullopt;
    }

    void store(const std::filesystem::path& path, std::shared_ptr<T> ptr)
    {
        std::weak_ptr<T> handle = ptr;
        m_map.emplace(path, std::move(handle));
    }

private:
    std::unordered_map<std::filesystem::path, std::weak_ptr<T>> m_map;
};

#endif
