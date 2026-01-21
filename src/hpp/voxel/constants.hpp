#pragma once

#include "godot_cpp/variant/vector3.hpp"
#include <cstdint>

namespace Voxel
{
    // World
    static constexpr uint32_t CHUNK_AXIS_LENGTH_U = 16u;
    static constexpr uint32_t CHUNK_HEIGHT_U = 512u;
    static constexpr float CHUNK_AXIS_LENGTH_F = static_cast<float>(CHUNK_AXIS_LENGTH_U);
    static constexpr uint32_t CHUNK_BLOCK_COUNT_MAX = CHUNK_AXIS_LENGTH_U * CHUNK_AXIS_LENGTH_U * CHUNK_HEIGHT_U;
    static constexpr uint32_t SIMULATION_DISTANCE_MAX = 64u;

    // Textures
    static constexpr int ATLAS_TILES_PER_ROW = 32;
    static constexpr int ATLAS_TILES_PER_COLUMN = 32;
    static constexpr float TILE_UV_SIZE = 1.f / static_cast<float>(ATLAS_TILES_PER_ROW);

    // Math
    const inline godot::Vector3 CHUNK_AAA() { return godot::Vector3(0, 0, 0); }
    const inline godot::Vector3 CHUNK_BBB() { return godot::Vector3(CHUNK_AXIS_LENGTH_F, CHUNK_HEIGHT_U, CHUNK_AXIS_LENGTH_U); }
} //namespace Voxel
