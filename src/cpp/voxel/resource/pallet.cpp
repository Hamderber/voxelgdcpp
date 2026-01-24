#include "hpp/voxel/resource/pallet.hpp"
#include "godot_cpp/classes/base_material3d.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/classes/texture.hpp"
#include "hpp/tools/log_stream.hpp"
#include "hpp/tools/material.hpp"

using namespace godot;

namespace Voxel::Resource
{
    void Pallet::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_material", "type"), &Pallet::get_material);

        BIND_CONSTANT(TYPE_UNKNOWN);
        BIND_CONSTANT(TYPE_GENERIC);
        BIND_CONSTANT(TYPE_GLASS);
        BIND_CONSTANT(TYPE_METAL);
        BIND_CONSTANT(TYPE_COUNT);

        ClassDB::bind_method(D_METHOD("get_unknown_material"), &Pallet::get_unknown_material);
        ClassDB::bind_method(D_METHOD("set_unknown_material", "m"), &Pallet::set_unknown_material);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "unknown_material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"),
                     "set_unknown_material", "get_unknown_material");

        ClassDB::bind_method(D_METHOD("get_generic_material"), &Pallet::get_generic_material);
        ClassDB::bind_method(D_METHOD("set_generic_material", "m"), &Pallet::set_generic_material);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "generic_material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"),
                     "set_generic_material", "get_generic_material");

        ClassDB::bind_method(D_METHOD("get_glass_material"), &Pallet::get_glass_material);
        ClassDB::bind_method(D_METHOD("set_glass_material", "m"), &Pallet::set_glass_material);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "glass_material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"),
                     "set_glass_material", "get_glass_material");

        ClassDB::bind_method(D_METHOD("get_metal_material"), &Pallet::get_metal_material);
        ClassDB::bind_method(D_METHOD("set_metal_material", "m"), &Pallet::set_metal_material);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "metal_material", PROPERTY_HINT_RESOURCE_TYPE, "StandardMaterial3D"),
                     "set_metal_material", "get_metal_material");

        ClassDB::bind_method(D_METHOD("get_atlas"), &Pallet::get_atlas);
        ClassDB::bind_method(D_METHOD("set_atlas", "p_atlas"), &Pallet::set_atlas);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "atlas", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_atlas", "get_atlas");
    }

    Ref<StandardMaterial3D> Pallet::get_material(int p_type) const
    {
        if (p_type >= 0 && p_type < TYPE_COUNT)
            return m_materials[p_type];

        Tools::Log::error() << "Invalid material type:" << p_type;

        return m_materials[TYPE_UNKNOWN];
    }

    void Pallet::default_pallet()
    {
        Tools::Material::EnsureDefaultMaterial(m_materials[TYPE_UNKNOWN], "Pallet");

        m_materials[TYPE_GENERIC] = m_materials[TYPE_UNKNOWN]->duplicate();
        m_materials[TYPE_GENERIC]->set_name("Generic");
        m_materials[TYPE_GENERIC]->set_albedo(Color(1.f, 1.f, 1.f));

        m_materials[TYPE_GLASS] = m_materials[TYPE_UNKNOWN]->duplicate();
        m_materials[TYPE_GLASS]->set_name("Glass");
        m_materials[TYPE_GLASS]->set_albedo(Color(.8f, .8f, .8f, .5f));
        m_materials[TYPE_GLASS]->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
        // m_materials[TYPE_GLASS]->set_shading_mode(godot::BaseMaterial3D::SHADING_MODE_UNSHADED);
        // Might need to change this if transluscents look wrong on areas where the sun is blocked (ex. caves)
        m_materials[TYPE_GLASS]->set_flag(godot::BaseMaterial3D::FLAG_DONT_RECEIVE_SHADOWS, true);

        m_materials[TYPE_METAL] = m_materials[TYPE_UNKNOWN]->duplicate();
        m_materials[TYPE_METAL]->set_name("Metal");
        m_materials[TYPE_METAL]->set_albedo(Color(1.f, 1.f, 1.f));
        m_materials[TYPE_METAL]->set_metallic(1.f);

        String atlas_path = "res://textures/voxel_atlas.png";

        ResourceLoader *loader = ResourceLoader::get_singleton();
        m_atlas = loader->load(atlas_path);

        if (m_atlas.is_null())
        {
            if (!FileAccess::file_exists(atlas_path))
            {
                Tools::Log::error() << "File does NOT exist at: " << atlas_path.ascii().ptr();
            }
            else
            {
                Tools::Log::error() << "File exists but failed to load as resource at: " << atlas_path.ptr()
                                    << " (likely import failure or invalid format)";
            }
        }
        else
        {
            Tools::Log::debug() << "Successfully loaded atlas: " << atlas_path.ptr()
                                << " (type: " << m_atlas->get_class().ascii().ptr() << ")";
        }

        apply_atlas_to_materials();

        emit_changed();
        Tools::Log::debug("Created default pallet.");
    }

    void Pallet::set_atlas(Ref<Texture> p_atlas)
    {
        m_atlas = p_atlas;
        apply_atlas_to_materials();
        emit_changed();
    }

    void Pallet::apply_atlas_to_materials()
    {
        if (m_atlas.is_null())
            return;

        for (int i = 0; i < TYPE_COUNT; ++i)
        {
            if (m_materials[i].is_valid())
            {
                m_materials[i]->set("albedo_texture", Variant(m_atlas));
            }
        }
    }
} //namespace Voxel::Resource
