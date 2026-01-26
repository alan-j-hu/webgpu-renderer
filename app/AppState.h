#ifndef APPSTATE_H
#define APPSTATE_H

#include "Deserialize.h"
#include "Export.h"
#include "Project.h"
#include "Serialize.h"
#include "ThumbnailUtil.h"
#include "Commands/Command.h"

#include "noworry/renderer.h"
#include "noworry/Draw2D/SpriteRenderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/scene/scene.h"

#include <stdexcept>
#include <type_traits>
#include <vector>
#include <webgpu/webgpu.h>

/// Helper class containing data used by multiple sub-editors.
class AppState
{
    class CommandHolderBase
    {
    public:
        virtual Command* get() = 0;
        virtual void reset() = 0;
    };

public:
    template <class C>
    class CommandHolder : public CommandHolderBase
    {
    public:
        CommandHolder()
            : m_command(nullptr)
        {
        }

        virtual C* get() override { return m_command; }

        virtual void reset() override
        {
            if (m_command != nullptr) {
                m_command->finish();
            }
            m_command = nullptr;
        }

        void set(C& command)
        {
            m_command = &command;
        }

    private:
        C* m_command;
    };

private:
    class CurrentCommand
    {
    public:
        CurrentCommand();
        CurrentCommand(std::unique_ptr<Command> command,
                       CommandHolderBase* holder);

        Command* get();

        std::unique_ptr<Command> clear();

        template <class C>
        void set(std::unique_ptr<C> command,
                 CommandHolder<C>* holder)
        {
            if (m_command.get() != nullptr) {
                throw std::logic_error("Current command set without clear!");
            }

            if (holder != nullptr) {
                holder->set(*command);
            }
            m_command = std::move(command);
            m_holder = holder;
        }

    private:
        std::unique_ptr<Command> m_command;
        CommandHolderBase* m_holder;
    };

public:
    AppState(WGPUDevice);

    WGPUColor background_color() { return m_background_color; }

    Renderer& renderer() { return m_renderer; }
    SpriteRenderer& sprite_renderer() { return m_sprite_renderer; }
    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }
    BasicMesh& small_grid_mesh() { return m_small_grid_mesh; }

    Exporter& exporter() { return m_exporter; }

    JsonDeserializer& deserializer() { return m_deserializer; }

    JsonSerializer& serializer() { return m_serializer; }

    Project& project() { return m_project; }

    const Project& project() const { return m_project; }

    void set_project(Project);

    template <class C,
              class = std::enable_if_t<std::is_convertible_v<C*, Command*>>>
    void push_command(
        std::unique_ptr<C> command,
        CommandHolder<C>* holder = nullptr)
    {
        if (m_current_command.get() != nullptr) {
            auto ptr = m_current_command.clear();
            m_undo_stack.push_back(std::move(ptr));
        }

        auto outcome = command->first_do(m_project);
        if (outcome != Command::Outcome::UNCHANGED) {
            m_redo_stack.clear();
        }

        if (outcome == Command::Outcome::DONE) {
            m_undo_stack.push_back(std::move(command));
        } else {
            m_current_command.set(std::move(command), holder);
        }
    }

    void update_current_command();

    void finish_current_command();

    void undo();

    void redo();

    ThumbnailUtil& thumbnail_util() { return m_thumbnail_util; }

private:
    std::vector<std::unique_ptr<Command>> m_undo_stack;
    std::vector<std::unique_ptr<Command>> m_redo_stack;
    CurrentCommand m_current_command;

    WGPUColor m_background_color;
    Renderer m_renderer;
    SpriteRenderer m_sprite_renderer;
    ResourceTable m_resources;
    Exporter m_exporter;
    JsonDeserializer m_deserializer;
    JsonSerializer m_serializer;

    Material* m_default_material;
    Material* m_wireframe_material;
    BasicMesh m_small_grid_mesh;

    Project m_project;

    ThumbnailUtil m_thumbnail_util;
};

#endif
