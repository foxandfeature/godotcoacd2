#ifndef COACD_H
#define COACD_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/convex_polygon_shape3d.hpp>
#include "coacd_settings.h"

namespace godot
{

	class CoACD : public Object
	{
		GDCLASS(CoACD, Object)

	protected:
		static void _bind_methods();

	public:
		CoACD();
		~CoACD();

		static CoACD *instance;

		enum LogLevels
		{
			OFF,
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			CRITICAL
		};

		Array convex_decomposition(const Ref<ConcavePolygonShape3D> mesh, const Ref<CoACDSettings> = Ref<CoACDSettings>());

		void set_log_level(LogLevels p_log_level);
		LogLevels get_log_level();

		void set_print_to_console(bool p_print_to_console);
		bool get_print_to_console();

	private:
		LogLevels log_level = WARNING;
		bool print_to_console = true;
	};

}

VARIANT_ENUM_CAST(CoACD::LogLevels);

#endif // COACD_H