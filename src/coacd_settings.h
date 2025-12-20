#ifndef COACD_SETTINGS_H
#define COACD_SETTINGS_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/ref.hpp>

namespace godot
{

	class CoACDSettings : public RefCounted
	{
		GDCLASS(CoACDSettings, RefCounted)

	protected:
		static void _bind_methods();

	public:
		CoACDSettings();
		~CoACDSettings();

		enum PreprocessModes
		{
			AUTO,
			ON,
			OFF
		};

		enum ApproximationModes
		{
			CONVEX,
			BOX
		};

		void set_threshold(const double p_treshold);
		double get_threshold() const;

		void set_max_convex_hulls(const int p_max_convex_hulls);
		int get_max_convex_hulls() const;

		void set_preprocess_mode(const PreprocessModes p_preprocess_mode);
		PreprocessModes get_preprocess_mode() const;

		void set_preprocess_resolution(const int p_preprocess_resolution);
		int get_preprocess_resolution() const;

		void set_sample_resolution(const int p_sample_resolution);
		int get_sample_resolution() const;

		void set_mcts_nodes(const int p_mcts_nodes);
		int get_mcts_nodes() const;

		void set_mcts_iterations(const int p_mcts_iterations);
		int get_mcts_iterations() const;

		void set_mcts_max_depth(const int p_mcts_max_depth);
		int get_mcts_max_depth() const;

		void set_pca(const bool p_pca);
		bool get_pca() const;

		void set_merge_postprocessing(const bool p_merge_postprocessing);
		bool get_merge_postprocessing() const;

		void set_decimate(const bool p_decimate);
		bool get_decimate() const;

		void set_max_ch_vertex(const int p_max_ch_vertex);
		int get_max_ch_vertex() const;

		void set_extrude(const bool p_extrude);
		bool get_extrude() const;

		void set_extrude_margin(const double p_extrude_margin);
		double get_extrude_margin() const;

		void set_approximation_mode(const ApproximationModes p_approximation_mode);
		ApproximationModes get_approximation_mode() const;

		void set_seed(const unsigned int p_seed);
		unsigned int get_seed() const;

	private:
		double threshold = 0.05;
		int max_convex_hulls = -1;
		PreprocessModes preprocess_mode = OFF;
		int preprocess_resolution = 50;
		int sample_resolution = 2000;
		int mcts_nodes = 20;
		int mcts_iterations = 150;
		int mcts_max_depth = 3;
		bool pca = false;
		bool merge_postprocessing = true;
		bool decimate = false;
		int max_ch_vertex = 256;
		bool extrude = false;
		double extrude_margin = 0.01;
		ApproximationModes approximation_mode = CONVEX;
		unsigned int seed = 0;
	};

}

VARIANT_ENUM_CAST(CoACDSettings::PreprocessModes);
VARIANT_ENUM_CAST(CoACDSettings::ApproximationModes);

#endif // COACD_SETTINGS_H