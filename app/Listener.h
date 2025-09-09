#ifndef LISTENER_H
#define LISTENER_H

#include <algorithm>
#include <vector>

/// The purpose of the listener is to facilitate loose coupling between the
/// project file data structure and the non-essential graphical state.
/// In MVC terms, the former is the model and the latter is the view.
template<class T>
class Listener
{
public:
    virtual void operator()(T) = 0;

private:
};

template<class T>
class Listenable
{
public:
    bool add_listener(Listener<T>& listener)
    {
        auto end = m_listeners.end();
        auto it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
        if (it != end) {
            return false;
        }
        m_listeners.push_back(&listener);
        return true;
    }

    bool remove_listener(Listener<T>& listener)
    {
        auto end = m_listeners.end();
        auto it = std::find(m_listeners.begin(), m_listeners.end(), &listener);
        if (it == end) {
            return false;
        }
        m_listeners.erase(it);
        return true;
    }

    void notify(T t)
    {
        for (auto listener : m_listeners) {
            (*listener)(t);
        }
    }

private:
    std::vector<Listener<T>*> m_listeners;
};

#endif
