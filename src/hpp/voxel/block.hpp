#pragma once

#include "resource/pallet.hpp"

namespace Voxel
{
    class Block
    {
    public:
        // enum Face
        // {
        //     FACE_Z_POS = 0,
        //     FACE_Z_NEG,
        //     FACE_X_POS,
        //     FACE_X_NEG,
        //     FACE_Y_POS,
        //     FACE_Y_NEG,
        // };

        Block() = default;
        ~Block() = default;

        void set_solid(bool p_isSolid) { m_isSolid = p_isSolid; }
        bool is_solid() const { return m_isSolid; }
        bool opaque() const { return m_isSolid && m_materialType != Resource::Pallet::MaterialType::TYPE_GLASS; }

        void set_material_type(Resource::Pallet::MaterialType p_material) { m_materialType = p_material; }
        Resource::Pallet::MaterialType get_material_type() const { return m_materialType; }

        void set_texture(Resource::Pallet::BlockTexture p_texture) { m_texture = p_texture; }
        Resource::Pallet::BlockTexture get_texture() const { return m_texture; }

    private:
        bool m_isSolid = false;
        Resource::Pallet::BlockTexture m_texture = Resource::Pallet::TEXTURE_MISSING;
        Resource::Pallet::MaterialType m_materialType = Resource::Pallet::MaterialType::TYPE_GENERIC;
    };
} //namespace Voxel
