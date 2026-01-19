#ifndef COACD_SETTINGS_H
#define COACD_SETTINGS_H

#include <godot_cpp/classes/resource.hpp>

namespace godot
{

	class CoACDSettings : public Resource
	{
		GDCLASS(CoACDSettings, Resource)

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
		double threshold;
		int max_convex_hulls;
		PreprocessModes preprocess_mode;
		int preprocess_resolution;
		int sample_resolution;
		int mcts_nodes;
		int mcts_iterations;
		int mcts_max_depth;
		bool pca;
		bool merge_postprocessing;
		bool decimate;
		int max_ch_vertex;
		bool extrude;
		double extrude_margin;
		ApproximationModes approximation_mode;
		unsigned int seed;
	};

}

VARIANT_ENUM_CAST(CoACDSettings::PreprocessModes);
VARIANT_ENUM_CAST(CoACDSettings::ApproximationModes);

#endif // COACD_SETTINGS_H