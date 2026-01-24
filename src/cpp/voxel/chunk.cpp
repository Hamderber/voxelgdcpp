#include "hpp/voxel/chunk.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "hpp/tools/log_stream.hpp"
#include "hpp/tools/string.hpp"
#include "hpp/voxel/block.hpp"
#include "hpp/voxel/chunk_mesher.hpp"
#include "hpp/voxel/constants.hpp"
#include "hpp/voxel/world.hpp"
#include <cstdint>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/color.hpp>

using namespace godot;
using namespace Voxel::Resource;

#define DEBUG_VERBOSE

namespace Voxel
{
    void Chunk::_ready()
    {
        set_notify_transform(true);
        initialize_block_data();
        ensure_instance();
        sync_instance_transform();
    }

    void Chunk::_exit_tree()
    {
        RenderingServer *pRenderingServer = RenderingServer::get_singleton();

        if (pRenderingServer && m_instanceRID.is_valid())
        {
            pRenderingServer->free_rid(m_instanceRID);
            m_instanceRID = RID();
        }
    }

    void Chunk::_notification(int p_what)
    {
        if (p_what == NOTIFICATION_TRANSFORM_CHANGED)
        {
            sync_instance_transform();
        }
    }

    void Chunk::ensure_instance()
    {
        if (m_instanceRID.is_valid())
        {
            return;
        }

        RenderingServer *pRenderingServer = RenderingServer::get_singleton();
        if (!pRenderingServer)
            return;

        Ref<World3D> world = get_world_3d();
        if (world.is_null())
            return;

        const RID scenario = world->get_scenario();
        if (!scenario.is_valid())
            return;

        m_instanceRID = pRenderingServer->instance_create2(m_mesh->get_rid(), scenario);
        pRenderingServer->instance_set_custom_aabb(m_instanceRID, AABB(CHUNK_AAA(), CHUNK_BBB()));
        pRenderingServer->instance_set_visible(m_instanceRID, true);
    }

    void Chunk::initialize_block_data()
    {
        m_pBlocks = std::make_unique<Block *[]>(CHUNK_BLOCK_COUNT_MAX);

        for (uint32_t i{}; i < CHUNK_BLOCK_COUNT_MAX; i++)
        {
            auto block = new Block();
            block->set_solid(false);
            m_pBlocks[i] = block;
        }

        ChunkMesher::create_mesh(this, m_mesh);
    }

    void Chunk::set_world_position(World *pWorld, int x, int z)
    {
        m_pWorld = pWorld;

        constexpr int L = static_cast<int>(CHUNK_AXIS_LENGTH_U);

        m_chunk_pos = godot::Vector2i(x / L, z / L);
        set_position(godot::Vector3i(x, 0, z));
        Tools::Log::debug() << "Set chunk " << this << " to " << Tools::String::to_string(m_chunk_pos)
                            << " in world " << pWorld << ".";
    }

    void Chunk::sync_instance_transform()
    {
        if (!m_instanceRID.is_valid())
            return;

        RenderingServer *rs = RenderingServer::get_singleton();
        if (!rs)
            return;

        rs->instance_set_transform(m_instanceRID, get_global_transform());
    }

    const Block *Chunk::get_block_at(godot::Vector3 p_pos)
    {
        return get_block_at(p_pos.x, p_pos.y, p_pos.z);
    }

    const Block *Chunk::get_block_at(uint32_t x, uint32_t y, uint32_t z)
    {
        if (!m_pBlocks)
        {
            Tools::Log::error() << "Attempted to access block at "
                                << Tools::String::xyz_to_string(x, y, z)
                                << " but the chunk's block data wasn't initialized.";
        }

        return m_pBlocks[get_block_index_local(x, y, z)];
    }

    void Chunk::generate_blocks()
    {
        const uint32_t XZ = CHUNK_AXIS_LENGTH_U;
        const uint32_t Y = CHUNK_HEIGHT_U;

        auto rng = m_pWorld->get_generation_rng();
        auto settings = m_pWorld->get_settings();
        auto sea_level = settings->get_sea_level();

        for (int y = 0; y < Y; y++)
        {
            for (int z = 0; z < XZ; z++)
            {
                for (int x = 0; x < XZ; x++)
                {
                    // 1 / belowSeaLevel solid vs air at/below sea level, 1 / aboveSeaLevel above
                    const int belowSeaLevel = 5;
                    const int aboveSeaLevel = 100;
                    auto block = new Block();
                    bool solid = rng->randi_range(1, y < sea_level ? belowSeaLevel : aboveSeaLevel) == 1;

                    if (solid)
                    {
                        // 0 is for unknown only
                        auto index = rng->randi_range(1, Pallet::TYPE_COUNT - 1);
                        block->set_material_type(static_cast<Pallet::MaterialType>(index));
                        block->set_solid(solid);
                        block->set_texture(static_cast<Pallet::BlockTexture>(index));
                    }

                    m_pBlocks[get_block_index_local(x, y, z)] = block;
                }
            }
        }

        Tools::Log::debug() << "(Re)generated blocks for chunk at " << Tools::String::to_string(m_chunk_pos) << ".";

        ChunkMesher::create_mesh(this, m_mesh);
        remesh_neighbors();
    }

    void Chunk::remesh()
    {
        if (mesh_locked())
            return;

        ChunkMesher::create_mesh(this, m_mesh);
    }

    Chunk::Neighbors Chunk::get_neighbors()
    {
        auto pos_x = Vector2i(m_chunk_pos.x + 1, m_chunk_pos.y);
        auto neg_x = Vector2i(m_chunk_pos.x - 1, m_chunk_pos.y);
        auto pos_z = Vector2i(m_chunk_pos.x, m_chunk_pos.y + 1);
        auto neg_z = Vector2i(m_chunk_pos.x, m_chunk_pos.y - 1);

#ifdef DEBUG_VERBOSE
        Tools::Log::debug() << "Getting neighbors for chunk " << Tools::String::to_string(m_chunk_pos) << " at "
                            << Tools::String::to_string(pos_x) << ", " << Tools::String::to_string(neg_x) << ", "
                            << Tools::String::to_string(pos_z) << ", and " << Tools::String::to_string(neg_z) << ".";
#endif

        Chunk::Neighbors neighbors{};
        neighbors.pos_x = m_pWorld->try_get_chunk(pos_x);
        neighbors.neg_x = m_pWorld->try_get_chunk(neg_x);
        neighbors.pos_z = m_pWorld->try_get_chunk(pos_z);
        neighbors.neg_z = m_pWorld->try_get_chunk(neg_z);

        return neighbors;
    }

    void Chunk::remesh_neighbors()
    {
        auto neighbors = get_neighbors();

        if (neighbors.pos_x)
            neighbors.pos_x->remesh();

        if (neighbors.neg_x)
            neighbors.neg_x->remesh();

        if (neighbors.pos_z)
            neighbors.pos_z->remesh();

        if (neighbors.neg_z)
            neighbors.neg_z->remesh();
    }
} //namespace Voxel
