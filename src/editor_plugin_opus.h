#pragma once

#include "editor_import_plugin_opus.h"
#include <godot_cpp/classes/editor_plugin.hpp>

using namespace godot;

namespace opus_gdextension {

class EditorPluginOpus : public EditorPlugin {
	GDCLASS(EditorPluginOpus, EditorPlugin);

private:
	Ref<EditorImportPluginOpus> editor_import_plugin_opus;

protected:
	static void _bind_methods() {}

public:
	void _enter_tree() override;
	void _exit_tree() override;
};

}
