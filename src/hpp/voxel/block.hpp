#pragma once

#include "resource/pallet.hpp"

namespace Voxel
{
    class Block
    {
    public:
        Block() = default;
        ~Block() = default;

        void set_solid(bool p_isSolid) { m_isSolid = p_isSolid; }
        bool is_solid() const { return m_isSolid; }

        void set_material_type(Resource::Pallet::MaterialType p_material) { m_materialType = p_material; }
        Resource::Pallet::MaterialType get_material_type() const { return m_materialType; }

    private:
        bool m_isSolid;
        Resource::Pallet::MaterialType m_materialType = Resource::Pallet::MaterialType::TYPE_GENERIC;
    };
} //namespace Voxel
