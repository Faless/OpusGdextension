#pragma once

#include "godot_cpp/classes/editor_plugin.hpp"

#include "editor_import_plugin_opus.h"
#include "editor_inspector_plugin_audio_stream_opus.h"

using namespace godot;

namespace opus_gdextension {

class EditorPluginOpus : public EditorPlugin {
	GDCLASS(EditorPluginOpus, EditorPlugin);

private:
	Ref<EditorImportPluginOpus> editor_import_plugin_opus;
	Ref<EditorInspectorPluginAudioStreamOpus> editor_inspector_plugin_audio_stream_opus;

protected:
	static void _bind_methods() {}

public:
	void _enter_tree() override;
	void _exit_tree() override;
};

}
