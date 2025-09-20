#ifndef LISTENER_H
#define LISTENER_H

#include <algorithm>
#include <utility>
#include <vector>

template<class Listener>
class Listenable
{
public:
    bool add_listener(Listener& listener)
    {
        auto end = m_listeners.end();
        auto it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
        if (it != end) {
            return false;
        }
        m_listeners.push_back(&listener);
        return true;
    }

    bool remove_listener(Listener& listener)
    {
        auto end = m_listeners.end();
        auto it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
        if (it == end) {
            return false;
        }
        m_listeners.erase(it);
        return true;
    }

    template<class ...Args1, class ...Args2>
    void notify(void (Listener::*member)(Args1...), Args2&&... args)
    {
        for (auto listener : m_listeners) {
            (listener->*member)(args...);
        }
    }

private:
    std::vector<Listener*> m_listeners;
};

#endif
