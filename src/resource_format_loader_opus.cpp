#include "godot_cpp/classes/resource_uid.hpp"

#include <ogg/ogg.h>
#include <opus/opus.h>

#include "resource_format_loader_opus.h"
#include "audio_stream_opus.h"

using namespace godot;

namespace opus_gdextension {

PackedStringArray ResourceFormatLoaderOpus::_get_recognized_extensions() const {
	return { "oggopusstr_gdextension" };
}

bool ResourceFormatLoaderOpus::_handles_type(const StringName &p_type) const {
	return p_type == SNAME_AudioStreamOpus || ClassDB::is_parent_class(p_type, SNAME_AudioStreamOpus);
}

String ResourceFormatLoaderOpus::_get_resource_type(const String &p_path) const {
	String extension = p_path.get_extension().to_lower();
	if (extension == "oggopusstr_gdextension") {
		return "AudioStreamOpus";
	}
	return "";
}

Variant ResourceFormatLoaderOpus::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	Ref<AudioStreamOpus> opus_stream = AudioStreamOpus::load_from_file(p_path);
	if (opus_stream.is_null()) {
		UtilityFunctions::printerr("Failed to load AudioStreamOpus at path: ", p_path);
		return ERR_FILE_CANT_READ;
	}
	return opus_stream;
}

}
