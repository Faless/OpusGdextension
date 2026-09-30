#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/theme.hpp"
#include "godot_cpp/classes/editor_interface.hpp"

#include "editor_plugin_opus.h"

namespace opus_gdextension {

void EditorPluginOpus::_enter_tree() {
	editor_import_plugin_opus.instantiate();
	add_import_plugin(editor_import_plugin_opus);

	// Add icon for AudioStreamOpus
	Ref<Theme> editor_theme = EditorInterface::get_singleton()->get_editor_theme();
	if (editor_theme.is_valid()) {
		Ref<Texture2D> icon_texture = ResourceLoader::get_singleton()->load("res://addons/OpusGdextension/Icons/AudioStreamOpus.svg");
		if (icon_texture.is_valid()) {
			editor_theme->set_icon("AudioStreamOpus", "EditorIcons", icon_texture);
		}
	}
}

void EditorPluginOpus::_exit_tree() {
	remove_import_plugin(editor_import_plugin_opus);
	editor_import_plugin_opus.unref();
}

}
