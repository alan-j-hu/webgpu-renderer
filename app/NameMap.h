#ifndef NAMEMAP_H
#define NAMEMAP_H

#include <map>
#include <string>

template <class T>
class NameMap
{
public:
    using iterator = std::map<std::string, T>::const_iterator;

    iterator begin() const { return m_names_in_use.begin(); }
    iterator end() const { return m_names_in_use.end(); }

    std::size_t size() const { return m_names_in_use.size(); }

    bool is_name_in_use(const std::string& name)
    {
        return m_names_in_use.contains(name);
    }

    std::pair<const std::string&, T&> insert(const std::string& name, T data)
    {
        std::string str = name;
        int counter = 1;
        while (is_name_in_use(name)) {
            str = name + std::to_string(counter);
            ++counter;
        }

        auto [it, did_insert] = m_names_in_use.emplace(name, std::move(data));

        return std::pair<const std::string&, T&>(it->first, it->second);
    }

    T* find(const std::string& name)
    {
        auto it = m_names_in_use.find(name);
        if (it == m_names_in_use.end()) {
            return nullptr;
        }
        return &it->second;
    }

    const T* find(const std::string& name) const
    {
        auto it = m_names_in_use.find(name);
        if (it == m_names_in_use.end()) {
            return nullptr;
        }
        return &it->second;
    }

    void erase(const std::string& name)
    {
        m_names_in_use.erase(name);
    }

private:
    std::map<std::string, T> m_names_in_use;
};

#endif
