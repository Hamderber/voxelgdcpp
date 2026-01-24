#pragma once

// #include "godot_cpp/variant/vector3i.hpp"
#include "resource/pallet.hpp"

namespace Voxel
{
    // enum class OreintationMode
    // {
    //     // Stone
    //     NONE,
    //     // Logs
    //     AXIS,
    //     // Furnaces
    //     FACING,
    //     // Stairs
    //     FULL_VOXEL
    // };

    class Block
    {
    public:
        Block() = default;
        ~Block() = default;

        void set_solid(bool p_isSolid) { m_isSolid = p_isSolid; }
        bool is_solid() const { return m_isSolid; }

        void set_material_type(Resource::Pallet::MaterialType p_material) { m_materialType = p_material; }
        Resource::Pallet::MaterialType get_material_type() const { return m_materialType; }

        void set_texture(Resource::Pallet::BlockTexture p_texture) { m_texture = p_texture; }
        Resource::Pallet::BlockTexture get_texture() const { return m_texture; }

        // void set_random_oreintation(bool p_toggle) { m_randomOreintation = p_toggle; }
        // bool is_random_texture_oreintation() const { return m_randomOreintation; }

        // void set_oreintation_mode(OreintationMode p_oreintationMode) { m_oreintationMode = p_oreintationMode; }
        // OreintationMode get_oreintation_mode() const { return m_oreintationMode; }

    private:
        bool m_isSolid = false;
        Resource::Pallet::BlockTexture m_texture = Resource::Pallet::TEXTURE_MISSING;
        // bool m_randomOreintation = false;
        // OreintationMode m_oreintationMode = OreintationMode::NONE;
        // godot::Vector3i m_oreintationCtx{};
        Resource::Pallet::MaterialType m_materialType = Resource::Pallet::MaterialType::TYPE_GENERIC;
    };
} //namespace Voxel
