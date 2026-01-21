#pragma once

#include "godot_cpp/classes/base_material3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "hpp/tools/log.hpp"
#include <sstream>
#include <string_view>

namespace Tools
{
    class Material
    {
    public:
        static const godot::Color get_unknown_color() { return godot::Color(1.f, 0.f, 0.86f); }
        static godot::Ref<godot::StandardMaterial3D> get_default_material()
        {
            if (!m_material.is_valid())
            {
                godot::Ref<godot::StandardMaterial3D> mat3d;

                mat3d.instantiate();
                mat3d->set_name("UNKNOWN_MATERIAL");
                mat3d->set_albedo(get_unknown_color());
                mat3d->set_texture_filter(godot::BaseMaterial3D::TEXTURE_FILTER_NEAREST);

                m_material = mat3d;

                Tools::Log::debug("Created default UNKNOWN_MATERIAL.");
            }

            return m_material;
        }

        static void EnsureDefaultMaterial(godot::Ref<godot::StandardMaterial3D> &p_material, std::string_view p_objName = "an object")
        {
            if (p_material.is_valid())
                return;

            p_material = get_default_material();
            p_material->emit_changed();

            std::stringstream ss;
            ss << "Set material to default for " << p_objName << ".";
            Tools::Log::debug(ss.str());
        }

    private:
        static godot::Ref<godot::StandardMaterial3D> m_material;
    };
} //namespace Tools
