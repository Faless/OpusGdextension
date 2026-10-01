#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_saver.hpp"

#include "editor_import_plugin_opus.h"
#include "audio_stream_opus.h"

String EditorImportPluginOpus::_get_importer_name() const {
	return "OpusGdextensionImporter";
}

String EditorImportPluginOpus::_get_visible_name() const {
	return "Opus (GDExtension)";
}

PackedStringArray EditorImportPluginOpus::_get_recognized_extensions() const {
	return { "opus" };
}

String EditorImportPluginOpus::_get_save_extension() const {
	return "oggopusstr_gdextension";
}

String EditorImportPluginOpus::_get_resource_type() const {
	return "AudioStreamOpus";
}

TypedArray<Dictionary> EditorImportPluginOpus::_get_import_options(const String &p_path, int32_t p_preset_index) const {
	TypedArray<Dictionary> options;

	Dictionary loop;
	loop["name"] = "loop";
	loop["default_value"] = false;
	options.push_back(loop);

	Dictionary loop_offset;
	loop_offset["name"] = "loop_offset";
	loop_offset["default_value"] = 0.0;
	options.push_back(loop_offset);

	Dictionary bpm;
	bpm["name"] = "bpm";
	bpm["default_value"] = 0.0;
	bpm["property_hint"] = PROPERTY_HINT_RANGE;
	bpm["hint_string"] = "0,400,0.01,or_greater";
	options.push_back(bpm);

	Dictionary beat_count;
	beat_count["name"] = "beat_count";
	beat_count["default_value"] = 0;
	beat_count["property_hint"] = PROPERTY_HINT_RANGE;
	beat_count["hint_string"] = "0,512,or_greater";
	options.push_back(beat_count);

	Dictionary bar_beats;
	bar_beats["name"] = "bar_beats";
	bar_beats["default_value"] = 4;
	bar_beats["property_hint"] = PROPERTY_HINT_RANGE;
	bar_beats["hint_string"] = "2,32,or_greater";
	options.push_back(bar_beats);

	return options;
}

int32_t EditorImportPluginOpus::_get_preset_count() const {
	return 0;
}

String EditorImportPluginOpus::_get_preset_name(int32_t p_preset_index) const {
	return "";
}

Error EditorImportPluginOpus::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
	bool loop = p_options["loop"];
	double loop_offset = p_options["loop_offset"];
	double bpm = p_options["bpm"];
	int beat_count = p_options["beat_count"];
	int bar_beats = p_options["bar_beats"];

	Ref<AudioStreamOpus> opus_stream = AudioStreamOpus::load_from_file(p_source_file);
	if (opus_stream.is_null()) {
		return ERR_CANT_OPEN;
	}

	opus_stream->set_loop(loop);
	opus_stream->set_loop_offset(loop_offset);
	opus_stream->set_bpm(bpm);
	opus_stream->set_beat_count(beat_count);
	opus_stream->set_bar_beats(bar_beats);

	return ResourceSaver::get_singleton()->save(opus_stream, p_save_path + String(".") + _get_save_extension());
}
