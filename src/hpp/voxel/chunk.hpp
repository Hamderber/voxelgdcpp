#pragma once

#include "block.hpp"
#include "constants.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "resource/pallet.hpp"
#include <cstdint>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/rid.hpp>

namespace Voxel
{
    class World;

    class Chunk : public godot::Node3D
    {
        GDCLASS(Chunk, Node3D)
    public:
        struct Neighbors
        {
            Chunk *pos_x;
            Chunk *neg_x;
            Chunk *pos_z;
            Chunk *neg_z;
        };

        typedef godot::Vector2i ChunkPos;

        Chunk() = default;
        ~Chunk() override = default;

        void _ready() override;
        void _exit_tree() override;

        void set_pallet(godot::Ref<Resource::Pallet> p_pallet) { m_pallet = p_pallet; }
        void set_world_position(World *pWorld, int x, int y);
        const Block *get_block_at(godot::Vector3 p_pos);
        const Block *get_block_at(uint32_t x, uint32_t y, uint32_t z);
        inline size_t get_block_index_local(uint32_t x, uint32_t y, uint32_t z) const
        {
            return x +
                   static_cast<size_t>(z) * CHUNK_AXIS_LENGTH_U +
                   static_cast<size_t>(y) * (CHUNK_AXIS_LENGTH_U * CHUNK_AXIS_LENGTH_U);
        }
        const ChunkPos get_pos() { return m_chunk_pos; }

        void generate_blocks();

        World *get_world() const { return m_pWorld; }
        const godot::RID &get_rid() const { return m_instanceRID; }

        Neighbors get_neighbors();

        void mesh_lock() { m_mesh_locked = true; }
        void mesh_unlock() { m_mesh_locked = false; }
        bool mesh_locked() const { return m_mesh_locked; }
        void remesh();
        void remesh_neighbors();

    protected:
        static void _bind_methods() {}
        void _notification(int p_what);

    private:
        void initialize_block_data();
        void ensure_instance();
        void sync_instance_transform();

        bool m_isInitialized = false;

        World *m_pWorld;

        godot::Ref<Resource::Pallet> m_pallet;
        godot::Ref<godot::ArrayMesh> m_mesh;
        godot::RID m_instanceRID;

        ChunkPos m_chunk_pos;
        std::unique_ptr<Voxel::Block *[]> m_pBlocks;
        // The mesh starts locked so that the chunk isnt meshed until its neighbors are available during spawn generation.
        // Initial mesh creation still happens, though remeshing is prevented until the mesh is unlocked
        bool m_mesh_locked = true;
    };
} //namespace Voxel
