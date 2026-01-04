#ifndef ERROR_MODAL_H
#define ERROR_MODAL_H

#include <string>

class ErrorModal
{
public:
    void open(const char*);

    void update(const char*);

private:
    std::string m_message;
    enum class State { OPEN, CLOSED } m_state;
};

#endif
