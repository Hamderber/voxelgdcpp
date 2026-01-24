#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/classes/texture.hpp"
#include <godot_cpp/core/class_db.hpp>

namespace Voxel::Resource
{
    class Pallet : public godot::Resource
    {
        GDCLASS(Pallet, godot::Resource);

    public:
        enum MaterialType
        {
            TYPE_UNKNOWN = 0,
            TYPE_GENERIC,
            TYPE_GLASS,
            TYPE_METAL,
            TYPE_COUNT
        };

        enum BlockTexture
        {
            TEXTURE_MISSING = 0,
            TEXTURE_UNUSED_1,
            TEXTURE_UNUSED_2,
            TEXTURE_UNUSED_3,
            TEXTURE_UNUSED_4,
            TEXTURE_UNUSED_5,
            TEXTURE_UNUSED_6,
            TEXTURE_UNUSED_7,
            TEXTURE_UNUSED_8,
            TEXTURE_UNUSED_9,
            TEXTURE_UNUSED_10,
            TEXTURE_UNUSED_11,
            TEXTURE_UNUSED_12,
            TEXTURE_UNUSED_13,
            TEXTURE_UNUSED_14,
            TEXTURE_UNUSED_15,
            TEXTURE_UNUSED_16,
            TEXTURE_UNUSED_17,
            TEXTURE_UNUSED_18,
            TEXTURE_UNUSED_19,
            TEXTURE_UNUSED_20,
            TEXTURE_UNUSED_21,
            TEXTURE_UNUSED_22,
            TEXTURE_UNUSED_23,
            TEXTURE_UNUSED_24,
            TEXTURE_UNUSED_25,
            TEXTURE_UNUSED_26,
            TEXTURE_UNUSED_27,
            TEXTURE_UNUSED_28,
            TEXTURE_UNUSED_29,
            TEXTURE_UNUSED_30,
            TEXTURE_UNUSED_31,
        };

        godot::Ref<godot::StandardMaterial3D> get_material(int p_type) const;

        godot::Ref<godot::StandardMaterial3D> get_unknown_material() const { return get_material(TYPE_UNKNOWN); }
        void set_unknown_material(godot::Ref<godot::StandardMaterial3D> m)
        {
            m_materials[TYPE_UNKNOWN] = m;
            emit_changed();
        }

        godot::Ref<godot::StandardMaterial3D> get_generic_material() const { return get_material(TYPE_GENERIC); }
        void set_generic_material(godot::Ref<godot::StandardMaterial3D> m)
        {
            m_materials[TYPE_GENERIC] = m;
            emit_changed();
        }

        godot::Ref<godot::StandardMaterial3D> get_glass_material() const { return get_material(TYPE_GLASS); }
        void set_glass_material(godot::Ref<godot::StandardMaterial3D> m)
        {
            m_materials[TYPE_GLASS] = m;
            emit_changed();
        }

        godot::Ref<godot::StandardMaterial3D> get_metal_material() const { return get_material(TYPE_METAL); }
        void set_metal_material(godot::Ref<godot::StandardMaterial3D> m)
        {
            m_materials[TYPE_METAL] = m;
            emit_changed();
        }

        godot::Ref<godot::Texture> get_atlas() const { return m_atlas; }
        void set_atlas(godot::Ref<godot::Texture> p_atlas);

        void apply_atlas_to_materials();
        void default_pallet();

    protected:
        static void _bind_methods();

    private:
        godot::Ref<godot::StandardMaterial3D> m_materials[TYPE_COUNT];
        godot::Ref<godot::Texture> m_atlas;
    };
} //namespace Voxel::Resource
