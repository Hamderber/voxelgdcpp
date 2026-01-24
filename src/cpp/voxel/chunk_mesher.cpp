#include "hpp/voxel/chunk_mesher.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "hpp/tools/log_stream.hpp"
#include "hpp/tools/string.hpp"
#include "hpp/voxel/block.hpp"
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
#ifdef DEBUG_VERBOSE
    static uint32_t num_faces = 0;
    static uint32_t num_faces_skipped = 0;
#endif

    static void add_face(
            PackedVector3Array &vertices,
            PackedVector3Array &vertex_normals,
            PackedVector2Array &uvs,
            PackedInt32Array &indices,
            const Vector3 &v0,
            const Vector3 &v1,
            const Vector3 &v2,
            const Vector3 &v3,
            const Vector3 &normal,
            const Vector2 &uv_base_offset)
    {
        const int base_index = vertices.size();

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        vertex_normals.push_back(normal);
        vertex_normals.push_back(normal);
        vertex_normals.push_back(normal);
        vertex_normals.push_back(normal);

        const float uv_size = TILE_UV_SIZE;
        const float margin = 0.001f;

        uvs.push_back(uv_base_offset + Vector2(margin, uv_size - margin));
        uvs.push_back(uv_base_offset + Vector2(uv_size - margin, uv_size - margin));
        uvs.push_back(uv_base_offset + Vector2(uv_size - margin, margin));
        uvs.push_back(uv_base_offset + Vector2(margin, margin));

        // Clockwise winding for Godot
        indices.push_back(base_index + 0);
        indices.push_back(base_index + 2);
        indices.push_back(base_index + 1);

        indices.push_back(base_index + 0);
        indices.push_back(base_index + 3);
        indices.push_back(base_index + 2);

#ifdef DEBUG_VERBOSE
        num_faces++;
#endif
    }

    static Vector2 get_tile_uv_offset(Resource::Pallet::BlockTexture type)
    {
        int tile_index = static_cast<int>(type);

        if (tile_index < 0 || tile_index >= (ATLAS_TILES_PER_ROW * ATLAS_TILES_PER_COLUMN))
        {
            tile_index = 0;
        }

        int tile_x = tile_index % ATLAS_TILES_PER_ROW;
        int tile_y = tile_index / ATLAS_TILES_PER_COLUMN;

        return Vector2(
                static_cast<float>(tile_x) * TILE_UV_SIZE,
                static_cast<float>(tile_y) * TILE_UV_SIZE);
    }

    static void draw_face(Chunk *p_chunk,
                          Chunk *p_neighbor,
                          ChunkMesher::SurfaceData &p_sd,
                          const ChunkMesher::FacePoints &p_points,
                          const Block *p_block,
                          const int &p_x, const int &p_y, const int &p_z,
                          const Vector3 &p_offset,
                          bool p_block_in_chunk)
    {
        constexpr uint32_t XZ = CHUNK_AXIS_LENGTH_U;
        constexpr uint32_t Y = CHUNK_HEIGHT_U;

        bool draw_face = false;
        if (p_block_in_chunk)
        {
            draw_face = !p_chunk->get_block_at(p_x + p_offset.x, p_y + p_offset.y, p_z + p_offset.z)->opaque();
        }
        else if (p_neighbor)
        {
            draw_face = !p_neighbor->get_block_at(p_x, p_y, 0)->opaque();
#ifdef DEBUG_VERBOSE
            if (!draw_face)
                num_faces_skipped++;
#endif
        }
        else
        {
            draw_face = true;
        }

        if (draw_face)
        {
            Vector2 uv_offset = get_tile_uv_offset(p_block->get_texture());
            add_face(p_sd.vertices, p_sd.vertex_normals, p_sd.uvs, p_sd.indices,
                     p_points.p1, p_points.p2, p_points.p3, p_points.p4,
                     p_offset, uv_offset);
        }
    }

    void ChunkMesher::create_mesh(Chunk *p_chunk, godot::Ref<godot::ArrayMesh> &p_mesh)
    {
#ifdef DEBUG_VERBOSE
        num_faces = 0;
        num_faces_skipped = 0;
#endif
        if (p_mesh.is_valid() && p_mesh->get_surface_count() > 0)
        {
            p_mesh->clear_surfaces();
        }
        else
        {
            p_mesh.instantiate();
        }

        SurfaceData data[Pallet::TYPE_COUNT];

        const uint32_t XZ = CHUNK_AXIS_LENGTH_U;
        const uint32_t Y = CHUNK_HEIGHT_U;

        auto world = p_chunk->get_world();
        auto chunk_pos = p_chunk->get_pos();
        auto neighbors = p_chunk->get_neighbors();

        CubePoints points{};
        bool block_in_chunk = true;

        for (int y = 0; y < Y; y++)
        {
            for (int z = 0; z < XZ; z++)
            {
                for (int x = 0; x < XZ; x++)
                {
                    auto block = p_chunk->get_block_at(x, y, z);
                    if (!block || !block->is_solid())
                        continue;

                    auto type = block->get_material_type();
                    if (type < 0 || type >= Pallet::TYPE_COUNT)
                    {
                        Tools::Log::error() << "Attempted to assign unknown material value " << type
                                            << " to block at " << Tools::String::xyz_to_string(x, y, z)
                                            << " in chunk " << Tools::String::to_string(chunk_pos) << ".";
                        type = Pallet::TYPE_UNKNOWN;
                    }

                    SurfaceData &sd = data[type];

                    const Vector3 o(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));

                    points.p000 = o + Vector3(0, 0, 0);
                    points.p100 = o + Vector3(1, 0, 0);
                    points.p110 = o + Vector3(1, 1, 0);
                    points.p010 = o + Vector3(0, 1, 0);
                    points.p001 = o + Vector3(0, 0, 1);
                    points.p101 = o + Vector3(1, 0, 1);
                    points.p111 = o + Vector3(1, 1, 1);
                    points.p011 = o + Vector3(0, 1, 1);

                    block_in_chunk = z < XZ - 1;
                    draw_face(p_chunk, neighbors.pos_z, sd, points.pos_z(), block, x, y, z, Vector3(0, 0, 1), block_in_chunk);

                    block_in_chunk = z > 0;
                    draw_face(p_chunk, neighbors.neg_z, sd, points.neg_z(), block, x, y, z, Vector3(0, 0, -1), block_in_chunk);

                    block_in_chunk = x == XZ - 1;
                    draw_face(p_chunk, neighbors.pos_x, sd, points.pos_x(), block, x, y, z, Vector3(1, 0, 0), block_in_chunk);

                    block_in_chunk = x > 0;
                    draw_face(p_chunk, neighbors.neg_x, sd, points.neg_x(), block, x, y, z, Vector3(-1, 0, 0), block_in_chunk);

                    block_in_chunk = y < Y - 1;
                    draw_face(p_chunk, nullptr, sd, points.pos_y(), block, x, y, z, Vector3(0, 1, 0), block_in_chunk);

                    block_in_chunk = y > 0;
                    draw_face(p_chunk, nullptr, sd, points.neg_y(), block, x, y, z, Vector3(0, -1, 0), block_in_chunk);
                }
            }
        }

        const int surface_order[] = { Pallet::TYPE_GENERIC, Pallet::TYPE_METAL, Pallet::TYPE_UNKNOWN, Pallet::TYPE_GLASS };

        auto worldPallet = p_chunk->get_world()->get_pallet();
        auto chunkRID = p_chunk->get_rid();

        for (int i = 0; i < Pallet::TYPE_COUNT; i++)
        {
            int type = surface_order[i];
            SurfaceData &sd = data[type];
            if (sd.indices.size() == 0)
                continue;

            Array arrays;
            arrays.resize(Mesh::ARRAY_MAX);
            arrays[Mesh::ARRAY_VERTEX] = sd.vertices;
            arrays[Mesh::ARRAY_NORMAL] = sd.vertex_normals;
            arrays[Mesh::ARRAY_TEX_UV] = sd.uvs;
            arrays[Mesh::ARRAY_INDEX] = sd.indices;

            p_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);

            int surface_idx = p_mesh->get_surface_count() - 1;
            Ref<StandardMaterial3D> mat = worldPallet->get_material(type);
            if (mat.is_valid())
            {
                p_mesh->surface_set_material(surface_idx, mat);
            }
        }

        if (chunkRID.is_valid())
        {
            auto rs = RenderingServer::get_singleton();
            rs->instance_set_base(chunkRID, p_mesh->get_rid());
            rs->instance_set_visible(chunkRID, p_mesh->get_surface_count() > 0);
        }

#ifdef DEBUG_VERBOSE
        if (p_mesh.is_valid() && p_mesh->get_surface_count() > 0)
        {
            Tools::Log::debug() << "Mesh has " << p_mesh->get_surface_count()
                                << " surfaces, " << p_mesh->surface_get_array_len(0)
                                << " vertices, and " << num_faces << " faces for chunk "
                                << Tools::String::to_string(chunk_pos) << ". "
                                << num_faces_skipped << " face(s) were skipped due to neighboring chunk's block being opaque.";
        }
#endif
    }
} //namespace Voxel
