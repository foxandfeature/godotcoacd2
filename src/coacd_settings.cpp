#include "coacd_settings.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void CoACDSettings::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_threshold", "p_threshold"), &CoACDSettings::set_threshold);
    ClassDB::bind_method(D_METHOD("get_threshold"), &CoACDSettings::get_threshold);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold", PROPERTY_HINT_RANGE, "0.01,1"), "set_threshold", "get_threshold");

    ClassDB::bind_method(D_METHOD("set_max_convex_hulls", "p_max_convex_hulls"), &CoACDSettings::set_max_convex_hulls);
    ClassDB::bind_method(D_METHOD("get_max_convex_hulls"), &CoACDSettings::get_max_convex_hulls);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_convex_hulls"), "set_max_convex_hulls", "get_max_convex_hulls");

    ClassDB::bind_method(D_METHOD("set_preprocess_mode", "p_preprocess_mode"), &CoACDSettings::set_preprocess_mode);
    ClassDB::bind_method(D_METHOD("get_preprocess_mode"), &CoACDSettings::get_preprocess_mode);
    BIND_ENUM_CONSTANT(AUTO);
    BIND_ENUM_CONSTANT(ON);
    BIND_ENUM_CONSTANT(OFF);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "preprocess_mode", PROPERTY_HINT_ENUM, "Auto,On,Off"), "set_preprocess_mode", "get_preprocess_mode");

    ClassDB::bind_method(D_METHOD("set_preprocess_resolution", "p_preprocess_resolution"), &CoACDSettings::set_preprocess_resolution);
    ClassDB::bind_method(D_METHOD("get_preprocess_resolution"), &CoACDSettings::get_preprocess_resolution);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "preprocess_resolution", PROPERTY_HINT_RANGE, "5,1000"), "set_preprocess_resolution", "get_preprocess_resolution");

    ClassDB::bind_method(D_METHOD("set_sample_resolution", "p_sample_resolution"), &CoACDSettings::set_sample_resolution);
    ClassDB::bind_method(D_METHOD("get_sample_resolution"), &CoACDSettings::get_sample_resolution);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "sample_resolution"), "set_sample_resolution", "get_sample_resolution");

    ClassDB::bind_method(D_METHOD("set_mcts_nodes", "p_mcts_nodes"), &CoACDSettings::set_mcts_nodes);
    ClassDB::bind_method(D_METHOD("get_mcts_nodes"), &CoACDSettings::get_mcts_nodes);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mcts_nodes"), "set_mcts_nodes", "get_mcts_nodes");

    ClassDB::bind_method(D_METHOD("set_mcts_iterations", "p_mcts_iterations"), &CoACDSettings::set_mcts_iterations);
    ClassDB::bind_method(D_METHOD("get_mcts_iterations"), &CoACDSettings::get_mcts_iterations);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mcts_iterations"), "set_mcts_iterations", "get_mcts_iterations");

    ClassDB::bind_method(D_METHOD("set_mcts_max_depth", "p_mcts_max_depth"), &CoACDSettings::set_mcts_max_depth);
    ClassDB::bind_method(D_METHOD("get_mcts_max_depth"), &CoACDSettings::get_mcts_max_depth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mcts_max_depth"), "set_mcts_max_depth", "get_mcts_max_depth");

    ClassDB::bind_method(D_METHOD("set_pca", "p_pca"), &CoACDSettings::set_pca);
    ClassDB::bind_method(D_METHOD("get_pca"), &CoACDSettings::get_pca);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pca"), "set_pca", "get_pca");

    ClassDB::bind_method(D_METHOD("set_merge_postprocessing", "p_merge_postprocessing"), &CoACDSettings::set_merge_postprocessing);
    ClassDB::bind_method(D_METHOD("get_merge_postprocessing"), &CoACDSettings::get_merge_postprocessing);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "merge_postprocessing"), "set_merge_postprocessing", "get_merge_postprocessing");

    ClassDB::bind_method(D_METHOD("set_decimate", "p_decimate"), &CoACDSettings::set_decimate);
    ClassDB::bind_method(D_METHOD("get_decimate"), &CoACDSettings::get_decimate);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "decimate"), "set_decimate", "get_decimate");

    ClassDB::bind_method(D_METHOD("set_max_ch_vertex", "p_max_ch_vertex"), &CoACDSettings::set_max_ch_vertex);
    ClassDB::bind_method(D_METHOD("get_max_ch_vertex"), &CoACDSettings::get_max_ch_vertex);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_ch_vertex"), "set_max_ch_vertex", "get_max_ch_vertex");

    ClassDB::bind_method(D_METHOD("set_extrude", "p_extrude"), &CoACDSettings::set_extrude);
    ClassDB::bind_method(D_METHOD("get_extrude"), &CoACDSettings::get_extrude);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "extrude"), "set_extrude", "get_extrude");

    ClassDB::bind_method(D_METHOD("set_extrude_margin", "p_extrude_margin"), &CoACDSettings::set_extrude_margin);
    ClassDB::bind_method(D_METHOD("get_extrude_margin"), &CoACDSettings::get_extrude_margin);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "extrude_margin"), "set_extrude_margin", "get_extrude_margin");

    ClassDB::bind_method(D_METHOD("set_approximation_mode", "p_approximation_mode"), &CoACDSettings::set_approximation_mode);
    ClassDB::bind_method(D_METHOD("get_approximation_mode"), &CoACDSettings::get_approximation_mode);
    BIND_ENUM_CONSTANT(CONVEX);
    BIND_ENUM_CONSTANT(BOX);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "approximation_mode", PROPERTY_HINT_ENUM, "Convex,Box"), "set_approximation_mode", "get_approximation_mode");

    ClassDB::bind_method(D_METHOD("set_seed", "p_seed"), &CoACDSettings::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &CoACDSettings::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
}

CoACDSettings::CoACDSettings() {}
CoACDSettings::~CoACDSettings() {}

void CoACDSettings::set_threshold(const double p_threshold) { threshold = p_threshold; }
double CoACDSettings::get_threshold() const { return threshold; }

void CoACDSettings::set_max_convex_hulls(const int p_max_convex_hulls) { max_convex_hulls = p_max_convex_hulls; }
int CoACDSettings::get_max_convex_hulls() const { return max_convex_hulls; }

void CoACDSettings::set_preprocess_mode(const PreprocessModes p_preprocess_mode) { preprocess_mode = p_preprocess_mode; }
CoACDSettings::PreprocessModes CoACDSettings::get_preprocess_mode() const { return preprocess_mode; }

void CoACDSettings::set_preprocess_resolution(const int p_preprocess_resolution) { preprocess_resolution = p_preprocess_resolution; }
int CoACDSettings::get_preprocess_resolution() const { return preprocess_resolution; }

void CoACDSettings::set_sample_resolution(const int p_sample_resolution) { sample_resolution = p_sample_resolution; }
int CoACDSettings::get_sample_resolution() const { return sample_resolution; }

void CoACDSettings::set_mcts_nodes(const int p_mcts_nodes) { mcts_nodes = p_mcts_nodes; }
int CoACDSettings::get_mcts_nodes() const { return mcts_nodes; }

void CoACDSettings::set_mcts_iterations(const int p_mcts_iterations) { mcts_iterations = p_mcts_iterations; }
int CoACDSettings::get_mcts_iterations() const { return mcts_iterations; }

void CoACDSettings::set_mcts_max_depth(const int p_mcts_max_depth) { mcts_max_depth = p_mcts_max_depth; }
int CoACDSettings::get_mcts_max_depth() const { return mcts_max_depth; }

void CoACDSettings::set_pca(const bool p_pca) { pca = p_pca; }
bool CoACDSettings::get_pca() const { return pca; }

void CoACDSettings::set_merge_postprocessing(const bool p_merge_postprocessing) { merge_postprocessing = p_merge_postprocessing; }
bool CoACDSettings::get_merge_postprocessing() const { return merge_postprocessing; }

void CoACDSettings::set_decimate(const bool p_decimate) { decimate = p_decimate; }
bool CoACDSettings::get_decimate() const { return decimate; }

void CoACDSettings::set_max_ch_vertex(const int p_max_ch_vertex) { max_ch_vertex = p_max_ch_vertex; }
int CoACDSettings::get_max_ch_vertex() const { return max_ch_vertex; }

void CoACDSettings::set_extrude(const bool p_extrude) { extrude = p_extrude; }
bool CoACDSettings::get_extrude() const { return extrude; }

void CoACDSettings::set_extrude_margin(const double p_extrude_margin) { extrude_margin = p_extrude_margin; }
double CoACDSettings::get_extrude_margin() const { return extrude_margin; }

void CoACDSettings::set_approximation_mode(const ApproximationModes p_approximation_mode) { approximation_mode = p_approximation_mode; }
CoACDSettings::ApproximationModes CoACDSettings::get_approximation_mode() const { return approximation_mode; }

void CoACDSettings::set_seed(const unsigned int p_seed) { seed = p_seed; }
unsigned int CoACDSettings::get_seed() const { return seed; }