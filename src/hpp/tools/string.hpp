#pragma once

#include "godot_cpp/variant/vector2i.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "hpp/voxel/constants.hpp"
#include <cstdint>
#include <sstream>
#include <string>

namespace Tools
{
    class String
    {
    public:
        static const std::string to_string(godot::Vector3 &p_vector3)
        {
            std::stringstream ss;
            ss << "(" << p_vector3.x << ", " << p_vector3.y << ", " << p_vector3.z << ")";
            return ss.str();
        }

        static const std::string to_string(godot::Vector3i &p_vector3)
        {
            std::stringstream ss;
            ss << "(" << p_vector3.x << ", " << p_vector3.y << ", " << p_vector3.z << ")";
            return ss.str();
        }

        static const std::string to_string(godot::Vector2i &p_vector2)
        {
            std::stringstream ss;
            ss << "(" << p_vector2.x << ", " << p_vector2.y << ")";
            return ss.str();
        }

        static const std::string xyz_to_string(float x, float y, float z)
        {
            std::stringstream ss;
            ss << "(" << x << ", " << y << ", " << z << ")";
            return ss.str();
        }

        static const std::string xyz_to_string(uint32_t x, uint32_t y, uint32_t z)
        {
            std::stringstream ss;
            ss << "(" << x << ", " << y << ", " << z << ")";
            return ss.str();
        }

        static const std::string xyz_to_string(int x, int y, int z)
        {
            std::stringstream ss;
            ss << "(" << x << ", " << y << ", " << z << ")";
            return ss.str();
        }
    };
} //namespace Tools
