#include "coacd.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

#include "../CoACD-godot-src/public/coacd.h"
#include "../CoACD-godot-src/src/model_obj.h"

using namespace godot;

CoACD *CoACD::instance = nullptr;

namespace
{
    class CaptureSink : public spdlog::sinks::base_sink<std::mutex>
    {
    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override
        {
            std::string log_std(msg.payload.begin(), msg.payload.end());
            godot::String log_str(log_std.c_str());
            CoACD::LogLevels level;
            switch (msg.level)
            {
            case spdlog::level::trace:
            case spdlog::level::debug:
                level = CoACD::LOG_DEBUG;
                break;
            case spdlog::level::info:
                level = CoACD::LOG_INFO;
                break;
            case spdlog::level::warn:
                level = CoACD::LOG_WARNING;
                break;
            case spdlog::level::err:
                level = CoACD::LOG_ERROR;
                break;
            case spdlog::level::critical:
                level = CoACD::LOG_CRITICAL;
                break;
            case spdlog::level::off:
            default:
                level = CoACD::LOG_OFF;
                break;
            }
            CoACD *coacd_instance = CoACD::instance;
            coacd_instance->emit_signal("new_log", log_str, level);
            if (coacd_instance->get_print_to_console())
            {
                switch (level)
                {
                case CoACD::LOG_DEBUG:
                    UtilityFunctions::print_verbose(log_str);
                    break;
                case CoACD::LOG_INFO:
                    UtilityFunctions::print(log_str);
                    break;
                case CoACD::LOG_WARNING:
                    UtilityFunctions::push_warning(log_str);
                    break;
                case CoACD::LOG_ERROR:
                case CoACD::LOG_CRITICAL:
                    UtilityFunctions::push_error(log_str);
                    break;
                default:
                    break;
                }
            }
        }
        void flush_() override {}
    };
}

void CoACD::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("convex_decomposition", "mesh", "settings"), &CoACD::convex_decomposition, DEFVAL(Ref<CoACDSettings>()));

    ClassDB::bind_method(D_METHOD("set_log_level", "p_log_level"), &CoACD::set_log_level);
    ClassDB::bind_method(D_METHOD("get_log_level"), &CoACD::get_log_level);
    BIND_ENUM_CONSTANT(LOG_OFF);
    BIND_ENUM_CONSTANT(LOG_DEBUG);
    BIND_ENUM_CONSTANT(LOG_INFO);
    BIND_ENUM_CONSTANT(LOG_WARNING);
    BIND_ENUM_CONSTANT(LOG_ERROR);
    BIND_ENUM_CONSTANT(LOG_CRITICAL);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "log_level", PROPERTY_HINT_ENUM, "OFF,DEBUG,INFO,WARNING,ERROR,CRITICAL"), "set_log_level", "get_log_level");

    ClassDB::bind_method(D_METHOD("set_print_to_console", "p_print_to_console"), &CoACD::set_print_to_console);
    ClassDB::bind_method(D_METHOD("get_print_to_console"), &CoACD::get_print_to_console);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "print_to_console"), "set_print_to_console", "get_print_to_console");

    ADD_SIGNAL(MethodInfo("new_log", PropertyInfo(Variant::STRING, "log"), PropertyInfo(Variant::INT, "log_level", PROPERTY_HINT_ENUM, "OFF,DEBUG,INFO,WARNING,ERROR,CRITICAL")));
}

CoACD::CoACD()
{
    instance = this;

    set_log_level(log_level);
    auto logger = spdlog::get("CoACD");
    auto sink = std::make_shared<CaptureSink>();
    logger->sinks().push_back(sink);
}

CoACD::~CoACD()
{
    instance = nullptr;
}

Array CoACD::convex_decomposition(const Ref<ConcavePolygonShape3D> mesh, Ref<CoACDSettings> settings)
{
    if (!settings.is_valid())
    {
        settings.instantiate();
    }

    ERR_FAIL_COND_V_EDMSG((settings->get_threshold() > 1), Array(), "CoACD threshold > 1 (should be 0.01-1).");
    ERR_FAIL_COND_V_EDMSG((settings->get_threshold() < 0.01), Array(), "CoACD threshold < 0.01 (should be 0.01-1).");

    ERR_FAIL_COND_V_EDMSG((settings->get_preprocess_resolution() > 1000), Array(), "CoACD preprocess resolution > 1000 (should be 20-100).");
    ERR_FAIL_COND_V_EDMSG((settings->get_preprocess_resolution() < 5), Array(), "CoACD preprocess resolution < 5 (should be 20-100).");

    PackedVector3Array points = mesh->get_faces();

    coacd::Mesh coacd_mesh;
    std::vector<Vector3> unique_vertices;
    std::vector<int> triangle_indices;

    for (int i = 0; i < points.size(); i += 3)
    {
        const Vector3 &v1 = points[i];
        const Vector3 &v2 = points[i + 1];
        const Vector3 &v3 = points[i + 2];

        for (const Vector3 &v : {v1, v2, v3})
        {
            int vertex_idx = -1;
            for (int j = 0; j < unique_vertices.size(); j++)
            {
                if (unique_vertices[j].is_equal_approx(v))
                {
                    vertex_idx = j;
                    break;
                }
            }

            if (vertex_idx == -1)
            {
                vertex_idx = unique_vertices.size();
                unique_vertices.push_back(v);
            }

            triangle_indices.push_back(vertex_idx);
        }
    }

    coacd_mesh.vertices.reserve(unique_vertices.size());
    for (const Vector3 &v : unique_vertices)
    {
        coacd_mesh.vertices.push_back({v.x, v.y, v.z});
    }

    coacd_mesh.indices.reserve(triangle_indices.size() / 3);
    for (int i = 0; i < triangle_indices.size(); i += 3)
    {
        coacd_mesh.indices.push_back({triangle_indices[i + 0],
                                      triangle_indices[i + 1],
                                      triangle_indices[i + 2]});
    }

    coacd::Model coacd_model;
    coacd_model.Load(coacd_mesh.vertices, coacd_mesh.indices);
    double mesh_vol = coacd::MeshVolume(coacd_model);
    if (mesh_vol < 0)
    {
        for (int i = 0; i < (int)coacd_mesh.indices.size(); i++)
            std::swap(coacd_mesh.indices[i][0], coacd_mesh.indices[i][1]);
    }

    std::string preprocess_mode;
    switch (settings->get_preprocess_mode())
    {
    case CoACDSettings::AUTO:
        preprocess_mode = "auto";
        break;
    case CoACDSettings::ON:
        preprocess_mode = "on";
        break;
    case CoACDSettings::OFF:
        preprocess_mode = "off";
        break;
    default:
        break;
    }

    std::string approximation_mode;
    switch (settings->get_approximation_mode())
    {
    case CoACDSettings::CONVEX:
        approximation_mode = "ch";
        break;
    case CoACDSettings::BOX:
        approximation_mode = "box";
        break;
    default:
        break;
    }

    std::vector<coacd::Mesh> convex_parts = coacd::CoACD(
        coacd_mesh,
        settings->get_threshold(),
        settings->get_max_convex_hulls(),
        preprocess_mode,
        settings->get_preprocess_resolution(),
        settings->get_sample_resolution(),
        settings->get_mcts_nodes(),
        settings->get_mcts_iterations(),
        settings->get_mcts_max_depth(),
        settings->get_pca(),
        settings->get_merge_postprocessing(),
        settings->get_decimate(),
        settings->get_max_ch_vertex(),
        settings->get_extrude(),
        settings->get_extrude_margin(),
        approximation_mode,
        settings->get_seed());

    Array result;

    for (const auto &part : convex_parts)
    {
        PackedVector3Array convex_points;
        convex_points.resize(part.indices.size() * 3);

        for (size_t i = 0; i < part.indices.size(); i++)
        {
            const auto &triangle = part.indices[i];
            for (int j = 0; j < 3; j++)
            {
                const auto &vertex = part.vertices[triangle[j]];
                convex_points[i * 3 + j] = Vector3(vertex[0], vertex[1], vertex[2]);
            }
        }

        Ref<ConvexPolygonShape3D> convex_shape;
        convex_shape.instantiate();
        convex_shape->set_points(convex_points);

        result.push_back(convex_shape);
    }

    return result;
}

void CoACD::set_log_level(LogLevels p_log_level)
{
    log_level = p_log_level;

    std::string_view level_name;
    switch (log_level)
    {
    case LOG_OFF:
        level_name = "off";
        break;
    case LOG_DEBUG:
        level_name = "debug";
        break;
    case LOG_INFO:
        level_name = "info";
        break;
    case LOG_WARNING:
        level_name = "warning";
        break;
    case LOG_ERROR:
        level_name = "error";
        break;
    case LOG_CRITICAL:
        level_name = "critical";
        break;
    default:
        break;
    }
    coacd::set_log_level(level_name);
};

CoACD::LogLevels CoACD::get_log_level()
{
    return log_level;
};

void CoACD::set_print_to_console(bool p_print_to_console)
{
    print_to_console = p_print_to_console;
};

bool CoACD::get_print_to_console()
{
    return print_to_console;
};