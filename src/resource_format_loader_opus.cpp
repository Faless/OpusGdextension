#include "godot_cpp/classes/resource_saver.hpp"

#include <ogg/ogg.h>
#include <opus/opus.h>

#include "resource_format_loader_opus.h"
#include "audio_stream_opus.h"

using namespace godot;

namespace opus_gdextension {

PackedStringArray ResourceFormatLoaderOpus::_get_recognized_extensions() const {
	return { "opus" };
}

bool ResourceFormatLoaderOpus::_handles_type(const StringName &p_type) const {
	return ClassDB::is_parent_class(p_type, "AudioStream");
}

String ResourceFormatLoaderOpus::_get_resource_type(const String &p_path) const {
	String extension = p_path.get_extension().to_lower();
	if (extension == "opus") {
		return "AudioStreamOpus";
	}
	return "";
}

Variant ResourceFormatLoaderOpus::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	Ref<AudioStreamOpus> opus_stream = AudioStreamOpus::load_from_file(p_path);
	if (opus_stream.is_null()) {
		return ERR_CANT_OPEN;
	}
	return opus_stream;
}

void ResourceFormatLoaderOpus::_bind_methods() {
}

ResourceFormatLoaderOpus::ResourceFormatLoaderOpus() {
}

}