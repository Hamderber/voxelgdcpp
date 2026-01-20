#include "hpp/voxel/world.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/templates/hashfuncs.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include "hpp/tools/hash.hpp"
#include "hpp/tools/log.hpp"
#include "hpp/tools/log_stream.hpp"
#include "hpp/voxel/constants.hpp"
#include <cstdint>

using namespace godot;

namespace Voxel
{
    void World::_ready()
    {
        default_pallet();
        default_generation_settings();
        set_generation_rng();
        build_debounce_timer();
        subscribe_to_signals();

        build_spawn();
    }

    void World::set_generation_rng()
    {
        if (!m_worldGenRNG.is_valid())
        {
            m_worldGenRNG.instantiate();
        }

        m_worldGenRNG->set_seed(m_seed);
    }

    void World::_exit_tree() {}

    void World::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_view_distance"), &World::get_render_distance);
        ClassDB::bind_method(D_METHOD("set_view_distance", "v"), &World::set_render_distance);

        std::stringstream ss;
        ss << "1," << SIMULATION_DISTANCE_MAX << ",suffix:Chunks";
        ADD_PROPERTY(PropertyInfo(Variant::INT, "view_distance", PROPERTY_HINT_RANGE, ss.str().c_str()),
                     "set_view_distance", "get_view_distance");

        ClassDB::bind_method(D_METHOD("get_seed"), &World::get_seed);
        ClassDB::bind_method(D_METHOD("set_seed", "s"), &World::set_seed);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

        godot::ClassDB::bind_method(godot::D_METHOD("get_spawn_radius"), &World::get_spawn_radius);
        godot::ClassDB::bind_method(godot::D_METHOD("set_spawn_radius", "s"), &World::set_spawn_radius);
        ADD_PROPERTY(godot::PropertyInfo(godot::Variant::INT, "radius", godot::PROPERTY_HINT_RANGE, "1,10"),
                     "set_spawn_radius", "get_spawn_radius");

        ClassDB::bind_method(D_METHOD("get_pallet"), &World::get_pallet);
        ClassDB::bind_method(D_METHOD("set_pallet", "p"), &World::set_pallet);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "pallet", PROPERTY_HINT_RESOURCE_TYPE, "Pallet"),
                     "set_pallet", "get_pallet");

        ClassDB::bind_method(D_METHOD("get_settings"), &World::get_settings);
        ClassDB::bind_method(D_METHOD("set_settings", "g"), &World::set_settings);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "settings", PROPERTY_HINT_RESOURCE_TYPE, "GenerationSettings"),
                     "set_settings", "get_settings");

        ClassDB::bind_method(D_METHOD("request_rebuild"), &World::request_rebuild);
        ClassDB::bind_method(D_METHOD("rebuild"), &World::rebuild);
        ClassDB::bind_method(D_METHOD("rebuild_debounce_timer"), &World::rebuild_debounce_timer);
    }

    void World::build_debounce_timer()
    {
        m_pDebounceTimer = memnew(Timer);
        m_pDebounceTimer->set_wait_time(DEBOUNCE_DELAY);
        m_pDebounceTimer->set_one_shot(true);
        add_child(m_pDebounceTimer);
    }

    void World::rebuild_debounce_timer()
    {
        m_pDebounceTimer->stop();
        m_pDebounceTimer->start();
    }

    void World::default_pallet()
    {
        if (m_pallet.is_valid())
            return;

        m_pallet.instantiate();
        m_pallet->default_pallet();
    }

    void World::default_generation_settings()
    {
        if (m_generationSettings.is_valid())
            return;

        m_generationSettings.instantiate();

        Tools::Log::debug("Created default world generation settings.");
    }

    void World::subscribe_to_signals()
    {
        m_pDebounceTimer->connect("timeout", Callable(this, "rebuild"));
        m_generationSettings->connect("changed", Callable(this, "request_rebuild"));

        Tools::Log::debug("World subscribed to signal(s).");
    }

    void World::request_rebuild()
    {
        if (!is_inside_tree())
            return;

        if (!m_pDebounceTimer)
        {
            rebuild_debounce_timer();
        }
        else
        {
            m_pDebounceTimer->stop();
            m_pDebounceTimer->set_wait_time(DEBOUNCE_DELAY);
            m_pDebounceTimer->start();
        }

        Tools::Log::debug("World rebuild requested!");
    }

    void World::rebuild()
    {
        // TODO: Un-dirty
        m_dirty = true;

        set_generation_rng();

        for (auto chunk : m_chunks)
        {
            chunk.second->generate_blocks();
        }

        Tools::Log::debug("World rebuild executed!");
    }

    void World::generate_new_chunk(int x, int z)
    {
        Chunk *pChunk = memnew(Chunk);
        pChunk->set_world_position(this, x, z);
        pChunk->set_pallet(m_pallet);

        m_chunks.emplace(Tools::Hash::chunk(pChunk), pChunk);

        pChunk->set_name("Chunk_" + itos(x) + "_" + itos(z));

        add_child(pChunk);
        pChunk->set_owner(this);

        pChunk->generate_blocks();
    }

    void World::build_spawn()
    {
        Tools::Log::debug() << "Building spawn...";

        uint32_t count = 0;
        auto L = CHUNK_AXIS_LENGTH_U;

        for (int32_t x = -m_spawnRadius; x < m_spawnRadius; x++)
        {
            for (int32_t z = -m_spawnRadius; z < m_spawnRadius; z++)
            {
                generate_new_chunk(x * L, z * L);
                count++;
            }
        }

        Tools::Log::debug() << "Spawn complete. " << count << " chunks generated.";
    }
} //namespace Voxel
