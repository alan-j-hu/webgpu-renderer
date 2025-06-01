#ifndef MODAL_H
#define MODAL_H

#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Enum returned by Modal::render to indicate whether to close the modal or
/// keep it open.
enum class ModalResponse
{
    KeepOpen,
    Close
};

/// A modal is a dialog that blocks interaction with the rest of the UI.
class Modal
{
public:
    Modal(std::string_view);
    virtual ~Modal() = default;

    const std::string& title() { return m_title; }

    virtual ModalResponse render() = 0;

private:
    std::string m_title;
};

/// Renders a stack of modals from oldest to newest. The application should
/// keep one ModalStack and push modals to the stack.
class ModalStack
{
public:
    bool has_focus() { return m_modals.size() != 0; }

    void push(std::unique_ptr<Modal>);

    void render();

private:
    std::vector<std::unique_ptr<Modal>> m_modals;

    ModalResponse render_modal(Modal&);
};

#endif
