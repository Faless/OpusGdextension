#include "register_types.h"

#include "gdextension_interface.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/resource_saver.hpp"

#include "audio_stream_opus.h"
#include "resource_format_loader_opus.h"
#include "resource_format_saver_opus.h"
#include "editor_import_plugin_opus.h"
#include "editor_inspector_plugin_audio_stream_opus.h"
#include "editor_plugin_opus.h"

using namespace godot;
using namespace opus_gdextension;

static Ref<ResourceFormatLoaderOpus> resource_format_loader_opus;
static Ref<ResourceFormatSaverOpus> resource_format_saver_opus;

void initialize_opus_gdextension_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(AudioStreamPlaybackOpus);
		GDREGISTER_CLASS(AudioStreamOpus);
		GDREGISTER_CLASS(ResourceFormatLoaderOpus);
		GDREGISTER_CLASS(ResourceFormatSaverOpus);

		resource_format_loader_opus.instantiate();
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_format_loader_opus, true);

		resource_format_saver_opus.instantiate();
		ResourceSaver::get_singleton()->add_resource_format_saver(resource_format_saver_opus, true);
	}

	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(EditorImportPluginOpus);
		GDREGISTER_CLASS(EditorInspectorPluginAudioStreamOpusEditor);
		GDREGISTER_CLASS(EditorInspectorPluginAudioStreamOpus);
		GDREGISTER_CLASS(EditorPluginOpus);

		EditorPlugins::add_by_type<EditorPluginOpus>();
	}
}

void uninitialize_opus_gdextension_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ResourceLoader::get_singleton()->remove_resource_format_loader(resource_format_loader_opus);
		resource_format_loader_opus.unref();

		ResourceSaver::get_singleton()->remove_resource_format_saver(resource_format_saver_opus);
		resource_format_saver_opus.unref();
	}

	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::remove_by_type<EditorPluginOpus>();
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT opus_gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_opus_gdextension_module);
	init_obj.register_terminator(uninitialize_opus_gdextension_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
