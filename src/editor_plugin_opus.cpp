#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/theme.hpp"
#include "godot_cpp/classes/editor_interface.hpp"

#include "editor_plugin_opus.h"

namespace opus_gdextension {

void EditorPluginOpus::_enter_tree() {
	editor_import_plugin_opus.instantiate();
	add_import_plugin(editor_import_plugin_opus);

	editor_inspector_plugin_audio_stream_opus.instantiate();
	add_inspector_plugin(editor_inspector_plugin_audio_stream_opus);
}

void EditorPluginOpus::_exit_tree() {
	remove_import_plugin(editor_import_plugin_opus);
	editor_import_plugin_opus.unref();

	remove_inspector_plugin(editor_inspector_plugin_audio_stream_opus);
	editor_inspector_plugin_audio_stream_opus.unref();
}

}
