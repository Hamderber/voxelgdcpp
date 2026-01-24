#pragma once

#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "hpp/voxel/chunk.hpp"

namespace Voxel
{
    class ChunkMesher
    {
    public:
        struct SurfaceData
        {
            godot::PackedVector3Array vertices;
            godot::PackedVector3Array vertex_normals;
            godot::PackedVector2Array uvs;
            godot::PackedInt32Array indices;
        };

        struct FacePoints
        {
            const godot::Vector3 &p1;
            const godot::Vector3 &p2;
            const godot::Vector3 &p3;
            const godot::Vector3 &p4;
        };

        struct CubePoints
        {
            godot::Vector3 p000;
            godot::Vector3 p100;
            godot::Vector3 p110;
            godot::Vector3 p010;
            godot::Vector3 p001;
            godot::Vector3 p101;
            godot::Vector3 p111;
            godot::Vector3 p011;

            const FacePoints pos_z() { return { p001, p101, p111, p011 }; }
            const FacePoints neg_z() { return { p100, p000, p010, p110 }; }
            const FacePoints pos_x() { return { p101, p100, p110, p111 }; }
            const FacePoints neg_x() { return { p000, p001, p011, p010 }; }
            const FacePoints pos_y() { return { p011, p111, p110, p010 }; }
            const FacePoints neg_y() { return { p000, p100, p101, p001 }; }
        };

        static void create_mesh(Chunk *p_chunk, godot::Ref<godot::ArrayMesh> &p_mesh);
    };
} //namespace Voxel
