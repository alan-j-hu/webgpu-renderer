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

#include <optional>
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
        virtual bool needs_update() = 0;
        virtual void set_needs_update(bool) = 0;
    };

    struct Snapshot
    {
        std::optional<int> selected_tiledef_idx;
        glm::ivec2 selected_level_idx;
        std::optional<int> selected_layer_idx;
        std::unique_ptr<Command> command;
    };

public:
    template <class C>
    class CommandHolder : public CommandHolderBase
    {
    public:
        CommandHolder() : m_command(nullptr), m_needs_update(false)
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

        bool needs_update() override
        {
            return m_needs_update;
        }

        void set_needs_update(bool b) override
        {
            m_needs_update = b;
        }

    private:
        C* m_command;
        bool m_needs_update;
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

    std::optional<int> selected_tiledef_idx() const;
    std::optional<std::shared_ptr<TileDef>> selected_tiledef();
    void select_tiledef(std::optional<int>);

    Level* selected_level();
    void select_level(glm::ivec2);

    std::optional<int> selected_layer_idx() const;
    Layer* selected_layer();
    void select_layer(std::optional<int>);

    template <class C,
              class = std::enable_if_t<std::is_convertible_v<C*, Command*>>>
    void push_command(
        std::unique_ptr<C> command,
        CommandHolder<C>* holder = nullptr)
    {
        if (m_command_holder != nullptr) {
            m_command_holder->reset();
        }

        if (holder != nullptr) {
            holder->set(*command);
        }

        m_command_queue.push_back(std::move(command));
        m_command_holder = holder;
    }

    void finish_long_command();

    void run_pending_commands();

    void undo();

    void redo();

    std::optional<std::string> check_error();

    ThumbnailUtil& thumbnail_util() { return m_thumbnail_util; }

private:
    std::vector<Snapshot> m_undo_stack;
    std::vector<Snapshot> m_redo_stack;
    std::vector<std::unique_ptr<Command>> m_command_queue;
    CommandHolderBase* m_command_holder;
    std::optional<std::string> m_error;

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
    std::optional<int> m_selected_tiledef_idx;
    glm::ivec2 m_selected_level_idx;
    std::optional<int> m_selected_layer_idx;

    ThumbnailUtil m_thumbnail_util;

    void update_long_command();
};

#endif
