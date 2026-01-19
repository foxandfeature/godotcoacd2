#include "register_types.h"

#include "coacd.h"
#include "coacd_settings.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void initialize_godotcoacd_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    GDREGISTER_CLASS(CoACD);
    GDREGISTER_CLASS(CoACDSettings);

    memnew(CoACD);
    Engine::get_singleton()->register_singleton("CoACD", CoACD::instance);
}

void uninitialize_godotcoacd_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        memfree(CoACD::instance);
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT godotcoacd_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_godotcoacd_module);
        init_obj.register_terminator(uninitialize_godotcoacd_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}