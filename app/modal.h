#ifndef MODAL_H
#define MODAL_H

#include <memory>
#include <string>
#include <string_view>
#include <vector>

class Modal
{
public:
    Modal(std::string_view);

    const std::string& title() { return m_title; }

    virtual void render();

    virtual void on_submit();

    virtual void on_cancel();

private:
    std::string m_title;
};

class ModalStack
{
public:
    bool has_focus() { return m_modals.size() != 0; }

    void push(std::unique_ptr<Modal>);

    void render();

private:
    std::vector<std::unique_ptr<Modal>> m_modals;

    bool render_modal(Modal&);
};

#endif
