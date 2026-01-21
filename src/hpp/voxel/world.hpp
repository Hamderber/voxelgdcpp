#pragma once

#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "hpp/voxel/chunk.hpp"
#include "resource/generation_settings.hpp"
#include "resource/pallet.hpp"
#include <cstdint>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <hpp/tools/log.hpp>
#include <unordered_map>

namespace Voxel
{
    namespace Resource
    {
        class GenerationSettings;
    }

    class World : public godot::Node3D
    {
        GDCLASS(World, godot::Node3D)

    public:
        World() = default;
        ~World() override = default;

        void _ready() override;
        void _exit_tree() override;

        int32_t get_render_distance() const { return m_renderDistance; }
        void set_render_distance(int32_t v)
        {
            m_renderDistance = godot::MAX(v, 1);
            request_rebuild();
        }

        int64_t get_seed() const { return m_seed; }
        void set_seed(int64_t s)
        {
            m_seed = s;
            request_rebuild();
        }

        int32_t get_spawn_radius() const { return m_spawnRadius; }
        void set_spawn_radius(int32_t s)
        {
            m_spawnRadius = godot::CLAMP(s, 1, 25);
            request_rebuild();
        }

        godot::Ref<Resource::Pallet> get_pallet() const { return m_pallet; }
        void set_pallet(const godot::Ref<Resource::Pallet> &p)
        {
            m_pallet = p;
            request_rebuild();
        }

        godot::Ref<Resource::GenerationSettings> get_settings() const { return m_generationSettings; }
        void set_settings(const godot::Ref<Resource::GenerationSettings> &g)
        {
            m_generationSettings = g;
            request_rebuild();
        }

        godot::Ref<godot::RandomNumberGenerator> get_generation_rng() { return m_worldGenRNG; }

    protected:
        static void _bind_methods();

    private:
        void set_generation_rng();
        void build_debounce_timer();
        void rebuild_debounce_timer();
        void default_pallet();
        void default_generation_settings();
        void subscribe_to_signals();

        void request_rebuild();
        void rebuild();

        void build_spawn();
        void generate_new_chunk(int x, int y);

        godot::Timer *m_pDebounceTimer;
        const double DEBOUNCE_DELAY = 1.5;

        // TODO: Implement material object dither distance fade for all chunk materials based on this value and update when
        // it changes
        int32_t m_renderDistance = 6;
        int64_t m_seed = 8675309;
        int32_t m_spawnRadius = 3;
        godot::Ref<Resource::Pallet> m_pallet;
        godot::Ref<Resource::GenerationSettings> m_generationSettings;
        godot::Ref<godot::RandomNumberGenerator> m_worldGenRNG;
        bool m_dirty = true;

        std::unordered_map<uint64_t, Chunk *> m_chunks;
    };
} //namespace Voxel
