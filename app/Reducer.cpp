#include "Reducer.h"
#include <utility>

TileInst::TileInst(std::shared_ptr<TileDef> def, int z)
    : m_def(std::move(def)), m_z(z)
{
}

bool operator==(const TileInst& lhs, const TileInst& rhs)
{
    return lhs.z() == rhs.z() && lhs.def().get() == rhs.def().get();
}

Layer::Layer()
{
    for (int i = 0; i < 16 * 16; ++i) {
        m_tiles.push_back(std::nullopt);
    }
}

const std::optional<TileInst>& Layer::at(int x, int y) const
{
    return m_tiles.at(y * 16 + x);
}

void Layer::set(int x, int y, std::optional<TileInst> option)
{
    m_tiles[y * 16 + x] = std::move(option);
    m_listenable.notify(&Layer::Listener::notify, *this);
}

std::size_t Project::tiledef_count() const
{
    return m_tile_defs.size();
}

std::shared_ptr<TileDef> Project::tiledef_at(int idx) const
{
    return m_tile_defs.at(idx);
}

void Project::add_tiledef(TileDef tiledef)
{
    m_tile_defs.push_back(std::make_shared<TileDef>(tiledef));
}

std::size_t Project::layer_count() const
{
    return m_layers.size();
}

const Layer& Project::layer_at(int idx) const
{
    return *m_layers.at(idx);
}

Layer& Project::layer_at(int idx)
{
    return *m_layers.at(idx);
}

void Project::remove_tiledef(int idx)
{
    if (idx < 0 || idx >= m_tile_defs.size()) {
        return;
    }
    m_tile_defs.erase(m_tile_defs.begin() + idx);
}

void Project::add_layer()
{
    m_layers.push_back(std::make_unique<Layer>());
    m_add_layer_listenable.notify(
        &Project::Listener::add_layer,
        *m_layers.at(m_layers.size() - 1),
        m_layers.size() - 1);
}

void Project::add_layer(std::unique_ptr<Layer> layer, int idx)
{
    if (idx < 0 || idx > m_layers.size()) {
        return;
    }
    m_layers.insert(m_layers.begin() + idx, std::move(layer));
    m_add_layer_listenable.notify(
        &Project::Listener::add_layer,
        *m_layers.at(idx),
        idx);
}

std::unique_ptr<Layer> Project::remove_layer(int idx)
{
    if (idx < 0 || idx >= m_layers.size()) {
        return nullptr;
    }
    std::unique_ptr<Layer> layer = std::move(m_layers.at(idx));
    m_layers.erase(m_layers.begin() + idx);
    m_add_layer_listenable.notify(&Project::Listener::remove_layer, idx);
    return layer;
}
