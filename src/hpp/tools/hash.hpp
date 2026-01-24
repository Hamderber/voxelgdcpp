#pragma once

#include "godot_cpp/variant/vector2i.hpp"
#include "hpp/voxel/chunk.hpp"
#include <cstdint>

namespace Tools
{
    class Hash
    {
    public:
        static const uint64_t chunk(Voxel::Chunk *pChunk)
        {
            const auto pos = pChunk->get_pos();
            return chunk_pos(pos);
        }

        static const uint64_t chunk_pos(godot::Vector2i pos)
        {
            return (static_cast<uint64_t>(pos.x) << 32 | static_cast<uint64_t>(pos.y));
        }
    };
} //namespace Tools
